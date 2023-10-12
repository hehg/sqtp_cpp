#pragma once
#include  "sq_type_define.h"
#include "sq_struct_def.h"
#include <map>
#include "stock_fe_base.h"
using namespace sq;
using namespace std;

//动能指标
namespace sq_plug
{
    struct stock_price_fe_info
	{
		price_t open = 0;
		price_t mid_price = 0;
		double idx_price = 0;
		qty_t volume = 0; //总成交量??
		double weight = 0;

		double ABDPDiffF = 0;
		double ABDPDiffM = 0;
		double ABVDiffF = 0;
		double ABVDiffM = 0;
	};
	class stock_quote_price:public stock_fe_base
	{
	public:
		stock_quote_price()
		{
		}
		~stock_quote_price(){};
		void add_stock(int idx, double w, double s);
		void put(int tid,char*data,int size);
		void calc_result(void *result);
		void reset();

		std::map<int, stock_price_fe_info> m_fes;
	};
}