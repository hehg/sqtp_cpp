#pragma once
#include <string>
#include <unordered_map>
#include  "sq_type_define.h"
#include "sq_struct_def.h"
#include "stock_fe_base.h"
using namespace sq;
/**
 * 大单
 */
namespace sq_plug
{
	struct stock_match_record
	{
		stock_match_record() {}
		stock_match_record(int64_t id, qty_t qty,price_t price) {
			turnover = (price*qty);
			match_qty = qty;
			order_id = id;
		}
		void add(int v, double p) {
			turnover += (v*p);
			match_qty += v;
		}
		int64_t  order_id;
		qty_t    match_qty=0;
		double   turnover=0;
		bool     is_active=false;
	};
	class stock_big_order
	{
	public:
	    /**
		 * @param s 市值
		 */
		stock_big_order(double s)
		{
			
			share_base = s*BigOrder_Base_ShareRate;
		}
		~stock_big_order(){}

		void put(int tid,char*data,int size);
		int  process_match(stock_match_quot*quot);
		
		private:
		/*
		* 天狼的大单判断标准：以下三个条件只要符合一个即是大单。
		* 1、成交的原始委托大于等于500手
		* 2、成交额在50万元以上
		* 3、成交额占流通市值0.1%
		*/
		bool is_big_order(double to, int vol);
		double boDeltaTL(stock_match_record&m, double to, int vol);

		int BigOrder_Base_VOL= 50000;
		double BigOrder_Base_AMT = 500000;
		double BigOrder_Base_ShareRate = 0.001;
		double share_base=0;
		
		std::unordered_map<int, stock_match_record> m_other_buy;
		std::unordered_map<int, stock_match_record> m_other_sell;
	public:
		  double rt_vip_inflow = 0; //1.大户资金净流入
		  double rt_vip_active_inflow = 0; //2.大户主动净流入金额 = 26.大户主买入金额 - 34.大户主卖出金额
		  double rt_vip_activebuy_amt = 0; //26.大户主买入金额
		  double rt_vip_activesell_amt = 0; //34.大户主卖出金额
		  double rt_vip_buy_amt = 0; //1003.当日大户买入成交金额 
		  double rt_vip_sell_amt = 0; //1004.当日大户卖出成交金额
		  double rt_trade_amt = 0; //当日所有成交金额

	};

	class stock_big_orders:public stock_fe_base
	{
		public:
			stock_big_orders() {};
		~stock_big_orders();
		 void add_stock(int idx,double w, double s);
		 void put(int tid,char*data,int size);
		 void calc_result(void*result);
		std::map<int,stock_big_order*> m_fes;
	};
}