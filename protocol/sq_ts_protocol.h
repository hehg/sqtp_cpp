#pragma once

namespace sq{
    namespace ts{
        //合约 证券 资产 信息
        struct symbol_info
        {
            std::string symbol;             //合约
            std::string exchange;           //交易所
            std::string symbol_name;               //名称
            std::string type;               //合约类别

            std::string product_id;
            std::string product_class;      //产品类别

            std::string trade_date_begin;   //开始交易日
            std::string trade_date_end;     //开始交易日
            std::string deliv_month;        //交割月
            std::string deliv_year;         //交割年
            std::string deliv_type;         //交割方式
            std::string deliv_date_begin;        //交割日
            std::string deliv_date_end;         //交割日
            std::string expiry_date;        //到期日
            std::string status;            //状态
            double  benchmark_price;       //挂牌基准价
            std::string settle_currency; //结算货币

            int  multiple;           //合约乘数
            int  price_tick;         //最小变动价位

            int  max_limit_order_vol; //最大下单手数
            int  min_limit_order_vol; //最小下单手数
            int  max_market_order_vol;//最大市价下单手数
            int  min_market_order_vol;//最小市价下单手数
            int  max_pos_vol;  //最大持仓量
           // int  min_pos_limit_order_vol;
            
            //期权
            std::string strike_price;   //执行价格
            std::string option_type;    //期权类型
            std::string exercise_type;  //行权类型
            std::string call_pull_flag; //看涨看跌
            std::string underlying_symbol; //标的合约

            //套利

        };
        

        //订单请求

        //订单应答

        //行情

        //成交通知
        //订单信息
        struct order_info
        {
            std::string src_id; //订单来源
            std::string order_id;
            std::string local_id;
            std::string account_id;
            std::string client_id;
            std::string trader_id; //下单席位
            std::string exchange_id;
            std::string symbol;
            std::string order_type;
            std::string direction;
            std::string offset; //开平
            std::string hedge; //投机套保
            std::string status;
            std::string volume;
            std::string remain_volume;
            std::string price;
            std::string stop_price;
            std::string order_date;
            std::string order_time;
            std::string time_condition;
            std::string volume_condition;
            std::string order_source;
            std::string triget_time;

            std::string trade_volume;//成交数量
            std::string trade_last_price; //最后一次的成交价格
            std::string trade_amount; //成交金额
            std::string trade_date; //最有一次成交日期
            std::string trade_time; //最后一次成交时间

            std::string cancel_date;
            std::string cancel_time;
            std::string cancel_volume; //撤单数量
            std::string cancel_trader; //
            

            std::string frozen_margin;//冻结的保证金
            std::string frozen_fee;//冻结的手续费
        };
        //资金信息
        struct fund_info
        {
            std::string account_id;
            std::string exchange_id;
            std::string client_id;  //交易编码
            std::string settle_currency; //结算货币

            std::string balance;    //结算准备金
            std::string available;   //可用资金
            std::string withdraw_available; //可取资金
            std::string margin;   //
            std::string exchange_margin;
            std::string deliv_margin;
            std::string deliv_exchange_margin;
            std::string fee;
            std::string fee_exchange;
            std::string report_fee;

            std::string fund_in;  //入金
            std::string fund_out; //出金

            std::string last_margin;  //上日保证金
            std::string last_mortage; //上日质押金额
            std::string last_balance; //上日准备金
            std::string last_deposit; //上日存款

            //冻结
            std::string frozen; //冻结总资金
            std::string frozen_margin;
            std::string frozen_exchange_margin;
            std::string frozen_fee;  //冻结手续费
            std::string frozen_premium; //冻结权利金
            std::string frozen_execution_margin; //行权冻结保证金
            std::string frozen_execution_fee;    //行权冻结费用

            std::string close_profit; //平仓盈利 盯市
            std::string close_loss;  //平仓亏损 盯市
            std::string close_profitloass; //平仓逐笔盈亏
            std::string float_profit;
            std::string float_loss;
            std::string float_profitloss;//浮动逐笔盈亏
            
            //占用
            std::string posi_used_margin;   //持仓占用保证金
            std::string posi_used_exchange_margin;
        };
        
        //持仓汇总信息
        struct position_summer_info
        {
            std::string account_id;
            std::string client_id;
            std::string symbol;
            std::string exchange_id;
            std::string direction;
            std::string hedge; //投机套保

            std::string volume;             //总持仓数量
            std::string last_position_vol;  //昨仓
            std::string today_position_vol; //今仓
            std::string frozen_position_vol;//冻结持仓
            std::string frozen_last_posi_vol;//冻结昨仓
            std::string frozen_today_posi_vol;//冻结今仓
            std::string amount;//持仓金额
            std::string hold_avg_price; //持仓均价
            std::string open_avg_price; //开仓均价

            std::string last_margin; //昨保证金
            std::string today_margin;//今保证金
            std::string margin;//当前保证金

            std::string fee; //费用
            std::string option_cap;//期权市值

            std::string close_profit;
            std::string close_loss;
            std::string close_profitloss;//逐笔盈亏
            std::string float_profit;
            std::string float_loss;
            std::string float_profitloss;//逐笔盈亏
        };

        //持仓明细
        stru position_detail_info
        {
            std::string account_id;
            std::string client_id;
            std::string symbol;
            std::string exchange_id;
            std::string direction;
            std::string hedge; //投机套保
            std::string open_date;

            std::string open_price;     //开仓价格
            std::string posi_type;      //今仓 昨仓
            std::string volume;         //当前持仓量
            std::string closed_volume;  //已平数量
            std::string frozen_volume;
            std::string close_profit;   //平仓盈利
            std::string close_loss;     //平仓亏损
            std::string last_settlement_price;//昨结算

        };
    }
}