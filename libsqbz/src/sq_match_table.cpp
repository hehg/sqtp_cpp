#include "sq_match_table.h"

namespace sq
{
	#if 0
	match_table::~match_table()
	{
		clear();
	}
	void match_table::clear()
	{
		m_map_key.clear();
		auto it = m_match_tbl.begin();
		for (; it != m_match_tbl.end(); ++it) {
			delete *it;
		}
		m_match_tbl.clear();
	}
	int match_table::add(sq_match_info&info)
	{
		auto it=m_map_key.find(info.match_id);
		if (it == m_map_key.end()) {
			sq_match_info*new_info = new sq_match_info();
			memcpy(new_info, &info, sizeof(sq_match_info));
			m_map_key[info.match_id] = new_info;
		}
		else {
			//检查是不是重复
			if (it->second->contract == info.contract&&
				it->second->direction == info.direction&&
				it->second->sq_local_id == info.sq_local_id&&
				it->second->match_price == info.match_price&&
				it->second->match_qty == info.match_qty&&
				it->second->user_id == info.user_id&&
				it->second->offset == info.offset
				) {
				return bz_ok;
			}
			//成交号一样,同一个客户，方向相反
			else if(it->second->user_id == info.user_id&&
				it->second->direction != info.direction)
			{
				string s1 = it->second->to_string();
				string s2 = info.to_string();
				log_error( "trade_by_self:{}<>{}\n", s1.c_str(), s2.c_str());
				m_trade_self_total += info.match_qty;
				if (info.direction == f_buy) {
					m_trade_self.buy_id = info.sq_local_id;
					m_trade_self.sell_id= it->second->sq_local_id;
				}
				else {
					m_trade_self.sell_id = info.sq_local_id;
					m_trade_self.buy_id = it->second->sq_local_id;
				}
				m_trade_self.match_total_qty = m_trade_self_total;
				m_trade_self.match_qty= info.match_qty;

				return bz_err_trade_by_self;
			}
			//检查是不是自成交
		}
		return bz_ok;
	}
#endif 


	
}