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
    struct stock_fe_mbl
	{
		price_t BuyPrice01;
		price_t BuyPrice02;
		price_t BuyPrice03;
		price_t BuyPrice04;
		price_t BuyPrice05;

		qty_t BuyVolume01;
		qty_t BuyVolume02;
		qty_t BuyVolume03;
		qty_t BuyVolume04;
		qty_t BuyVolume05;

		price_t SellPrice01;
		price_t SellPrice02;
		price_t SellPrice03;
		price_t SellPrice04;
		price_t SellPrice05;

		qty_t SellVolume01;
		qty_t SellVolume02;
		qty_t SellVolume03;
		qty_t SellVolume04;
		qty_t SellVolume05;
	};

	class stock_moment
	{
	public:
		stock_moment()
		{
			memset(&pre_mbl, 0, sizeof(stock_fe_mbl));
			memset(&cur_mbl, 0, sizeof(stock_fe_mbl));
		}
		~stock_moment(){};

		void put(int tid,char*data,int size);
		void reset();

	private:
		void copy_mbl(sq_quot *table);
		bool is_first = true;
		stock_fe_mbl pre_mbl;
		stock_fe_mbl cur_mbl;

	public:
		double rt_momentum_amt = 0;
		double rt_momentumA_amt = 0;
		double rt_momentumB_amt = 0;
		qty_t rt_momentum_vol = 0;
		
	};

	class stock_moments:public stock_fe_base
	{
	public:
		stock_moments()
		{
		}
		void add_stock(int idx,double w, double s);
		void put(int tid,char*data,int size);
		void calc_result(void *result);
		std::map<int, stock_moment> m_fes;
	};
}