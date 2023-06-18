#pragma once
#include <stdint.h>
#include "container/byte_array.h"
namespace sq
{
    typedef unsigned short tid_t;
    using sys_order_id_t = byte_array<32>;
    using match_id_t = uint64_t;
    using user_id_t = byte_array<32>;
    using timestamp_t = uint64_t;

    using stock_id_t = byte_array<32>;
    using product_id_t = byte_array<32>;
    using contract_t = byte_array<32>;
    using contract_name_t = byte_array<32>;
    using trade_date_t = byte_array<9>;  //日期:yyyymmdd
    using trade_time_t = byte_array<16>; //时间戳hh:mm:ss.sss

    #define market_dce "dce"
    #define market_shfe "shfe";
    #define market_czce "czce"
    #define market_ine "ine"
    #define market_cffex "cffex"
    #define market_sse "sse"
    #define market_szse "szse"

    using market_id_t = byte_array<9>; //市场编码
    using variety_t = byte_array<9>;
    using price_t = double;
    using qty_t = int;
    using err_msg_t = byte_array<64>;

    using topic_t = int32_t;
    using order_id_t = int64_t;
    using offer_id_t = byte_array<64>;
    //order state
	enum  order_status_t:uint8_t
	{
		status_unknow = 0,                  //未知
		status_insert_submit = 1,           //报送
		status_cancel_submit = 2,           //撤单
		status_insert_reject = 3,           //订单拒绝
		status_cancel_reject = 4,           //撤单拒绝
		status_accept = 5,                  //订单接受
		status_part_trade = 6,              //部分成交
		status_all_trade = 7,               //全部成交
		status_cancel = 8,                  //订单撤单
		status_cancel_reject_by_broker = 9, //撤单被柜台拒绝
		status_count,
	};
    

    //开平
    enum  
    {
        f_open = 0,
        f_close = 1,
        f_close_last = 2, //
    };
    using offset_t=uint8_t;
    //类型定义
    enum  
    {
        f_buy = 0,
        f_sell = 1,
        f_ipo = 3,
    };
    using bs_flag_t=uint8_t;

    /*订单类型 32bit
     24～31 类别: 0--基本 1--套利
     16~23  未使用
     8～15  属性：0--NONE 1-FAK 2--FOK
     0～7   类型：0--限价 1-市价 2--市价止损(盈) 3-限价止损(盈)
    */
    //类别
    enum
    {
        OT_BASIC = 0x0000,
        OT_ARBI = 0x1000, //套利
        //属性
        OT_FAK = 0x0010,
        OT_FOK = 0x0020,
        //类型
        OT_LIMIT = 0x0001,       //限价
        OT_MARKET = 0x0002,      //市价
        OT_STOP_MARKET = 0x0003, //市价止损
        OT_STOP_LIMIT = 0x0004,  //限价止损
        OT_BEST = 0x0005,        //对价
    };
    const int OT_NONE = 0x000000;
    typedef uint32_t order_type_t;

    inline uint32_t ot_get_sort(order_type_t type)
    {
        return 0xf000 & type;
    }
    inline uint32_t ot_get_attr(order_type_t type)
    {
        return 0x00f0 & type;
    }
    inline uint32_t ot_get_type(order_type_t type)
    {
        return 0x000f & type;
    }
    const double MAX_PRICE_VAL = 100000000;

    
    enum  
    {
        f_none = 0,
        f_future = 1, //期货
        f_stock = 2,  //股票
        f_option = 3, //商品期权
        f_if_option = 4, //股指期权
        f_index = 5,     //指数
        f_stock_option=6, //股票期权
    };
    using product_type_t=uint8_t;
    //期权类型
    enum {
        option_call='C', //看涨
        option_pull='P'  //看跌
    };
    using callpull_type_t=char;
    typedef int quot_type_t;

    //交易状态
    enum {
        trade_state_open=1,
        trade_state_close=2,
        trade_state_pause=3,
        trade_state_am_auction=4,
        trade_state_pm_auction=5
    };
    using trade_state_t=uint8_t;
}