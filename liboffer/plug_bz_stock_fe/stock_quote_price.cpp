#include "stock_quote_price.h"
#include "sq_fe_struct.h"
#include "math/math_util.h"
#include "log/sq_logger.h"
using namespace sq;

namespace sq_plug
{
    void stock_quote_price::add_stock(int idx, double w, double s)
    {
        m_fes[idx] = stock_price_fe_info();
        m_fes[idx].weight = w;
    }
    /*
1、单只股票计算：
非涨跌停板情况下
ABDPDiffF:=(SP2-SP1)-(BP1-BP2)
ABDPDiffM:=((SP1+SP2+SP3+SP4+SP5)/5-SP1)-(BP1-(BP1+BP2+BP3+BP4+BP5)/5)
ABVDiffF:=SV1-BV1
ABVDiffM:=(SV1+SV2+SV3+SV4+SV5)/5-(BV1+BV2+BV3+BV4+BV5)/5
涨跌停板情况下，上述四个值=0

2、指数合成：各个股票相加，得到指数4个特征值
*/
    void stock_quote_price::put(int tid,char*data,int size)
    {
        if(tid!=tid_market_data){
            return;
        }
        sq_quot *table = (sq_quot*)data;
        assert(table->contract_idx>0);
        auto it = m_fes.find(table->contract_idx);
        if (it == m_fes.end())
        {
            return;
        }
        if (it->second.open == 0)
        {
            if (table->open > 0)
            {
                it->second.open = table->open;
            }
            if (table->price > 0)
            {
                it->second.open = table->price;
                log_info("stock {} open price open={},price={}\n",
                        table->contract_idx,
                        table->open,
                        table->price);
            }
            else
            {
                log_warn("stock {} open price error,open={},price={}\n",
                        table->contract_idx,
                        table->open,
                        table->price);
            }
        }

        if (table->mbl_buy_price[0] == 0 ||
            table->mbl_sell_price[0] == 0)
        {
            it->second.mid_price = table->price;
        }
        else
        {
            it->second.mid_price = (table->mbl_buy_price[0] + table->mbl_sell_price[0]) * 0.5;
        }
        if (it->second.open == 0)
        {
            it->second.idx_price = 0;
        }
        else
        {
            it->second.idx_price = 1000.0 * (it->second.mid_price - it->second.open) / it->second.open;
        }

        it->second.volume = table->match_qty;

#if 1
        // 计算特征值
        if (sq::math_util::double_compare(table->price, table->limit_up) == 0)
        { // 打板
            it->second.ABDPDiffF = 0;
            it->second.ABDPDiffM = 0;
            it->second.ABVDiffF = 0;
            it->second.ABVDiffM = 0;
        }
        else
        {
            double SP1 = table->mbl_sell_price[0];
            double SP2 = table->mbl_sell_price[1];
            double SP3 = table->mbl_sell_price[2];
            double SP4 = table->mbl_sell_price[3];
            double SP5 = table->mbl_sell_price[4];

            double BP1 = table->mbl_buy_price[0];
            double BP2 = table->mbl_buy_price[1];
            double BP3 = table->mbl_buy_price[2];
            double BP4 = table->mbl_buy_price[3];
            double BP5 = table->mbl_buy_price[4];

            double SV1 = table->mbl_sell_qty[0];
            double SV2 = table->mbl_sell_qty[1];
            double SV3 = table->mbl_sell_qty[2];
            double SV4 = table->mbl_sell_qty[3];
            double SV5 = table->mbl_sell_qty[4];

            double BV1 = table->mbl_buy_qty[0];
            double BV2 = table->mbl_buy_qty[1];
            double BV3 = table->mbl_buy_qty[2];
            double BV4 = table->mbl_buy_qty[3];
            double BV5 = table->mbl_buy_qty[4];
            if (SP1 > 0 && BP1 > 0 && SP2 > 0 && BP2 > 0 && SP2 > SP1 && BP1 > BP2)
            {
                it->second.ABDPDiffF = (SP2 - SP1) - (BP1 - BP2);

                it->second.ABDPDiffM = ((SP1 + SP2 + SP3 + SP4 + SP5) / 5.0 - SP1) - (BP1 - (BP1 + BP2 + BP3 + BP4 + BP5) / 5.0);
            }
            else
            {
                it->second.ABDPDiffF = 0;
                it->second.ABDPDiffM = 0;
            }
            it->second.ABVDiffF = SV1 - BV1;
            it->second.ABVDiffM = (SV1 + SV2 + SV3 + SV4 + SV5) / 5.0 - (BV1 + BV2 + BV3 + BV4 + BV5) / 5.0;
        }
#endif
    }
    void stock_quote_price::reset()
    {
        auto it = m_fes.begin();
        for (; it != m_fes.end(); ++it)
        {
            it->second.ABDPDiffF = 0;
            it->second.ABDPDiffM = 0;
            it->second.ABVDiffF = 0;
            it->second.ABVDiffM = 0;
            it->second.idx_price = 0;
            it->second.mid_price = 0;
            it->second.open = 0;
            it->second.volume = 0;
        }
    }

    void stock_quote_price::calc_result(void *result)
    {
        stock_fe_result *ret = (stock_fe_result *)result;
        int stock_opened = 0;
        double tempIdxPrice = 0;
        int64_t tempVolume = 0;
        double ABDPDiffF = 0;
        double ABDPDiffM = 0;
        double ABVDiffF = 0;
        double ABVDiffM = 0;
        double ActiveVol = 0;
        auto it = m_fes.begin();
        for (; it != m_fes.end(); ++it)
        {
            if (it->second.open > 0)
            {
                stock_opened++;
            }
            tempIdxPrice += it->second.idx_price * it->second.weight;
            // 总成交量
            tempVolume += it->second.volume;

            ABDPDiffF += it->second.ABDPDiffF;
            ABDPDiffM += it->second.ABDPDiffM;
            ABVDiffF += it->second.ABVDiffF;
            ABVDiffM += it->second.ABVDiffM;
        }
        ret->stock_opened = stock_opened;
        ret->idxPrice = tempIdxPrice;
        ret->volume = tempVolume;
        ret->ABDPDiffF = ABDPDiffF;
        ret->ABDPDiffM = ABDPDiffM;
        ret->ABVDiffF = ABVDiffF;
        ret->ABVDiffM = ABVDiffM;
    }
}