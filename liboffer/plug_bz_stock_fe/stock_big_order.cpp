#include "stock_big_order.h"
#include "math/math_util.h"
#include "sq_fe_struct.h"
using namespace sq;

#define BSFLAG_B 'B'
#define BSFLAG_S 'S'
namespace sq_plug
{
    /** 判断是否为大单
     * 天狼的大单判断标准：以下三个条件只要符合一个即是大单。
     * 1、成交的原始委托大于等于500手(500000股)
     * 2、成交额在50万元以上
     * 3、成交额占流通市值0.1%
     * @param to  成交额
     * @param vol 成交量
     */
    bool stock_big_order::is_big_order(double to, int vol)
    {
        if (vol >= BigOrder_Base_VOL ||
            math_util::double_compare(to, BigOrder_Base_AMT) >= 0 ||
            (share_base > 0 && math_util::double_compare(vol, share_base) >= 0))
        {
            return true;
        }
        return false;
    }

    /** 天狼标准
     * @param to 成交额
     * @param vol 成交量
     * @return 如果是大单，返回这次应该增加的成交额
     */
    double stock_big_order::boDeltaTL(stock_match_record &m, double to, int vol)
    {
        double tempDelta = 0;
        if (is_big_order(m.turnover, m.match_qty))
        { // 已经是大单
            tempDelta = to;
        }
        else if (is_big_order(m.turnover + to, m.match_qty + vol))
        { // 此刻变成大单
            tempDelta = m.turnover + to;
        }
        return tempDelta;
    }
    int stock_big_order::process_match(stock_match_quot *tran)
    {
        if (tran->ExecType != 'F')
            return 1;
        int64_t buyOrderID = tran->BidApplSeqNum;
        int64_t sellOrderID = tran->OfferApplSeqNum;

        double tempTradeVol = tran->TradeQty;

        double tempTradeprice = tran->TradePrice;
        tempTradeprice = tempTradeprice / 10000.0;

        double tempTradeAmt = tempTradeVol * tempTradeprice;
        rt_trade_amt += tempTradeAmt;

        char bsflag = tran->TradeBSFlag;
        if (bsflag == 'N')
        {
            if (buyOrderID > sellOrderID && sellOrderID > 0)
            {
                bsflag = BSFLAG_B;
            }
            else if (buyOrderID < sellOrderID && buyOrderID > 0)
            {
                bsflag = BSFLAG_S;
            }
            else
            {
                return 1;
            }
            // tran->TradeBSFlag=bsflag;
        }

        auto it_buy_mo = m_other_buy.find(buyOrderID);
        auto it_sell_mo = m_other_sell.find(sellOrderID);

        if (bsflag == BSFLAG_B)
        {
            if (m_other_sell.end() != it_sell_mo && it_sell_mo->second.is_active)
            {
                rt_vip_activesell_amt += boDeltaTL(it_sell_mo->second, tempTradeAmt, tempTradeVol); // 34.大户主卖出金额,天狼标准
            }
        }
        else if (bsflag == BSFLAG_S)
        {
            if (m_other_buy.end() != it_buy_mo && it_buy_mo->second.is_active)
            {
                rt_vip_activebuy_amt += boDeltaTL(it_buy_mo->second, tempTradeAmt, tempTradeVol); // 26.大户主买入金额,天狼标准
            }
        }
        else
        {
            assert(false);
        }

        // 非主动指标-1.大户资金净流入
        if (m_other_buy.end() == it_buy_mo)
        {
            if (is_big_order(tempTradeAmt, tempTradeVol))
            {                                   // 天狼标准
                rt_vip_buy_amt += tempTradeAmt; // 1003.当日大户买入成交金额
                if (bsflag == BSFLAG_B)
                {
                    rt_vip_activebuy_amt += tempTradeAmt; // 26.大户主买入金额
                }
            }
            stock_match_record buyTempMo(buyOrderID, tempTradeVol, tempTradeprice);
            if (bsflag == BSFLAG_B)
            {
                buyTempMo.is_active = true; // 主动
            }
            m_other_buy[buyOrderID] = buyTempMo;
        }
        else
        {
            rt_vip_buy_amt += boDeltaTL(it_buy_mo->second, tempTradeAmt, tempTradeVol); // 1003.当日大户买入成交金额 ,天狼标准
            if (bsflag == BSFLAG_B && it_buy_mo->second.is_active)
            {
                rt_vip_activebuy_amt += boDeltaTL(it_buy_mo->second, tempTradeAmt, tempTradeVol); // 26.大户主买入金额,天狼标准
            }
            it_buy_mo->second.add(tempTradeVol, tempTradeprice);
        }

        if (m_other_sell.end() == it_sell_mo)
        {
            if (is_big_order(tempTradeAmt, tempTradeVol))
            {                                    // 天狼标准
                rt_vip_sell_amt += tempTradeAmt; // 1004.当日大户卖出成交金额
                if (bsflag == BSFLAG_S)
                {
                    rt_vip_activesell_amt += tempTradeAmt; ////34.大户主卖出金额
                }
            }
            stock_match_record sellTempMo(sellOrderID, tempTradeVol, tempTradeprice);
            if (bsflag == BSFLAG_S)
            {
                sellTempMo.is_active = true; // 主动
            }
            m_other_sell[sellOrderID] = sellTempMo;
        }
        else
        {
            rt_vip_sell_amt += boDeltaTL(it_sell_mo->second, tempTradeAmt, tempTradeVol); // 1004.当日大户卖出成交金额,天狼标准
            if (bsflag == BSFLAG_S && it_sell_mo->second.is_active)
            {
                rt_vip_activesell_amt += boDeltaTL(it_sell_mo->second, tempTradeAmt, tempTradeVol); // 34.大户主卖出金额,天狼标准
            }
            it_sell_mo->second.add(tempTradeVol, tempTradeprice);
        }

        rt_vip_active_inflow = rt_vip_activebuy_amt - rt_vip_activesell_amt;
        rt_vip_inflow = rt_vip_buy_amt - rt_vip_sell_amt;
        // printf("rt_vip_buy_amt=%f,rt_vip_sell_amt=%f\n",rt_vip_buy_amt,rt_vip_sell_amt);
        return 0;
    }

    void stock_big_order::put(int tid, char *data, int size)
    {
        if (tid != tid_stock_match_quot)
        {
            return;
        }
        stock_match_quot *tran = (stock_match_quot *)data;
        process_match(tran);
    }

    stock_big_orders::~stock_big_orders()
    {
        auto it = m_fes.begin();
        for (; it != m_fes.end(); ++it)
        {
            delete it->second;
        }
        m_fes.clear();
    }
    void stock_big_orders::add_stock(int idx, double w, double s)
    {
        m_fes[idx] = new stock_big_order(s);
    }
    void stock_big_orders::put(int tid, char *data, int size)
    {
        if (tid != tid_stock_match_quot)
        {
            return;
        }
        stock_match_quot *tran = (stock_match_quot *)data;
        int idx = std::atoi(tran->contract.data());
        auto it = m_fes.find(idx);
        if (it == m_fes.end())
        {
            return;
        }
        it->second->put(tid, data, size);
    }

    void stock_big_orders::calc_result(void *result)
    {
        stock_fe_result *ret = (stock_fe_result *)result;
        double tempRt_vip_active_inflow = 0;
        double tempRt_vip_inflow = 0;
        double big_all_turnover = 0;
        auto it1 = m_fes.begin();
        for (; it1 != m_fes.end(); ++it1)
        {
            tempRt_vip_active_inflow += it1->second->rt_vip_active_inflow;
            tempRt_vip_inflow += it1->second->rt_vip_inflow;
            // it1->second.moment_process->reset();

            big_all_turnover += it1->second->rt_trade_amt;
        }
        ret->rtVipActiveInflow = tempRt_vip_active_inflow;
        ret->rtVipInflow = tempRt_vip_inflow;
        ret->rtTurnover = big_all_turnover;
    }
}