#include "match/sq_match_core.h"
#include "error/sq_err.h"

namespace sq
{
	/**
	 * 将订单放入 价位表 及委托队列
	*/
	int mc_contract::insert_order(mc_order_ptr_t& order)
	{
		//加入总队列
		//m_order_list[order->order_id]=order;

		if (order->direction == f_buy) {
			//加入价位表
			auto p_it = m_buy_price_level.find(order->price);
			if (p_it == m_buy_price_level.end()) {
				//新价格
				auto ret=m_buy_price_level.insert(std::make_pair(order->price,mc_price_level()));
				if(ret.second){
					p_it = ret.first;
				}
				else{
					//订单拒绝
					m_core->on_order_book_changed(m_contract.c_str(),order->order_id,order_status_t::status_insert_reject);
					return err_fail;
				}
			}
			//订单接受
		    m_core->on_order_book_changed(m_contract.c_str(),order->order_id,order_status_t::status_accept);

			//加入价位表
			p_it->second.m_total_qty += order->qty;
			p_it->second.m_order_list[order->order_id]=(order);

			m_core->on_price_level_changed(m_contract.c_str(),f_buy,order->price,p_it->second.m_total_qty);
			
		}
		else {
			//加入价位表
			auto p_it = m_sell_price_level.find(order->price);
			if (p_it == m_sell_price_level.end()) {
				//新价格
				auto ret=m_sell_price_level.insert(std::make_pair(order->price,mc_price_level()));
				if(ret.second){
					p_it = ret.first;
				}
				else{
					//订单拒绝
					m_core->on_order_book_changed(m_contract.c_str(),order->order_id,order_status_t::status_insert_reject);
					return err_fail;
				}

			}
			//订单接受
		    m_core->on_order_book_changed(m_contract.c_str(),order->order_id,order_status_t::status_accept);

			//加入价位表
			p_it->second.m_total_qty += order->qty;
			p_it->second.m_order_list[order->order_id]=(order);

			m_core->on_price_level_changed(m_contract.c_str(),f_sell,order->price,p_it->second.m_total_qty);
			
		}
		return ok;
	}
	
	int mc_contract::cancel_order(order_id_t order_id,bs_flag_t direction,int price)
	{
		int op_result = ok;
		do
		{
			if (direction == f_buy)
			{
				auto p_it = m_buy_price_level.find(price);
				if (p_it == m_buy_price_level.end())
				{
					op_result = no_exist_order;
					break;
				}
				auto o_it = p_it->second.m_order_list.find(order_id);
				if (o_it == p_it->second.m_order_list.end())
				{
					op_result = no_exist_price;
					break;
				}
				mc_order_ptr_t order = o_it->second;
				if (order->qty > 0)
				{
					order->is_fin = true;
					m_core->on_order_book_changed(m_contract.c_str(), order->order_id, order_status_t::status_cancel);
					p_it->second.m_order_list.erase(o_it);

					// 挂单量空了，删除这个价位
					p_it->second.m_total_qty -= order->qty; // 更新价位表挂单量
					if (p_it->second.m_total_qty == 0)
					{
						assert(p_it->second.m_order_list.empty() == true);
						m_buy_price_level.erase(p_it);
					}
					m_core->on_price_level_changed(m_contract.c_str(), f_buy, p_it->first, p_it->second.m_total_qty);
				}
				else
				{
					op_result = cancel_order_err;
					break;
				}
			}

			else
			{
				auto p_it = m_sell_price_level.find(price);
				if (p_it == m_sell_price_level.end())
				{
					op_result = no_exist_order;
					break;
				}
				auto o_it = p_it->second.m_order_list.find(order_id);
				if (o_it == p_it->second.m_order_list.end())
				{
					op_result = no_exist_order;
					break;
				}
				mc_order_ptr_t order = o_it->second;
				if (order->qty > 0)
				{
					order->is_fin = true;
					m_core->on_order_book_changed(m_contract.c_str(), order->order_id, order_status_t::status_cancel);
					p_it->second.m_order_list.erase(o_it);

					p_it->second.m_total_qty -= order->qty; // 更新价位表挂单量

					// 挂单量空了，删除这个价位
					if (p_it->second.m_total_qty == 0)
					{
						assert(p_it->second.m_order_list.empty() == true);

						m_sell_price_level.erase(p_it);
					}
					m_core->on_price_level_changed(m_contract.c_str(), f_sell, p_it->first, p_it->second.m_total_qty);

				}
				else
				{
					op_result = cancel_order_err;
					break;
				}
			}
		} while (false);

		if(op_result != ok)
		{
			m_core->on_order_book_changed(m_contract.c_str(), order_id, order_status_t::status_cancel_reject);
		}
		return ok;
	}
	
    void mc_contract::remove_order(order_id_t order_id,bs_flag_t direction,int price)
	{
			if (direction == f_buy)
			{
				auto p_it = m_buy_price_level.find(price);
				if (p_it == m_buy_price_level.end())
				{
					return;
				}
				auto o_it = p_it->second.m_order_list.find(order_id);
				if (o_it == p_it->second.m_order_list.end())
				{
					p_it->second.m_order_list.erase(o_it);
				}
			}
			else 
			{
				auto p_it = m_sell_price_level.find(price);
				if (p_it == m_sell_price_level.end())
				{
					return;
				}
				auto o_it = p_it->second.m_order_list.find(order_id);
				if (o_it == p_it->second.m_order_list.end())
				{
					p_it->second.m_order_list.erase(o_it);
				}
			}
	}
	
	int mc_contract::match_once(mc_order_ptr_t& order)
	{
		
		//撮合买方向订单
		if (order->direction == f_buy) 
		{
			auto my_price_it=m_buy_price_level.begin();
			//尝试吃掉这个订单
			while (order->qty > 0) 
			{
				//找第一个卖价位
				auto other_side_it = m_sell_price_level.begin();

				//无对手价,价位表空了
				if (other_side_it == m_sell_price_level.end()) {
					break;
				}
				//有成交机会
				if (order->price >= other_side_it->first) {
					//遍历这个价位的订单
					assert(other_side_it->second.m_total_qty > 0);
					while (other_side_it!=m_sell_price_level.end()) 
					{
						auto o_it = other_side_it->second.m_order_list.begin();
						assert(!other_side_it->second.m_order_list.empty());

						mc_order*side_order = o_it->second.get();
						assert(side_order->qty > 0);
						//成交数量计算
						int match_qty = side_order->qty >= order->qty ? order->qty : side_order->qty;
						
						//成交价格计算
						price_t match_price = side_order->price;

						//记录成交结果
						int64_t match_id = m_core->get_match_id();
						m_core->on_order_matched(m_contract.c_str(), match_id,
												 order->order_id,
												 order->direction, match_price, match_qty);
						m_core->on_order_matched(m_contract.c_str(), match_id,
												 side_order->order_id,
												 side_order->direction, match_price, match_qty);

						//更新订单信息
						order->qty -= match_qty;
						m_core->on_order_book_changed(m_contract.c_str(), order->order_id, 
						order->qty==0?order_status_t::status_all_trade:order_status_t::status_part_trade);

						side_order->qty -= match_qty;
						m_core->on_order_book_changed(m_contract.c_str(), side_order->order_id, 
						side_order->qty==0?order_status_t::status_all_trade:order_status_t::status_part_trade);

						//价位上的挂单总量减少
						other_side_it->second.m_total_qty -= match_qty;
						my_price_it->second.m_total_qty -= match_qty;

						m_core->on_price_level_changed(m_contract.c_str(),f_sell, other_side_it->first, other_side_it->second.m_total_qty);
						m_core->on_price_level_changed(m_contract.c_str(),f_buy, my_price_it->first, my_price_it->second.m_total_qty);
						//被吃光了，取下个订单继续撮合
						
						if (side_order->qty == 0) {
							other_side_it->second.m_order_list.erase(o_it);
							if(other_side_it->second.m_order_list.empty())
							{
								m_sell_price_level.erase(other_side_it);
								other_side_it=m_sell_price_level.end();
							}
						}
						//本订单吃完了
						if (order->qty == 0) {
							break;
						}
					}

					//这个价位被吃光了，删除这个价位
					//if (other_side_it->second.m_total_qty == 0) {
					//	m_sell_price_level.erase(other_side_it);
					//}
					if(my_price_it->second.m_total_qty == 0){
						m_buy_price_level.erase(my_price_it);
					}

					//本订单吃完了
					if (order->qty == 0) {
						remove_order(order->order_id,order->direction,order->price);
						break;
					}
				}
				else {
					break;
				}

			}
			
		}

		//撮合卖方向订单
		else {
			auto my_price_it=m_sell_price_level.begin();
			//尝试吃掉这个订单
			while (order->qty > 0) {
				//找第一个对手价位
				auto other_side_it = m_buy_price_level.begin();

				//无对手价
				if (other_side_it == m_buy_price_level.end()) {
					break;
				}
				//有成交机会
				if (order->price <= other_side_it->first) {
					//遍历这个价位的订单
					assert(other_side_it->second.m_total_qty > 0);
					//遍历这个价位的订单
					while (other_side_it!=m_buy_price_level.end())
					{
						assert(!other_side_it->second.m_order_list.empty());
						auto o_it = other_side_it->second.m_order_list.begin();
						
						mc_order*side_order = o_it->second.get();
						assert(side_order->qty > 0);
						//成交数量计算
						int match_qty = side_order->qty >= order->qty ? order->qty : side_order->qty;

						//成交价格计算
						int match_price = side_order->price;

						//记录成交结果
						int64_t match_id = m_core->get_match_id();
						m_core->on_order_matched(m_contract.c_str(), match_id,
												 order->order_id,
												 order->direction, match_price, match_qty);
						m_core->on_order_matched(m_contract.c_str(), match_id,
												 side_order->order_id,
												 side_order->direction, match_price, match_qty);


						//更新订单信息
						order->qty -= match_qty;
						m_core->on_order_book_changed(m_contract.c_str(), order->order_id, 
						order->qty==0?order_status_t::status_all_trade:order_status_t::status_part_trade);

						side_order->qty -= match_qty;
						m_core->on_order_book_changed(m_contract.c_str(), side_order->order_id, 
						side_order->qty==0?order_status_t::status_all_trade:order_status_t::status_part_trade);

						//价位上的挂单总量减少
						other_side_it->second.m_total_qty -= match_qty;
						my_price_it->second.m_total_qty -= match_qty;

						m_core->on_price_level_changed(m_contract.c_str(),f_sell, other_side_it->first, other_side_it->second.m_total_qty);
						m_core->on_price_level_changed(m_contract.c_str(),f_buy, my_price_it->first, my_price_it->second.m_total_qty);

						//被吃光了，取下个订单继续撮合
						if (side_order->qty == 0) {
							other_side_it->second.m_order_list.erase(o_it);
							//这价位空了
							if(other_side_it->second.m_order_list.empty())
							{
								m_buy_price_level.erase(other_side_it);
								other_side_it=m_buy_price_level.end();
							}
						}


						//本订单吃完了
						if (order->qty == 0) {
							break;
						}
					}

					//这个价位被吃光了，删除这个价位
					//if (other_side_it->second.m_total_qty == 0) {
					//	m_buy_price_level.erase(other_side_it);
					//}
					if(my_price_it->second.m_total_qty == 0){
						m_sell_price_level.erase(my_price_it);
					}
					//本订单吃完了
					if (order->qty == 0) {
						remove_order(order->order_id,order->direction,order->price);
						break;
					}
				}
				else {
					break;
				}
			}

		}

		return ok;
	}
	void mc_contract::dump(std::ostream&os)
	{
      //价位表
	  //buy
	  os<<std::fixed;
	  for (auto &p : m_buy_price_level) {
		  os << "buy:" <<std::setw(10)<< p.first << "," << p.second.m_total_qty << std::endl;
		  
		//  os<<"  "<<"id"<<" "<<"qty"<<std::endl;
		  
		  for(auto &o : p.second.m_order_list)
		  {
			os<<"  "<<o.second->order_id<<"   "<<o.second->qty<<std::endl;
		  }
	  }

	  //sell
	  for (auto &p : m_sell_price_level) {
		  os << "sell:" <<std::setw(10)<< p.first << "," << p.second.m_total_qty << std::endl;
		// os<<"  "<<"id"<<" "<<"qty"<<std::endl;
		  for(auto &o : p.second.m_order_list)
		  {
			os<<"  "<<o.second->order_id<<"   "<<o.second->qty<<std::endl;
		  }
	  }
	  
	}
	//===================sq_match_core========================
	sq_match_core::sq_match_core()
	{
	}
	void sq_match_core::begin_match()
	{

	}
	void sq_match_core::end_match()
	{
		//clear result
	}
	sq_match_core::~sq_match_core()
	{
		for (auto &s : m_contracts) {
			delete s.second;
			s.second = nullptr;
		}
		m_contracts.clear();

		for(auto&s:m_match_results)
		{
			delete s;
		}
		m_match_results.clear();
	}
	
	void sq_match_core::add_contract(sq_contract *contract)
	{
		std::string contract_id = contract->contract.to_string();
		if (m_contracts.find(contract_id) == m_contracts.end())
		{
			mc_contract *support = new mc_contract(this);
			support->m_contract = contract_id;
			
			m_contracts.insert(std::make_pair(contract_id, support));
		}
	}
	
	
	int sq_match_core::insert_order(const char*constract_id,order_id_t order_id,
	bs_flag_t direction,int price,qty_t qty)
	{
		auto c_it = m_contracts.find(constract_id);
		if (c_it == m_contracts.end())
			return no_exist_contract;
		//构造一个订单
		mc_order_ptr_t order=std::make_shared<mc_order>();
		order->order_id = order_id;
		order->direction = direction;
		order->price = price;
		order->qty = qty;
		order->type=0;
		int ret= c_it->second->insert_order(order);
		if(ret==ok)
		{
			if(order->direction==f_buy)
			{
				auto it=c_it->second->m_buy_price_level.begin();
				//在最优价上，下单触发一次成交
				if(it->first==order->price){
					c_it->second->match_once(order);
				}
			}
			else{
				auto it=c_it->second->m_sell_price_level.begin();
				//在最优价上，下单触发一次成交
				if(it->first==order->price){
					c_it->second->match_once(order);
				}
			}
			
		}

		return ret;

	}
	

	int sq_match_core::cancel_order(const char*contract,order_id_t order_id,bs_flag_t direction,int price)
	{
		auto c_it = m_contracts.find(contract);
		if (c_it == m_contracts.end())
			return no_exist_contract;

		return c_it->second->cancel_order(order_id,direction,price);
	}

	void sq_match_core::on_price_level_changed(const char *contract, bs_flag_t direction, int price, int qty)
	{
		mc_price_level_changed *result = new mc_price_level_changed;
		result->mc_type = mc_price_level_changed::type;
		result->contract = contract;
		result->price = price;
		result->direction = direction;
		result->qty = qty;
		m_match_results.push_back(result);
	}
	void sq_match_core::on_order_book_changed(const char *contract, order_id_t order_id, order_status_t status)
	{
		mc_order_book_changed *result = new mc_order_book_changed;
		result->mc_type = mc_order_book_changed::type;
		result->contract = contract;
		result->order_id = order_id;
		result->status = status;
		m_match_results.push_back(result);
	}
	void sq_match_core::on_order_matched(const char*contract,int64_t match_id,order_id_t 
	order_id,bs_flag_t direction,price_t match_price,qty_t match_qty)
	{
		mc_order_matched *result = new mc_order_matched;
		result->mc_type = mc_order_matched::type;
		result->contract = contract;
		result->match_id = match_id;
		result->order_id = order_id;
		result->direction = direction;
		result->match_price = match_price;
		result->match_qty = match_qty;
		m_match_results.push_back(result);
	}
	void sq_match_core::dump(std::ostream &os)
	{
		os << "==sq_match_core==" << std::endl;
		for (auto it = m_contracts.begin(); it != m_contracts.end(); ++it)
		{
			it->second->dump(os);
		}
	}

#if 0
	int  sq_match_core::match_order(int contract,int direction)
	{
		m_match_results.clear();

		auto it_contract = m_contracts.find(contract);
		if (it_contract == m_contracts.end())
			return no_exist_contract;

		bool bid_price_null = true;
		bool ask_price_null = true;
		bool bid_order_null = true;
		bool ask_order_null = true;

		auto bid_price_it = it_contract->second->m_buy_price_level.end();
		auto ask_price_it = it_contract->second->m_sell_price_level.end();
	//	mc_order_list_t::iterator bid_order_it = nullptr;

		while (true)
		{
			if (bid_price_null) {
				bid_price_it = it_contract->second->m_buy_price_level.begin();
				//没有任何价位了
				if (bid_price_it == it_contract->second->m_buy_price_level.end())
					break;

				bid_price_null = false;
			}
			
			if (ask_price_null) {
				ask_price_it = it_contract->second->m_sell_price_level.begin();
				//没有任何价位了
				if (ask_price_it == it_contract->second->m_sell_price_level.end())
					break;

				ask_price_null = false;
			}
			if (bid_price_it->first >= ask_price_it->first) {
				//取最优bid订单
				mc_order_list_t::iterator bid_order_it = bid_price_it->second.m_orders.begin();
				if (bid_order_it == bid_price_it->second.m_orders.end()) {
					//这个价位没有订单了，删除这个价位
					it_contract->second->m_buy_price_level.erase(bid_price_it);
					bid_price_null = true;
					continue;
				}

				//取最优ask订单
				auto ask_order_it = ask_price_it->second.m_orders.begin();
				if (ask_order_it == ask_price_it->second.m_orders.end()) {
					//这个价位没有订单了，删除这个价位
					it_contract->second->m_sell_price_level.erase(ask_price_it);
					ask_price_null = true;
					continue;
				}

				//确认成交价

				//确认成交量
				int match_qty = bid_order_it->second.m_ref->qty;
				if (bid_order_it->second.m_ref->qty > 
					ask_order_it->second.m_ref->qty) {
					match_qty= ask_order_it->second.m_ref->qty;
				}
				

			}
			else {
				break;
			}

			
		}
	}
#endif

}