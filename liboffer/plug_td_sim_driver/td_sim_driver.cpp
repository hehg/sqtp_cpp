#include <set>
#include "td_sim_driver.h"
#include "error/sq_err.h"
#include "math/math_util.h"
#include "time/date_time.h"
#include "log/sq_logger.h"

#include "sq_type_define.h"
using namespace sq;
namespace sq_plug
{

	// 计算当前行情与上一笔行情之间的成交信息
	void driver_match_info::driver_new_match(sq_quot *quot)
	{
		m_atp = 0;
		m_mpb = 0;
		m_mpa = 0;
		m_mvb = 0;
		m_mva = 0;
		m_cur_mid_price = 0.5 * (quot->bid_price() + quot->ask_price());

		if (m_pre_turnover != 0)
		{

#if 0
			double increase_turnover = quot->turnover /* - m_pre_turnover*/;
			double increase_match_qty = quot->match_qty /*- m_pre_match_qty*/;
#else
			double increase_turnover = (quot->turnover - m_pre_turnover);
			double increase_match_qty = quot->match_qty - m_pre_match_qty;
#endif
			// 没有成交
			if (increase_match_qty != 0)
			{

				m_atp = increase_turnover / (increase_match_qty * m_unit);

				// 在bid ask 之间
				if (math_util::double_compare(m_atp, m_pre_bid) >= 0 &&
					math_util::double_compare(m_atp, m_pre_ask) <= 0)
				{
					double i_atp = (int(m_atp / m_tick) * m_tick);
					if (math_util::double_compare(i_atp, m_atp) != 0)
					{
						m_mpb = i_atp;
						m_mpa = i_atp + m_tick;
						m_mva = ((increase_match_qty * m_mpb * m_unit) - increase_turnover) / ((m_mpb - m_mpa) * m_unit);
						m_mvb = increase_match_qty - m_mva;
						if (m_mva < 0)
							m_mva = 0;
						if (m_mva < 0)
							m_mpb = 0;
						// assert(m_mva >= 0);
						// assert(m_mvb >= 0);

						if (m_mva > increase_match_qty)
						{
							m_mva = increase_match_qty;
						}
						if (m_mvb > increase_match_qty)
						{
							m_mvb = increase_match_qty;
						}
					}
					else
					{
						if (m_atp <= m_pre_mid_price)
						{ // 跌
							m_mpb = i_atp;
							m_mpa = i_atp + m_tick;

							m_mvb = increase_match_qty;
							m_mva = 0;
						}
						else
						{ // 涨
							m_mpb = i_atp - m_tick;
							m_mpa = i_atp;

							m_mvb = 0;
							m_mva = increase_match_qty;
						}
					}
				}
				else if (m_atp < m_pre_bid)
				{ // 价格下跌了
					m_mpb = m_pre_bid;
					m_mvb = increase_match_qty;
					m_mpa = m_mpb + m_tick;
					m_mva = 0;
				}
				else if (m_atp > m_pre_ask)
				{ // 价格上涨了
					m_mpb = m_pre_ask - m_tick;
					m_mvb = 0;
					m_mpa = m_pre_ask;
					m_mva = increase_match_qty;
				}
				else
				{
					assert(false);
				}
				m_pre_turnover = quot->turnover;
				m_pre_match_qty = quot->match_qty;
				m_pre_mid_price = (quot->bid_price() + quot->ask_price()) * 0.5;
				m_pre_bid = quot->bid_price();
				m_pre_ask = quot->ask_price();
			}
			else
			{
				m_mpa = quot->ask_price();
				m_mpb = quot->bid_price();
			}
		}
		else
		{
			m_pre_turnover = quot->turnover;
			m_pre_match_qty = quot->match_qty;
			m_pre_mid_price = (quot->bid_price() + quot->ask_price()) * 0.5;
			m_pre_bid = quot->bid_price();
			m_pre_ask = quot->ask_price();

			m_atp = m_pre_mid_price;
			m_mpa = quot->ask_price();
			m_mpb = quot->bid_price();
			m_mva = 0;
			m_mvb = 0;
		}

#if 0
		log_debug("pkg_no={},m_mpa={},m_mpb={},m_mva={},m_mvb={}\n", quot->pkg_no,
			m_mpa, m_mpb, m_mva, m_mvb);
#endif
	}
	void driver_match_info::driver_new_handup_order(sq_quot *cur, sq_quot *next)
	{
		m_AttB = m_mvb / 2;
		m_AttS = m_mva / 2;
	}
	static bool cmp_by_order_id(sq_order_record *l, sq_order_record *r)
	{
		return l->sq_local_id > r->sq_local_id;
	}

	td_sim_driver::td_sim_driver()
	{

	}
	td_sim_driver::~td_sim_driver()
	{
		for (auto a : m_all_quot)
		{
			delete a;
		}
		m_all_quot.clear();
	}
	int td_sim_driver::open()
	{
	
		plug_base::open();

		m_tick = get_cfg_int("tick");
		m_unit = get_cfg_int("unit");
		m_beforeme_fix = get_cfg_int("beforeme_fix");
		m_beforeme_rate = get_cfg_double("beforeme_rate");
		m_cancel_match_rate = get_cfg_double("cancel_match_rate");
		m_all_match_type = get_cfg_int("all_match_type");
		m_enable_best_match = get_cfg_bool("enable_best_match");
		m_support_sim_log = get_cfg_bool("support_sim_log");
		m_quot_file_path = get_cfg_string("quot_file");
		m_quot_file_have_header=get_cfg_bool("quot_file_have_header");
		if(!m_quot_file_have_header){
			m_quot_file_header=get_cfg_string("quot_file_header");
		}
		vector<string> contracts = get_cfg_arrary("support_contract");
		if (!contracts.empty())
		{
			m_quot_table.add(contracts[0]);
			m_contract = contracts[0];
		}
		if (m_support_sim_log)
		{
			char time_info[64] = {0};
			time_t t=time(nullptr);
			strftime(time_info, sizeof(time_info), "%F_%H_%M_%S",localtime(&t));
			
			char name[128] = {0};
			sprintf(name, "./%s_%s.csv", m_sim_log_name.c_str(),time_info);
			sim_log.open(name, std::ios::out | std::ios::trunc);

			sim_log << "time,B1P,B1V,S1P,S1V,volume,turnover,mpa,mva,mpb,mvb,attb,atts,new_order,A.BeforeMe,B.BeforeMe" << endl;
			sim_log.precision(2);
			sim_log << std::fixed;
		}
		string match_price_type = get_cfg_string("match_price_triger_type");
		if (match_price_type == "handup")
		{
			m_match_price_type = match_price_handup;
		}
		else
		{
			m_match_price_type = match_price_otherside;
		}

		m_driver.m_tick = m_tick;
		m_driver.m_unit = m_unit;
		m_driver_next.m_tick = m_tick;
		m_driver_next.m_unit = m_unit;

		load_quot();
		m_run_flag=true;
		m_eg = new thread(&td_sim_driver::run, this);
		log_info("start plug_td_sim_driver\n");
		return ok;
	}
	int td_sim_driver::close()
	{
		m_run_flag=false;
		m_eg->join();
		delete m_eg;
		return ok;
	}
	int td_sim_driver::put(uint16_t tid, char* data,uint16_t size)
	{
		if(tid==tid_place_order){
			sq_req_order* req=(sq_req_order*)data;
			place_order(req);
		}
		else if(tid==tid_cancel_order){
			sq_req_cancel_order*req=(sq_req_cancel_order*)data;
			cancel_order(req);
		}
		else{
			log_fata("not support tid={}\n",tid);
		}
		return ok;
	}
	bool td_sim_driver::load_quot()
	{
		// printf("cmd line:tick=%f,unit=%f,quot_file=%s,N=%f\n", m_tick, m_unit, m_quot_file_path.c_str(), m_beforeme_fix);
		// 读取所有行情
		if (!m_quot_file_path.empty())
		{
			printf("begin load quot\n");
			if(!m_quot_file_have_header){
				assert(!m_quot_file_header.empty());
				quot_reader.set_header(m_quot_file_header.c_str());
			}
			bool ret = quot_reader.open(m_quot_file_path.c_str(), m_quot_file_have_header);

			if (ret == false)
			{
				log_fata("not exit quot file {}\n",m_quot_file_path.c_str());
			}
		}
		else
		{
			return false;
		}

		//read one quot
		while (true)
		{
			sq_quot *quot = new sq_quot;
			quot->tid = tid_market_data;
			bool ret = quot_reader.read_line(*quot);
			if (!ret)
			{
				delete quot;
				break;
			}

			if (quot->contract.to_string() != m_contract)
			{
				delete quot;
				continue;
			}

			quot->pkg_no = m_pkg_no++;
			m_all_quot.push_back(quot);
			break;
		}
		 printf("stop load quot\n");

		return true;

	}
	bool td_sim_driver::read_quot()
	{
		//read one quot
		while (true)
		{
			sq_quot *quot = new sq_quot;
			memset(quot,0,sizeof(sq_quot));
			quot->tid = tid_market_data;
			bool ret = quot_reader.read_line(*quot);
			if (!ret)
			{
				delete quot;
				break;
			}

			if (quot->contract.to_string() != m_contract)
			{
				delete quot;
				continue;
			}

			quot->pkg_no = m_pkg_no++;
			m_all_quot.push_back(quot);
			break;
		}

		if (m_cur_quot_idx == 0)
		{
			m_pre_quot = nullptr;
			m_cur_quot = m_all_quot[m_cur_quot_idx];
			m_next_quot = m_all_quot[m_cur_quot_idx + 1];
			m_cur_quot_idx += 1;
		}
		else if (m_cur_quot_idx < (int)m_all_quot.size() - 1)
		{
			m_pre_quot = m_cur_quot;
			m_cur_quot = m_all_quot[m_cur_quot_idx];
			m_next_quot = m_all_quot[m_cur_quot_idx + 1];
			m_cur_quot_idx += 1;
		}
		else
		{
			return false;
		}
		return true;
	}

	void td_sim_driver::run()
	{
		printf("begin run engine thread\n");
		while (m_run_flag)
		{

			if(false==read_quot())
			{
				printf("read_quot finish\n");
				break;
			}
#if 1
			if (m_cur_quot)
			{
				m_quot_table.update(m_contract,*m_cur_quot);

				// 推导成交,计算当前行情与上一笔行情计算的mpa，mpb
				m_driver.driver_new_match(m_cur_quot);

				// 处理目前的订单队列中没有能成交的订单
				match(m_cur_quot, m_driver);

				// 发送行情
				add_result(m_cur_quot->tid, (char *)m_cur_quot, sizeof(sq_quot));
				// 处理请求队列中的订单，撤单

				// 推导新增atts,利用下一笔行情与当前行情计算attb atts
				m_driver_next.driver_new_match(m_next_quot);

				// 推导新增挂单
				m_new_order = -1;
				m_driver_next.driver_new_handup_order(m_cur_quot, m_next_quot);

				if (m_cancel_match_rate > 0)
				{
					std::set<order_id_t> cancel_orders;
					std::set<order_id_t> have_matched_orders;

					bool have_cancel = false;
					int cmd_count = m_input_rb->get_real_count();
					for (int i = 0; i < cmd_count; i++)
					{
						req_msg *cmd = m_input_rb->at(i);
						// 撤单
						if (cmd->msg_type == tid_cancel_order)
						{
							have_cancel = true;
							sq_req_cancel_order *req = (sq_req_cancel_order *)cmd->msg;
							cancel_orders.insert(req->sq_local_id);
						}
					}
					// 预成交处理一遍，看看是否撤销的订单能够成交
					m_pre_match_result.clear();
					if (have_cancel == true)
					{
						match_sim_cancel(m_next_quot, m_driver_next, m_pre_match_result);
						if (m_pre_match_result.empty() == false)
						{
							// 在一个方向上只允许出现一次撤单有成交
							bool buy_has_match = false;
							bool sell_has_match = false;
							for (int i = 0; i < (int)m_pre_match_result.size(); i++)
							{
								if (m_pre_match_result[i].direction == f_buy)
								{
									order_id_t id = m_pre_match_result[i].order_id;
									if (have_matched_orders.find(id) != have_matched_orders.end())
									{
										continue;
									}

									if (cancel_orders.find(id) != cancel_orders.end())
									{
										assert(buy_has_match == false);
										// 产生一个成交
										int can_match_qty = int(m_cancel_match_rate * m_pre_match_result[i].match_qty);
										if (can_match_qty > 0)
										{
											sq_order_record *order = m_match_order_table.find(id);
											order->match_price = m_pre_match_result[i].match_price;
											order->match_qty = can_match_qty;
											order->match_total += order->match_qty;

											if (order->match_total >= order->qty)
											{
												order->change_status(status_all_trade);
												assert(order->match_total == order->qty);
											}
											else
											{
												order->change_status(status_part_trade);
											}
											have_matched_orders.insert(id);
											log_info("match_before_cancel:sq_local_id={},match_price={},direction={},match_qty={}\n",
													order->sq_local_id, order->match_price, order->direction, order->match_qty);

											add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
											decrease_handup_qty(order->price, order->direction, order->match_qty);
										}
										else
										{
											log_info("match_before_cancel qty is zero:sq_local_id={}\n", id);
										}
									}
									else
									{
										buy_has_match = true; // 其它订单有成交
									}
								}
								else if (m_pre_match_result[i].direction == f_sell)
								{
									order_id_t id = m_pre_match_result[i].order_id;
									if (have_matched_orders.find(id) != have_matched_orders.end())
									{
										continue;
									}
									if (cancel_orders.find(id) != cancel_orders.end())
									{
										assert(sell_has_match == false);
										// 产生一个成交
										int can_match_qty = int(m_cancel_match_rate * m_pre_match_result[i].match_qty);
										if (can_match_qty > 0)
										{
											sq_order_record *order = m_match_order_table.find(id);
											order->match_price = m_pre_match_result[i].match_price;
											order->match_qty = can_match_qty;
											order->match_total += order->match_qty;
											if (order->match_total >= order->qty)
											{
												order->change_status(status_all_trade);
												assert(order->match_total == order->qty);
											}
											else
											{
												order->change_status(status_part_trade);
											}
											have_matched_orders.insert(id);
											log_info("match_before_cancel:sq_local_id={},match_price={},direction={},match_qty={}\n",
													order->sq_local_id, order->match_price, order->direction, order->match_qty);

											add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
											decrease_handup_qty(order->price, order->direction, order->match_qty);
										}
										else
										{
											log_info("match_before_cancel qty is zero:sq_local_id={}", id);
										}
									}
									else
									{
										sell_has_match = true;
									}
								}
							}
						}
					}
				}

				handle_package();

				// 打印日志
				if (m_support_sim_log)
				{

					int ask_before_me = -1;
					order_id_t ask_before_me_order_id = 0;
					auto it = m_sell_handup_info.find(m_cur_quot->ask_price());
					if (it != m_sell_handup_info.end())
					{

						auto order_it = it->second.m_orders.begin();
						for (; order_it != it->second.m_orders.end(); ++order_it)
						{
							if ((*order_it)->order->is_finish == false)
							{
								ask_before_me = (*order_it)->before_me;
								ask_before_me_order_id = (*order_it)->order->sq_local_id;
								break;
							}
						}
					}

					int bid_before_me = -1;
					order_id_t bid_before_me_order_id = 0;
					it = m_buy_handup_info.find(m_cur_quot->bid_price());
					if (it != m_buy_handup_info.end())
					{

						auto order_it = it->second.m_orders.begin();
						for (; order_it != it->second.m_orders.end(); ++order_it)
						{
							if ((*order_it)->order->is_finish == false)
							{
								bid_before_me = (*order_it)->before_me;
								bid_before_me_order_id = (*order_it)->order->sq_local_id;
								break;
							}
						}
					}

					sim_log << m_cur_quot->time.data() << ","
							<< m_cur_quot->bid_price() << ","
							<< m_cur_quot->bid_qty() << ","
							<< m_cur_quot->ask_price() << ","
							<< m_cur_quot->ask_qty() << ","
							<< m_cur_quot->match_qty << ","
							<< m_cur_quot->turnover << ","
							<< m_driver_next.m_mpa << ","
							<< m_driver_next.m_mva << ","
							<< m_driver_next.m_mpb << ","
							<< m_driver_next.m_mvb << ","
							<< m_driver_next.m_AttB << ","
							<< m_driver_next.m_AttS << ","
							<< m_new_order << ","
							<< ask_before_me_order_id << "-" << ask_before_me << ","
							<< bid_before_me_order_id << "-" << bid_before_me
							<< endl;
				}
			}
#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		printf("stop run engine thread\n");
	}
	// 取下一笔行情里的价格、数量
	int td_sim_driver::get_next_quot_info(price_t price, qty_t &next_val)
	{
		if (m_next_quot)
		{
			for (int i = 0; i < 5; i++)
			{
				if (m_next_quot->mbl_buy_price[i] == price)
				{
					next_val = m_next_quot->mbl_buy_qty[i];
					return f_buy;
				}
			}
			for (int i = 0; i < 5; i++)
			{
				if (m_next_quot->mbl_sell_price[i] == price)
				{
					next_val = m_next_quot->mbl_sell_qty[i];
					return f_sell;
				}
			}
		}

		next_val = 0;
		return -1;
	}
	int td_sim_driver::get_cur_quot_info(price_t price, qty_t &next_val)
	{
		if (m_cur_quot)
		{
			for (int i = 0; i < 5; i++)
			{
				if (m_cur_quot->mbl_buy_price[i] == price)
				{
					next_val = m_cur_quot->mbl_buy_qty[i];
					return f_buy;
				}
			}
			for (int i = 0; i < 5; i++)
			{
				if (m_cur_quot->mbl_sell_price[i] == price)
				{
					next_val = m_cur_quot->mbl_sell_qty[i];
					return f_sell;
				}
			}
		}

		next_val = 0;
		return -1;
	}
	void td_sim_driver::send_all_match(sq_order_record *order)
	{

		if (m_all_match_type == 0)
		{
			int remain_qty = order->qty - order->match_total;
			if (order->direction == f_buy) // 先低价成交，再高价成交
			{
				if (remain_qty > 0 && order->price == m_driver_next.m_mpb && m_driver_next.m_mvb > 0)
				{

					order->match_price = m_driver_next.m_mpb;
					int match_vol = (qty_t)(m_driver_next.m_mvb / 2);
					order->match_qty = match_vol > remain_qty ? remain_qty : match_vol;

					remain_qty -= order->match_qty;
					order->match_total += order->match_qty;
					log_info("buy_order all_match:[mpb],sq_local_id={},match_price={},match_qty={}\n",
							order->sq_local_id, order->match_price, order->match_qty);
					if (remain_qty == 0)
					{
						order->change_status(status_all_trade);
					}
					else
					{
						order->change_status(status_part_trade);
					}
					add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
				}

				if (remain_qty > 0 && order->price == m_driver_next.m_mpa && m_driver_next.m_mva > 0)
				{
					if (m_driver_next.m_mpa == order->price)
					{
						order->match_price = m_driver_next.m_mpa;
						int match_vol = (qty_t)(m_driver_next.m_mva / 2);
						order->match_qty = match_vol > remain_qty ? remain_qty : match_vol;

						remain_qty -= order->match_qty;
						order->match_total += order->match_qty;
						log_info("buy_order all_match:[mpa],sq_local_id={},match_price={},match_qty={}\n",
								order->sq_local_id,order->match_price, order->match_qty);
						if (remain_qty == 0)
						{
							order->change_status(status_all_trade);
						}
						else
						{
							order->change_status(status_part_trade);
						}
						add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
					}
				}
				if (remain_qty > 0)
				{
					if (m_match_price_type == match_price_otherside)
					{
						order->match_price = m_cur_quot->bid_price();
					}
					else
					{
						order->match_price = order->price;
					}
					order->match_qty = remain_qty;

					remain_qty -= order->match_qty;
					order->match_total += order->match_qty;
					log_info( "buy_order all_match:[order],sq_local_id={},match_price={},match_qty={}\n",
							order->sq_local_id, order->match_price, order->match_qty);
					if (remain_qty == 0)
					{
						order->change_status(status_all_trade);
					}
					else
					{
						order->change_status(status_part_trade);
					}

					add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
				}
			}

			if (order->direction == f_sell) // 先高价成交 ，再低价成交
			{
				if (remain_qty > 0 && order->price == m_driver_next.m_mpa && m_driver_next.m_mva > 0)
				{

					order->match_price = m_driver_next.m_mpa;
					int match_vol = (qty_t)(m_driver_next.m_mva / 2);
					order->match_qty = match_vol > remain_qty ? remain_qty : match_vol;

					remain_qty -= order->match_qty;
					order->match_total += order->match_qty;
					log_info("buy_order all_match:[mpa],sq_local_id={},match_price={},match_qty={}\n",
							order->sq_local_id,  order->match_price, order->match_qty);
					if (remain_qty == 0)
					{
						order->change_status(status_all_trade);
					}
					else
					{
						order->change_status(status_part_trade);
					}
					add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
				}

				if (remain_qty > 0 && order->price == m_driver_next.m_mpb && m_driver_next.m_mvb > 0)
				{
					if (m_driver_next.m_mpb == order->price)
					{
						order->match_price = m_driver_next.m_mpb;
						int match_vol = m_driver_next.m_mvb / 2;
						match_vol = (match_vol == 0) ? 1 : match_vol;
						order->match_qty = match_vol > remain_qty ? remain_qty : match_vol;

						remain_qty -= order->match_qty;
						order->match_total += order->match_qty;
						log_info("buy_order all_match:[mpb],sq_local_id={},match_price={},match_qty={}\n",
								order->sq_local_id, order->match_price, order->match_qty);
						if (remain_qty == 0)
						{
							order->change_status(status_all_trade);
						}
						else
						{
							order->change_status(status_part_trade);
						}
						add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
					}
				}

				if (remain_qty > 0)
				{
					if (m_match_price_type == match_price_otherside)
					{
						order->match_price = m_cur_quot->bid_price();
					}
					else
					{
						order->match_price = order->price;
					}
					order->match_qty = remain_qty;

					remain_qty -= order->match_qty;
					order->match_total += order->match_qty;
					log_info("buy_order all_match:[order],sq_local_id={},match_price={},match_qty={}\n",
							order->sq_local_id, order->match_price, order->match_qty);
					if (remain_qty == 0)
					{
						order->change_status(status_all_trade);
					}
					else
					{
						order->change_status(status_part_trade);
					}
					assert(remain_qty == 0);
					add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
				}
			}
			assert(remain_qty == 0);
		}
		else
		{
			// 旧逻辑
			if (m_match_price_type == match_price_otherside)
			{
				if (order->direction == f_buy)
				{
					order->match_price = m_cur_quot->ask_price();
				}
				else
				{
					order->match_price = m_cur_quot->bid_price();
				}
			}
			else
			{
				order->match_price = order->price;
			}
			order->match_qty = order->qty;
			assert(order->match_qty > 0);
			order->match_total = order->qty;
			order->change_status(status_all_trade);
			log_info("all_match_in_driver:[flat],sq_local_id={},match_price={},match_qty={}\n",
					order->sq_local_id,  order->match_price, order->match_qty);
			add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
		}
	}
	void td_sim_driver::decrease_handup_qty(price_t price, int direction, qty_t vol)
	{
		if (direction == f_buy)
		{
			auto it = m_buy_handup_info.find(price);
			if (it != m_buy_handup_info.end())
			{
				it->second.m_total_qty -= vol;
				assert(it->second.m_total_qty >= 0);
			}
			else
			{
				log_warn("no price\n");
			}
		}
		else
		{
			auto it = m_sell_handup_info.find(price);
			if (it != m_sell_handup_info.end())
			{
				it->second.m_total_qty -= vol;
				assert(it->second.m_total_qty >= 0);
			}
			else
			{
				log_warn("no price\n");
			}
		}
	}
	// 执行挂单逻辑
	void td_sim_driver::handup_order(sq_order_record *order, qty_t new_order, qty_t beforeme)
	{
		log_debug("handup order [case={}] sq_local_id={},price={},beforeme={}\n",
				m_handup_case,
				order->sq_local_id, order->price,
				beforeme);

		// 9上买 排队
		if (order->direction == f_buy)
		{
			auto it = m_buy_handup_info.find(order->price);
			if (it == m_buy_handup_info.end())
			{
				m_buy_handup_info[order->price] = handup_info(); // std::list<handup_order_info*>();
				it = m_buy_handup_info.find(order->price);
			}
			handup_order_info *info = new handup_order_info();
			info->order = order;
			info->handup_pkg_no = m_cur_quot->pkg_no;
			info->handup = &it->second;

			qty_t my_order_qty = it->second.m_total_qty;
			// 重新计算一下my_order
			{
				auto my_order_it = it->second.m_orders.begin();
				int total = 0;
				for (; my_order_it != it->second.m_orders.end(); ++my_order_it)
				{
					handup_order_info *info = *my_order_it;
					if (info->order->is_finish == false)
					{
						int remain = info->order->qty - info->order->match_total;
						total += remain;
					}
				}
				assert(total == my_order_qty);
			}
			// 模式1 固定值
			if (m_beforeme_type == 0)
			{
				info->before_me = beforeme;
				if (info->before_me > m_next_quot->bid_qty())
				{
					info->before_me = m_next_quot->bid_qty();
				}
			}
			// 模式2 比例
			else
			{
				info->before_me = beforeme;
			}

			info->before_me += my_order_qty;

			// 记录所有订单量
			it->second.m_total_qty += order->qty;

			it->second.m_orders.push_back(info);

			log_debug("handup order on buylist:[case={}] sq_local_id={},price={},beforme={}\n",
					m_handup_case,
					info->order->sq_local_id, info->order->price,
					info->before_me);
		}
		else
		{
			auto it = m_sell_handup_info.find(order->price);
			if (it == m_sell_handup_info.end())
			{
				m_sell_handup_info[order->price] = handup_info(); // std::list<handup_order_info*>();
				it = m_sell_handup_info.find(order->price);
			}
			handup_order_info *info = new handup_order_info();
			info->order = order;
			info->handup_pkg_no = m_cur_quot->pkg_no;
			info->handup = &it->second;

			qty_t my_order_qty = it->second.m_total_qty;
			// 重新计算一下my_order
			{
				auto my_order_it = it->second.m_orders.begin();
				int total = 0;
				for (; my_order_it != it->second.m_orders.end(); ++my_order_it)
				{
					handup_order_info *info = *my_order_it;
					if (info->order->is_finish == false)
					{
						int remain = info->order->qty - info->order->match_total;
						total += remain;
					}
				}
				assert(total == my_order_qty);
			}
			// 模式1 固定值
			if (m_beforeme_type == 0)
			{

				info->before_me = beforeme;
				if (info->before_me > m_next_quot->ask_qty())
				{
					info->before_me = m_next_quot->ask_qty();
				}
			}

			// 模式2 比例
			else
			{
				info->before_me = beforeme;
			}
			info->before_me += my_order_qty;

			// 记录所有订单量
			it->second.m_total_qty += order->qty;

			it->second.m_orders.push_back(info);
			log_debug("handup order on selllist,[case={}] sq_local_id={},price={},beforeme={}\n",
					m_handup_case,
					info->order->sq_local_id, info->order->price,
					info->before_me);
		}
	}

	void td_sim_driver::on_order_cancel(sq_order_record *order)
	{
		decrease_handup_qty(order->price, order->direction, order->qty - order->match_total);
	}

	// 新来的订单
	void td_sim_driver::on_order_insert(sq_order_record *order)
	{
		int order_direction = order->direction;
		price_t order_price = order->price;
		m_new_order = 0;
		qty_t beforeme = 0;

		qty_t next_val = 0;
		int next_direction = get_next_quot_info(order->price, next_val);
		qty_t cur_val = 0;
		//int cur_direction = get_cur_quot_info(order->price, cur_val);

		// 如果最优价，直接触发成交
		if (m_enable_best_match)
		{
			if (order_direction == f_buy && order_price >= m_cur_quot->ask_price())
			{
				send_all_match(order);
				return;
			}
			else if (order_direction == f_sell && order_price <= m_cur_quot->bid_price())
			{
				send_all_match(order);
				return;
			}
		}

		// 价格不存在了
		if (next_direction == -1)
		{
			if (order_direction == f_buy)
			{
				if (order_price > m_next_quot->ask_price())
				{
					// 全部成交

					send_all_match(order);
				}

				else if (order_price < m_next_quot->ask_price() &&
						 order_price > m_next_quot->bid_price())
				{
					// (9/10 -> 9/11)
					if (order_price == m_driver_next.m_mpa)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttS;
						m_handup_case = 1;
					}
					//(9 / 10 -> 8 / 11)
					else if (order_price == m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttB;
						m_handup_case = 2;
					}
					//(8 / 10->8 / 12)
					else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = 0;
						m_handup_case = 3;
					}
					else
					{
						assert(false);
					}
					// 没有人跟我们竞争
					handup_order(order, m_new_order, beforeme);
				}

				else if (order_price < m_next_quot->bid_price()) // 我比下一个bid1差，但我能排第一
				{
					//   (9 / 10->11 / 12)
					if (order_price == m_driver_next.m_mpa)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttS;
						m_handup_case = 4;
					}
					// (9 / 10->11 / 12)
					else if (order_price == m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttB;
						m_handup_case = 5;
					}
					//  (8 / 10->11 / 12)
					else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = 0;
						m_handup_case = 6;
					}
					else
					{
						assert(false);
					}
					// 没有人跟我们竞争
					handup_order(order, m_new_order, beforeme);
				}
			}
			// 卖
			else
			{
				//(9 / 10 -> 11 / 12)
				if (order_price < m_next_quot->bid_price()) // 我比下一个价格的更优，我应该能成交
				{
					// 我全部成交了

					send_all_match(order);
				}

				else if (order_price > m_next_quot->bid_price() &&
						 order_price < m_next_quot->ask_price())
				{
					//   (9 / 10->8 / 11) (9 / 10->8 / 10)
					if (order_price == m_driver_next.m_mpa)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttS;
						m_handup_case = 7;
					}
					else if (order_price == m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttB;
						m_handup_case = 8;
					}
					else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = 0;
						m_handup_case = 9;
					}
					else
					{
						assert(false);
					}
					// 没有人跟我们竞争
					handup_order(order, m_new_order, beforeme);
				}

				else if (order_price > m_next_quot->ask_price())
				{
					// (9 / 10-> 8 / 9)
					if (order_price == m_driver_next.m_mpa)
					{
						m_new_order = 0;
						beforeme = (qty_t)m_driver_next.m_AttS;
						m_handup_case = 10;
					}
					else if (order_price == m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = m_driver_next.m_AttB;
						m_handup_case = 11;
					}
					else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
					{
						m_new_order = 0;
						beforeme = 0;
						m_handup_case = 12;
					}
					else
					{
						assert(false);
					}
					// 没有人跟我们竞争
					handup_order(order, m_new_order, beforeme);
				}
			}
		}
		// 订单是买,在下一笔行情中这个价格还是买
		else if (order_direction == f_buy && next_direction == f_buy)
		{
			if (order_price == m_next_quot->bid_price())
			{
				//  (9 / 10->10 / 11)
				if (order_price == m_driver_next.m_mpa)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 13;
					}
					else
					{
						m_new_order = m_driver_next.m_AttS + next_val;
						beforeme = m_new_order * m_beforeme_rate;
						m_handup_case = 14;
					}
				}
				//  (9 / 10->9 / 10)(9 / 10->9 / 11)
				else if (order_price == m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 15;
					}
					else
					{
						m_new_order = m_driver_next.m_AttB + next_val - cur_val;
						beforeme = m_new_order * m_beforeme_rate + cur_val;
						m_handup_case = 16;
					}
				}
				//   (7 / 10->8 / 11) (8 / 10 ->8 / 10)
				else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 17;
					}
					else
					{
						m_new_order = next_val - cur_val;
						beforeme = m_new_order * m_beforeme_rate + cur_val;
						m_handup_case = 18;
					}
				}
				else
				{
					assert(false);
				}
				handup_order(order, m_new_order, beforeme);
			}
			else if (order_price < m_next_quot->bid_price())
			{
				//  (9 / 10 -> 11 / 12)
				if (order_price == m_driver_next.m_mpa)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = (qty_t)m_new_order;
						m_handup_case = 19;
					}
					else
					{
						m_new_order = m_driver_next.m_AttS + next_val;
						beforeme = m_new_order * 1;
						m_handup_case = 20;
					}
				}
				//   (10 / 11 -> 11 / 12 order_price == 10) (8 / 10 -> 11 / 12)
				else if (order_price == m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 21;
					}
					else
					{
						m_new_order = m_driver_next.m_AttB + next_val - cur_val;
						beforeme = (m_driver_next.m_AttB + next_val) * 1;
						m_handup_case = 22;
					}
				}
				//   (8 / 10->9 / 11 order_price == 8)
				else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 23;
					}
					else
					{
						m_new_order = next_val - cur_val;
						beforeme = (next_val)*1;
						m_handup_case = 24;
					}
				}
				else
				{
					assert(false);
				}

				handup_order(order, m_new_order, beforeme);
			}
		}
		// 订单是卖,在下一笔行情中是卖
		else if (order_direction == f_sell && next_direction == f_sell)
		{
			// 处理原理同上
			if (order_price == m_next_quot->ask_price())
			{
				//(9 / 10->8 / 9)
				if (order_price == m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 25;
					}
					else
					{
						m_new_order = m_driver_next.m_AttB + next_val;
						beforeme = (m_driver_next.m_AttB + next_val) * m_beforeme_rate;
						m_handup_case = 26;
					}
				}
				// (9 / 10->9 / 10)(9 / 10->8 / 10)
				else if (order_price == m_driver_next.m_mpa)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = (qty_t)m_new_order;
						m_handup_case = 27;
					}
					else
					{
						m_new_order = m_driver_next.m_AttS + next_val - cur_val;
						beforeme = (m_driver_next.m_AttS + next_val - cur_val) * m_beforeme_rate + cur_val;
						m_handup_case = 28;
					}
				}
				// (7 / 10->6 / 8) (8 / 10 ->8 / 10)
				else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 29;
					}
					else
					{
						m_new_order = next_val - cur_val;
						beforeme = cur_val + (next_val - cur_val) * m_beforeme_rate;
						m_handup_case = 30;
					}
				}
				else
				{
					assert(false);
				}
				handup_order(order, m_new_order, beforeme);
			}

			else if (order_price > m_next_quot->ask_price()) // 跌的剧烈认为rankp=1
			{
				//(9 / 10 -> 7 / 8)
				if (order_price == m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 31;
					}
					else
					{
						m_new_order = m_driver_next.m_AttB + next_val;
						beforeme = (m_driver_next.m_AttB + next_val) * 1;
						m_handup_case = 32;
					}
				}
				// (9 / 10 -> 7 / 8 order_price = 10)
				else if (order_price == m_driver_next.m_mpa)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 33;
					}
					else
					{
						m_new_order = m_driver_next.m_AttS + next_val - cur_val;
						beforeme = (m_driver_next.m_AttS + next_val) * 1;
						m_handup_case = 34;
					}
				}
				// (8 / 10->7 / 9 order_price == 10)
				else if (order_price != m_driver_next.m_mpa && order_price != m_driver_next.m_mpb)
				{
					if (m_beforeme_type == 0)
					{
						m_new_order = m_beforeme_fix;
						beforeme = m_new_order;
						m_handup_case = 35;
					}
					else
					{
						m_new_order = next_val - cur_val;
						beforeme = (next_val)*1;
						m_handup_case = 36;
					}
				}
				else
				{
					assert(false);
				}
				handup_order(order, m_new_order, beforeme);
			}
		}

		// 订单是买,在下一笔行情中这个价格是卖（反方向）
		else if (order_direction == f_buy && next_direction == f_sell)
		{
			//(9 / 10->9 / 10) (9 / 10->8 / 9)
			//	这种情况全部成交，因为我用目前的价格买，下笔行情，还有这个价格的卖，说明我能成交
			send_all_match(order);
		}

		else if (order_direction == f_sell && next_direction == f_buy)
		{
			//(9 / 10->9 / 10) (9 / 10->10 / 11)
			//	这种情况全部成交，因为我用目前的价格卖，下笔行情，还有这个价格的买，说明我能成交
			send_all_match(order);
		}
	}
	// 按行情撮合
	void td_sim_driver::match(sq_quot *quot, driver_match_info &driver)
	{
		if (m_buy_handup_info.empty() == false || m_sell_handup_info.empty() == false)
		{
			// 处理所有挂单
			//	double cur_mid = (m_cur_quot->bid_price + m_cur_quot->ask_price)*0.5;
			//	double pre_mid = (m_pre_quot->bid_price + m_pre_quot->ask_price)*0.5;

			// 优先对价成交
			auto it = m_match_order_table.m_Index.begin();
			for (; it != m_match_order_table.m_Index.end(); ++it)
			{
				sq_order_record *order = it->second;
				if (order->is_finish == false)
				{
					_exe_match_by_quot(order, quot);
				}
			}
			std::vector<price_t> match_price;
			std::vector<qty_t> match_qty;
			// 处理挂单
			if (driver.m_mva > 0 && driver.m_mvb == 0)
			{ // 涨
				match_price.push_back(driver.m_mpa);
				match_qty.push_back(driver.m_mva / 2);
			}
			else if (driver.m_mva == 0 && driver.m_mvb > 0) // 跌
			{
				match_price.push_back(driver.m_mpb);
				match_qty.push_back(driver.m_mvb / 2);
			}
			else if (driver.m_mva > 0 && driver.m_mvb > 0) // 平
			{
				match_price.push_back(driver.m_mpa);
				match_price.push_back(driver.m_mpb);

				match_qty.push_back(driver.m_mva / 2);
				match_qty.push_back(driver.m_mvb / 2);
			}

			for (int i = 0; i < (int)match_price.size(); i++)
			{
				double mprice = match_price[i];
				int mqty = match_qty[i];
				// buy
				auto it_bid = m_buy_handup_info.find(mprice);
				if (it_bid != m_buy_handup_info.end())
				{
					auto it_order = it_bid->second.m_orders.begin();

					for (; it_order != it_bid->second.m_orders.end(); ++it_order)
					{
						handup_order_info *info = (*it_order);
						if (info->order->is_finish == false)
						{
							if (info->before_me <= mqty)
							{

								if (m_match_price_type == match_price_otherside)
								{
									info->order->match_price = m_cur_quot->bid_price();
								}
								else
								{
									info->order->match_price = info->order->price;
								}
								qty_t remain_qty = (info->order->qty - info->order->match_total);
								qty_t can_attach_qty = mqty - info->before_me;
								qty_t can_matched = (std::min)(remain_qty, can_attach_qty);
								if (can_matched > 0)
								{

									info->order->match_qty = can_matched;
									info->order->match_total += can_matched;
									if (info->order->match_total >= info->order->qty) //???
										info->order->change_status(status_all_trade);
									else
										info->order->change_status(status_part_trade);

									log_debug("handup_matched sq_local_id={},att_qty={},match_qty={},used_time={}\n",
											info->order->sq_local_id,
											mqty, info->order->match_qty, m_cur_quot->pkg_no - info->handup_pkg_no);

									add_result(tid_order_match, (char *)info->order, sizeof(sq_order_record));
									decrease_handup_qty(info->order->price, info->order->direction, info->order->match_qty);
								}
								info->before_me = 0;
							}
							else
							{

								info->before_me -= mqty;
								log_debug( "before_me sq_local_id={},att_qty={},before_me={}\n", info->order->sq_local_id,
										mqty, info->before_me);
							}
						}
					}
				}

				// sell
				auto it_ask = m_sell_handup_info.find(mprice);
				if (it_ask != m_sell_handup_info.end())
				{
					auto it_order = it_ask->second.m_orders.begin();

					for (; it_order != it_ask->second.m_orders.end(); ++it_order)
					{

						handup_order_info *info = (*it_order);
						if (info->order->is_finish == false)
						{

							if (info->before_me <= mqty)
							{

								if (m_match_price_type == match_price_otherside)
								{
									info->order->match_price = m_cur_quot->ask_price();
								}
								else
								{
									info->order->match_price = info->order->price;
								}

								qty_t remain_qty = (info->order->qty - info->order->match_total);
								qty_t can_attach_qty = mqty - info->before_me;
								qty_t can_matched = (std::min)(remain_qty, can_attach_qty);
								if (can_matched > 0)
								{

									info->order->match_qty = can_matched;
									info->order->match_total += can_matched;
									if (info->order->match_total >= info->order->qty)
										info->order->change_status(status_all_trade);
									else
										info->order->change_status(status_part_trade);

									log_debug("handup_matched sq_local_id={},att_qty={},match_qty={},used_time={}\n",
											info->order->sq_local_id,
											mqty, info->order->match_qty,
											 m_cur_quot->pkg_no - info->handup_pkg_no);

									add_result(tid_order_match, (char *)info->order, sizeof(sq_order_record));
									decrease_handup_qty(info->order->price, info->order->direction, info->order->match_qty);
								}

								info->before_me = 0;
							}
							else
							{
								info->before_me -= mqty;
								log_debug("before_me sq_local_id={},att_qty={},beforeme=%{}\n",
										info->order->sq_local_id,
										mqty,
										info->before_me);
							}
						}
					}
				}
			}
		}
	}

	bool td_sim_driver::_exe_match_by_quot(sq_order_record *order, sq_quot *quot)
	{
		if (order->is_finish)
			return false;

		if (quot == nullptr)
			return false;

		assert(quot->contract.to_string() == order->contract.to_string());
#if 1
		// 撮合
		double match_price = order->price; // 成交价
		qty_t match_qty = order->qty - order->match_total;
		assert(match_qty > 0);
		// bool bMatch = false;
		// 市价单,全部成交
		if (ot_get_type(order->type) == OT_MARKET)
		{
			if (order->direction == f_buy)
			{
				if (match_price == 0)
					match_price = quot->ask_price();
			}
			else
			{
				if (match_price == 0)
					match_price = quot->bid_price();
			}
		}
		// 对价
		else if (order->direction == 0 && order->price >= (quot->ask_price()))
		{
			match_qty = order->qty - order->match_total;
			// match_qty = (std::min)(match_qty, quot->ask_qty);
			if (m_match_price_type == match_price_otherside)
			{
				match_price = quot->ask_price();
			}
		}
		// 对价
		else if (order->direction == 1 && order->price <= (quot->bid_price()))
		{
			// match_qty = (std::min)(match_qty, quot->bid_qty);
			match_qty = order->qty - order->match_total;
			if (m_match_price_type == match_price_otherside)
			{
				match_price = quot->bid_price();
			}
		}

		else
		{
			match_qty = 0;
		}
		if (match_qty > 0)
		{
			order->match_price = match_price;
			order->match_qty = match_qty;
			order->match_total += order->match_qty;
			assert(order->match_qty > 0);
			assert(order->match_price > 0);

			if (order->match_total >= order->qty)
			{
				order->status = status_all_trade;
				order->is_finish = true;
				log_info( "all matched for quot change id={}\n", order->sq_local_id);
			}
			else
			{
				order->status = status_part_trade;
				log_info("part matched for quot change id=%{},match_price={},match_qty={}\n", order->sq_local_id,
						order->match_price, order->match_qty);
			}
			add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
			decrease_handup_qty(order->price, order->direction, order->match_qty);

			return true;
		}
#endif

		// 处理fak
		if (order->qty - order->match_total > 0 && ot_get_attr(order->type) == OT_FAK)
		{
			order->status = status_cancel;
			order->err_code = 0;
			order->err_msg = "fak cancel";
			order->is_finish = true;
			add_result(tid_order_state, (char *)order, sizeof(sq_order_record));
			assert(false); // 不支持
		}

		return false;
	}


	void td_sim_driver::match_sim_cancel(sq_quot *quot, driver_match_info &driver, std::vector<match_result> &result)
	{
		if (m_buy_handup_info.empty() == false || m_sell_handup_info.empty() == false)
		{
			// 处理所有挂单
			//	double cur_mid = (m_cur_quot->bid_price + m_cur_quot->ask_price)*0.5;
			//	double pre_mid = (m_pre_quot->bid_price + m_pre_quot->ask_price)*0.5;

			// 优先对价成交
			auto it = m_match_order_table.m_Index.begin();
			for (; it != m_match_order_table.m_Index.end(); ++it)
			{
				sq_order_record *order = it->second;
				if (order->is_finish == false)
				{
					_exe_match_by_quot_sim_cancel(order, quot, result);
				}
			}
			std::vector<double> match_price;
			std::vector<qty_t> match_qty;
			// 处理挂单
			if (driver.m_mva > 0 && driver.m_mvb == 0)
			{ // 涨
				match_price.push_back(driver.m_mpa);
				match_qty.push_back((qty_t)(driver.m_mva / 2));
			}
			else if (driver.m_mva == 0 && driver.m_mvb > 0) // 跌
			{
				match_price.push_back(driver.m_mpb);
				match_qty.push_back(driver.m_mvb / 2);
			}
			else if (driver.m_mva > 0 && driver.m_mvb > 0) // 平
			{
				match_price.push_back(driver.m_mpa);
				match_price.push_back(driver.m_mpb);

				match_qty.push_back(driver.m_mva / 2);
				match_qty.push_back(driver.m_mvb / 2);
			}

			for (int i = 0; i < (int)match_price.size(); i++)
			{
				double mprice = match_price[i];
				int mqty = int(match_qty[i]);
				// buy
				auto it_bid = m_buy_handup_info.find(mprice);
				if (it_bid != m_buy_handup_info.end())
				{
					auto it_order = it_bid->second.m_orders.begin();

					for (; it_order != it_bid->second.m_orders.end(); ++it_order)
					{
						handup_order_info *info = (*it_order);
						if (info->order->is_finish == false)
						{
							if (info->before_me <= mqty)
							{
								match_result mret;
								mret.order_id = info->order->sq_local_id;
								mret.direction = f_buy;
								if (m_match_price_type == match_price_otherside)
								{
									mret.match_price = m_cur_quot->bid_price();
								}
								else
								{
									mret.match_price = info->order->price;
								}
								qty_t remain_qty = (info->order->qty - info->order->match_total);
								qty_t can_attach_qty = mqty - info->before_me;
								qty_t can_matched = (std::min)(remain_qty, can_attach_qty);
								if (can_matched > 0)
								{

									mret.match_qty = can_matched;

									//qty_t match_total = info->order->match_total + mret.match_qty;

									result.push_back(mret);
									log_info("pre_match for cancel order by sim_match_cancel sq_local_id={},match_price={},match_qty={}\n",
											mret.order_id, mret.match_price, mret.match_qty);
								}
								// info->before_me = 0;
							}
							else
							{

								/*info->before_me -= mqty;
									mqty, info->before_me);*/
							}
						}
					}
				}

				// sell
				auto it_ask = m_sell_handup_info.find(mprice);
				if (it_ask != m_sell_handup_info.end())
				{
					auto it_order = it_ask->second.m_orders.begin();

					for (; it_order != it_ask->second.m_orders.end(); ++it_order)
					{

						handup_order_info *info = (*it_order);
						if (info->order->is_finish == false)
						{

							if (info->before_me <= mqty)
							{
								match_result mret;
								mret.order_id = info->order->sq_local_id;
								mret.direction = f_sell;
								if (m_match_price_type == match_price_otherside)
								{
									mret.match_price = m_cur_quot->ask_price();
								}
								else
								{
									mret.match_price = info->order->price;
								}

								qty_t remain_qty = (info->order->qty - info->order->match_total);
								qty_t can_attach_qty = mqty - info->before_me;
								qty_t can_matched = (std::min)(remain_qty, can_attach_qty);
								if (can_matched > 0)
								{

									mret.match_qty = can_matched;

									//qty_t match_total = info->order->match_total + mret.match_qty;

									result.push_back(mret);
									log_info("pre_match for cancel order by sim_match_cancel sq_local_id={},match_price={},match_qty={}\n",
											mret.order_id, mret.match_price, mret.match_qty);
								}

								// info->before_me = 0;
							}
							else
							{
								// info->before_me -= mqty;
								//	mqty, info->before_me);
							}
						}
					}
				}
			}
		}
	}

	bool td_sim_driver::_exe_match_by_quot_sim_cancel(sq_order_record *order, sq_quot *quot, std::vector<match_result> &result)
	{
		if (order->is_finish)
			return false;

		if (quot == nullptr)
			return false;

		assert(quot->contract.to_string() == order->contract.to_string());

		// 撮合
		double match_price = order->price; // 成交价
		qty_t match_qty = order->qty - order->match_total;
		assert(match_qty > 0);
		// bool bMatch = false;
		// 市价单,全部成交
		if (ot_get_type(order->type) == OT_MARKET)
		{
			if (order->direction == f_buy)
			{
				if (match_price == 0)
					match_price = quot->ask_price();
			}
			else
			{
				if (match_price == 0)
					match_price = quot->bid_price();
			}
		}
		// 对价
		else if (order->direction == 0 && order->price >= (quot->ask_price()))
		{
			match_qty = (order->qty - order->match_total);

			if (m_match_price_type == match_price_otherside)
			{
				match_price = quot->ask_price();
			}
		}
		// 对价
		else if (order->direction == 1 && order->price <= (quot->bid_price()))
		{

			match_qty = (order->qty - order->match_total);
			if (m_match_price_type == match_price_otherside)
			{
				match_price = quot->bid_price();
			}
		}

		else
		{
			match_qty = 0;
		}
		if (match_qty > 0)
		{
			match_result mret;
			mret.order_id = order->sq_local_id;
			mret.direction = order->direction;
			mret.match_price = match_price;
			mret.match_qty = match_qty;
			//qty_t match_total = order->match_total + order->match_qty;

			log_info("pre_match for cancel order by quot change sq_local_id={},match_price={},match_qty={}\n",
					mret.order_id, mret.match_price, mret.match_qty);

			result.push_back(mret);
			// add_result(tid_order_match, (char*)order, sizeof(sq_order_record));
			// decrease_handup_qty(order->price, order->direction, order->match_qty);

			return true;
		}

		return false;
	}

}