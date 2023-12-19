#ifndef DATAFEED_STRUCT_H
#define DATAFEED_STRUCT_H
#include <QString>
#include <limits>

constexpr  double null_data=(std::numeric_limits<double>::max)();

struct datafeed_best_quot
{

    double  price=null_data;        //最新价
    int     match_qty=0;            //成交量
    double  turnover=null_data;     //成交额
    int     open_interest=0;

    double  bid_price=null_data;
    int     bid_vol=0;

    double  ask_price=null_data;
    int     ask_vol=0;

    double  open=null_data;
    double  close=null_data;
    double  clear=null_data;

    double  raise_limit=null_data;
    double  fall_limit=null_data;
};

struct datafeed_mbl
{

    double bid[5]={0,0,0,0,0};
    double ask[5]={0,0,0,0,0};
    int    bid_vol[5]={0,0,0,0,0};
    int    ask_vol[5]={0,0,0,0,0};
};

struct datafeed_opt_param
{

    double delta=0;
    double gamma=0;
    double rho=0;
    double theta=0;
    double vega=0;
};
struct  datafeed_best_order
{

    double bid;
    double ask;
    int bid_vol[10]={0,0,0,0,0,0,0,0,0,0};
    int ask_vol[10]={0,0,0,0,0,0,0,0,0,0};
};
struct datafeed_order_statis
{
    int total_buy_vol=0;
    int total_sell_vol=0;
    double avg_buy_price=null_data;
    double avg_sell_price=null_data;
};

struct datafeed_match_price_qty
{
    double price1=null_data;
    int    buy_open_vol1=0;
    int    sell_open_vol1=0;
    int    buy_close_vol1=0;
    int    sell_close_vol1=0;
    double price2=null_data;
    int    buy_open_vol2=0;
    int    sell_open_vol2=0;
    int    buy_close_vol2=0;
    int    sell_close_vol2=0;
    double price3=null_data;
    int    buy_open_vol3=0;
    int    sell_open_vol3=0;
    int    buy_close_vol3=0;
    int    sell_close_vol3=0;
    double price4=null_data;
    int    buy_open_vol4=0;
    int    sell_open_vol4=0;
    int    buy_close_vol4=0;
    int    sell_close_vol4=0;

    double price5=null_data;
    int    buy_open_vol5=0;
    int    sell_open_vol5=0;
    int    buy_close_vol5=0;
    int    sell_close_vol5=0;
};
struct datafeed_quot_data
{
    QString market;     //市场
    QString contract;   //合约
    char    m_data[512];

};
//合约的行情快照
struct datafeed_quot_snap
{
    int index=-1;
    QString market;     //市场
    QString contract;   //合约
    datafeed_best_quot          m_best_quot;
    datafeed_mbl                m_mbl;
    datafeed_opt_param          m_opt_param;
    datafeed_best_order         m_best_order;
    datafeed_order_statis       m_order_statis;
    datafeed_match_price_qty    m_match_price_qty;
};

#endif // DATAFEED_STRUCT_H
