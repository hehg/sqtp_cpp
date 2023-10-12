#include "stock_moment.h"
#include "sq_fe_struct.h"
#include "math/math_util.h"
using namespace sq;
namespace sq_plug
{
    void stock_moments::add_stock(int stock,double w, double s)
	{
		m_fes[stock] = stock_moment();
	}
	void stock_moments::put(int tid,char*data,int size)
	{
        assert(tid==tid_market_data);
        sq_quot*quot=(sq_quot*)data;
		int  idx=atoi(quot->contract.data());
        assert(idx>0);
		auto it = m_fes.find(idx);
		if (it == m_fes.end())
		{
			return;
		}
		it->second.put(tid,data,size);
	}

	void stock_moments::calc_result(void*result)
	{
		stock_fe_result*ret=(stock_fe_result*)result;
		double mt_index = 0;
		auto it1 = m_fes.begin();
		for (; it1 != m_fes.end(); ++it1)
		{
			//动量
			mt_index += it1->second.rt_momentum_amt;
		}
		ret->momentumAmt=mt_index/1000.0;
	}
	void stock_moment::copy_mbl(sq_quot *quot)
	{
		cur_mbl.BuyPrice01 = (quot->mbl_buy_price[0]);
		cur_mbl.BuyPrice02 = (quot->mbl_buy_price[1]);
		cur_mbl.BuyPrice03 = (quot->mbl_buy_price[2]);
		cur_mbl.BuyPrice04 = (quot->mbl_buy_price[3]);
		cur_mbl.BuyPrice05 = (quot->mbl_buy_price[4]);

		cur_mbl.BuyVolume01 = quot->mbl_buy_qty[0];
		cur_mbl.BuyVolume02 = quot->mbl_buy_qty[1];
		cur_mbl.BuyVolume03 = quot->mbl_buy_qty[2];
		cur_mbl.BuyVolume04 = quot->mbl_buy_qty[3];
		cur_mbl.BuyVolume05 = quot->mbl_buy_qty[4];

		cur_mbl.SellPrice01 = (quot->mbl_sell_price[0]);
		cur_mbl.SellPrice02 = (quot->mbl_sell_price[1]);
		cur_mbl.SellPrice03 = (quot->mbl_sell_price[2]);
		cur_mbl.SellPrice04 = (quot->mbl_sell_price[3]);
		cur_mbl.SellPrice05 = (quot->mbl_sell_price[4]);

		cur_mbl.SellVolume01 = quot->mbl_sell_qty[0];
		cur_mbl.SellVolume02 = quot->mbl_sell_qty[1];
		cur_mbl.SellVolume03 = quot->mbl_sell_qty[2];
		cur_mbl.SellVolume04 = quot->mbl_sell_qty[3];
		cur_mbl.SellVolume05 = quot->mbl_sell_qty[4];
	}
    void stock_moment::put(int tid,char*data,int size)
	{
		if(tid!=tid_market_data){
			return;
		}
        sq_quot* quot=(sq_quot*)data;
        
		qty_t momentumB_vol = 0;
		qty_t momentumA_vol = 0;
		qty_t momentum_vol = 0;

		copy_mbl(quot);

		if (is_first)
		{
			pre_mbl = cur_mbl;
			is_first = false;
			return;
		}

		int cmp_ret = math_util::double_compare(pre_mbl.BuyPrice01, cur_mbl.BuyPrice01);
		if (cmp_ret == 0)
		{
			momentumB_vol = cur_mbl.BuyVolume01 - pre_mbl.BuyVolume01;
		}
		else if (cmp_ret == 1)
		{
			if (math_util::double_compare(pre_mbl.BuyPrice02, cur_mbl.BuyPrice01) <= 0)
			{
				momentumB_vol = cur_mbl.BuyVolume01 - (pre_mbl.BuyVolume02 + pre_mbl.BuyVolume01);
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice03, cur_mbl.BuyPrice01) <= 0)
			{
				momentumB_vol = cur_mbl.BuyVolume01 - (pre_mbl.BuyVolume03 + pre_mbl.BuyVolume02 + pre_mbl.BuyVolume01);
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice04, cur_mbl.BuyPrice01) <= 0)
			{
				momentumB_vol = cur_mbl.BuyVolume01 - (pre_mbl.BuyVolume04 + pre_mbl.BuyVolume03 + pre_mbl.BuyVolume02 + pre_mbl.BuyVolume01);
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice05, cur_mbl.BuyPrice01) <= 0)
			{
				momentumB_vol = cur_mbl.BuyVolume01 - (pre_mbl.BuyVolume05 + pre_mbl.BuyVolume04 + pre_mbl.BuyVolume03 + pre_mbl.BuyVolume02 + pre_mbl.BuyVolume01);
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice05, cur_mbl.BuyPrice01) > 0)
			{
				momentumB_vol = 0 - (pre_mbl.BuyVolume05 + pre_mbl.BuyVolume04 + pre_mbl.BuyVolume03 + pre_mbl.BuyVolume02 + pre_mbl.BuyVolume01);
			}
			else
			{
				// std::logic_error
				assert(false);
			}
		}
		else if (cmp_ret == -1)
		{ //
			if (math_util::double_compare(pre_mbl.BuyPrice01, cur_mbl.BuyPrice02) >= 0)
			{
				momentumB_vol = (cur_mbl.BuyVolume02 + cur_mbl.BuyVolume01) - pre_mbl.BuyVolume01;
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice01, cur_mbl.BuyPrice03) >= 0)
			{
				momentumB_vol = (cur_mbl.BuyVolume03 + cur_mbl.BuyVolume02 + cur_mbl.BuyVolume01) - pre_mbl.BuyVolume01;
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice01, cur_mbl.BuyPrice04) >= 0)
			{
				momentumB_vol = (cur_mbl.BuyVolume04 + cur_mbl.BuyVolume03 + cur_mbl.BuyVolume02 + cur_mbl.BuyVolume01) - pre_mbl.BuyVolume01;
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice01, cur_mbl.BuyPrice05) >= 0)
			{
				momentumB_vol = (cur_mbl.BuyVolume05 + cur_mbl.BuyVolume04 + cur_mbl.BuyVolume03 + cur_mbl.BuyVolume02 + cur_mbl.BuyVolume01) - pre_mbl.BuyVolume01;
			}
			else if (math_util::double_compare(pre_mbl.BuyPrice01, cur_mbl.BuyPrice05) < 0)
			{
				momentumB_vol = cur_mbl.BuyVolume05 + cur_mbl.BuyVolume04 + cur_mbl.BuyVolume03 + cur_mbl.BuyVolume02 + cur_mbl.BuyVolume01;
			}
			else
			{
				assert(false);
			}
		}

		cmp_ret = math_util::double_compare(pre_mbl.SellPrice01, cur_mbl.SellPrice01);
		if (cmp_ret == 0)
		{
			momentumA_vol = pre_mbl.SellVolume01 - cur_mbl.SellVolume01;
		}
		else if (cmp_ret == 1) //
		{
			if (math_util::double_compare(pre_mbl.SellPrice01, cur_mbl.SellPrice02) <= 0)
			{
				momentumA_vol = pre_mbl.SellVolume01 - (cur_mbl.SellVolume02 + cur_mbl.SellVolume01);
			}
			else if (math_util::double_compare(pre_mbl.SellPrice01, cur_mbl.SellPrice03) <= 0)
			{
				momentumA_vol = pre_mbl.SellVolume01 - (cur_mbl.SellVolume03 + cur_mbl.SellVolume02 + cur_mbl.SellVolume01);
			}
			else if (math_util::double_compare(pre_mbl.SellPrice01, cur_mbl.SellPrice04) <= 0)
			{
				momentumA_vol = pre_mbl.SellVolume01 - (cur_mbl.SellVolume04 + cur_mbl.SellVolume03 + cur_mbl.SellVolume02 + cur_mbl.SellVolume01);
			}
			else if (math_util::double_compare(pre_mbl.SellPrice01, cur_mbl.SellPrice05) <= 0)
			{
				momentumA_vol = pre_mbl.SellVolume01 - (cur_mbl.SellVolume05 + cur_mbl.SellVolume04 + cur_mbl.SellVolume03 + cur_mbl.SellVolume02 + cur_mbl.SellVolume01);
			}
			else if (math_util::double_compare(pre_mbl.SellPrice01, cur_mbl.SellPrice05) > 0)
			{
				momentumA_vol = 0 - (cur_mbl.SellVolume05 + cur_mbl.SellVolume04 + cur_mbl.SellVolume03 + cur_mbl.SellVolume02 + cur_mbl.SellVolume01);
			}
			else
			{
				assert(false);
			}
		}
		else if (cmp_ret == -1)
		{
			if (math_util::double_compare(pre_mbl.SellPrice02, cur_mbl.SellPrice01) >= 0)
			{
				momentumA_vol = (pre_mbl.SellVolume02 + pre_mbl.SellVolume01) - cur_mbl.SellVolume01;
			}
			else if (math_util::double_compare(pre_mbl.SellPrice03, cur_mbl.SellPrice01) >= 0)
			{
				momentumA_vol = (pre_mbl.SellVolume03 + pre_mbl.SellVolume02 + pre_mbl.SellVolume01) - cur_mbl.SellVolume01;
			}
			else if (math_util::double_compare(pre_mbl.SellPrice04, cur_mbl.SellPrice01) >= 0)
			{
				momentumA_vol = (pre_mbl.SellVolume04 + pre_mbl.SellVolume03 + pre_mbl.SellVolume02 + pre_mbl.SellVolume01) - cur_mbl.SellVolume01;
			}
			else if (math_util::double_compare(pre_mbl.SellPrice05, cur_mbl.SellPrice01) >= 0)
			{
				momentumA_vol = (pre_mbl.SellVolume05 + pre_mbl.SellVolume04 + pre_mbl.SellVolume03 + pre_mbl.SellVolume02 + pre_mbl.SellVolume01) - cur_mbl.SellVolume01;
			}
			else if (math_util::double_compare(pre_mbl.SellPrice05, cur_mbl.SellPrice01) < 0)
			{
				momentumA_vol = (pre_mbl.SellVolume05 + pre_mbl.SellVolume04 + pre_mbl.SellVolume03 + pre_mbl.SellVolume02 + pre_mbl.SellVolume01);
			}
			else
			{
				assert(false);
			}
		}

		momentum_vol = momentumA_vol + momentumB_vol;

		if ((0 == pre_mbl.BuyPrice01) || (0 == pre_mbl.BuyPrice02) || (0 == pre_mbl.BuyPrice03) || (0 == pre_mbl.BuyPrice04) || (0 == pre_mbl.BuyPrice05) || (0 == pre_mbl.SellPrice01) || (0 == pre_mbl.SellPrice02) || (0 == pre_mbl.SellPrice03) || (0 == pre_mbl.SellPrice04) || (0 == pre_mbl.SellPrice05))
		{
			momentum_vol = 0;

			momentumA_vol = 0;
			momentumB_vol = 0;
		}
		rt_momentum_vol += momentum_vol;

		double midPrice;
		if ((0 == cur_mbl.BuyPrice01) || (0 == cur_mbl.SellPrice01))
		{
			midPrice = quot->price;
		}
		else
		{
			midPrice = (cur_mbl.BuyPrice01 + cur_mbl.SellPrice01) / 2;
		}

		rt_momentum_amt += momentum_vol * midPrice;

		rt_momentumA_amt += momentumA_vol * midPrice;
		rt_momentumB_amt += momentumB_vol * midPrice;

		pre_mbl = cur_mbl;
	}

	void stock_moment::reset()
	{
		rt_momentum_vol = 0;
		rt_momentum_amt = 0;

		rt_momentumA_amt = 0;
		rt_momentumB_amt = 0;
	}
}