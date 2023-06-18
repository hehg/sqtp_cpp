#include "td_sim_by_quot.h"
#include <set>
#include "error/sq_err.h"
#include "math/math_util.h"
#include "time/date_time.h"
#include "log/sq_logger.h"

#include "sq_type_define.h"
using namespace sq;
namespace sq_plug {
   static  bool cmp_by_order_id(sq_order_record*l, sq_order_record*r) {
        return l->sq_local_id < r->sq_local_id;
    }

    td_sim_by_quot::td_sim_by_quot()
    {
		
    }
    td_sim_by_quot::~td_sim_by_quot()
    {

    }
	
    int td_sim_by_quot::open()
	{

        m_multi = get_cfg_int("multi", 1);
        m_cur_quot.multi = m_multi;
        m_quot_file_path = get_cfg_string("quot_file");
        m_quot_file_have_header = get_cfg_bool("quot_file_have_header");
        if (!m_quot_file_have_header)
        {
            m_quot_file_header = get_cfg_string("quot_file_header");
            m_quot_reader.set_header(m_quot_file_header.c_str());
        }
        bool ret = m_quot_reader.open(m_quot_file_path.c_str(), m_quot_file_have_header);

        if (ret == false)
        {
            log_fata("not exit quot file {}\n", m_quot_file_path.c_str());
        }
       // m_triger_type=get_cfg_string("triger_type","quot");

        vector<string> contracts = get_cfg_arrary("support_contract");
		if (!contracts.empty())
		{
			//m_quot_table.add(contracts[0]);
		}

        
		read_quot_wait_time =get_cfg_int("read_quot_wait_time", 1);

        m_run_flag=true;
		m_eg = new thread(&td_sim_by_quot::run, this);
		log_info("start plug_td_sim_driver\n");
		return ok;
    }
    void td_sim_by_quot::run()
    {
        printf("begin run engine thread\n");
		while (m_run_flag)
		{
            sq_quot*quot=read_quot();
			if(quot==nullptr)
			{
				printf("read_quot finish\n");
				break;
			}
            match(quot);
            // 发送行情
			add_result(quot->tid, (char *)quot, sizeof(sq_quot));
			if(read_quot_wait_time>0)
				std::this_thread::sleep_for(std::chrono::milliseconds(read_quot_wait_time));
			

			handle_package();
		}

		printf("stop run engine thread\n");
    }
    int td_sim_by_quot::close()
    {

        m_run_flag = false;
        m_eg->join();
        delete m_eg;
        return ok;
    }
    int td_sim_by_quot::put(uint16_t tid, char *data, uint16_t size)
    {
        if (tid == tid_place_order)
        {
            sq_req_order *req = (sq_req_order *)data;
            place_order(req);
        }
        else if (tid == tid_cancel_order)
        {
            sq_req_cancel_order *req = (sq_req_cancel_order *)data;
            cancel_order(req);
        }
        else
        {
            log_fata("not support tid={}\n", tid);
        }
        return ok;
    }
    sq_quot* td_sim_by_quot::read_quot()
	{
		while (true) {
			bool ret = m_quot_reader.read_line(m_cur_quot);
			if (ret) {
                return &m_cur_quot;
				// if (m_support_all) {
				// 	return &m_cur_quot;
				// }
				// if (m_support_all == false && is_support_contract(m_cur_quot.contract.data())) {
				// 	return &m_cur_quot;
				// }
				// else {
				// 	//printf ("%s:%d not support contract: %s\n", __FILE__, __LINE__, m_cur_quot.contract.data());
				// 	continue;
				// }
			}
			else {
				return nullptr;
			}
		}
		
	}
    //按行情撮合
    void td_sim_by_quot::match(sq_quot*quot)
    {
        //计算一下2个tick 之间的成交量
        sq_quot*pre_quot=m_last_tick_quot.find(quot->contract.data());
        int tick_match_qty=0;
        double tick_turnover=0;
        double tick_avg_price=0;
        if(pre_quot){
            tick_match_qty=quot->match_qty-pre_quot->match_qty;
            tick_turnover=quot->turnover-pre_quot->turnover;
            if(tick_match_qty>0 && tick_turnover>0){
                tick_avg_price=tick_turnover/tick_match_qty;
            }
            assert(tick_avg_price>=0);
        }


        //每个订单进行一次撮合
        vector<sq_order_record*> out;
        //找买方向,价格>=quot->ask_price
		if (!m_contract_order_price_queue.empty()) {
            double can_match_price=quot->ask_price();
            if(tick_avg_price>0 && tick_avg_price<can_match_price){
                can_match_price=tick_avg_price;
            }
			m_contract_order_price_queue[quot->contract.to_string()].search(f_buy, can_match_price,
				price_level_table::op_great_eq, out);
		}
       
        //找买方向,价格<=quot->bid_price
		if (!m_contract_order_price_queue.empty()) {
            double can_match_price=quot->bid_price();
            if(tick_avg_price>0 && tick_avg_price>can_match_price){
                can_match_price=tick_avg_price;
            }
			m_contract_order_price_queue[quot->contract.to_string()].search(f_sell, can_match_price,
				price_level_table::op_less_eq, out);
		}
        //存在能够成交的订单
        if (out.size() > 0)
        {
            std::sort(out.begin(), out.end(), cmp_by_order_id);

            std::vector<sq_order_record *>::iterator it = out.begin();

            for (; it != out.end(); ++it)
            {
                sq_order_record *order = *it;
                //已经结束的订单
                if (order->is_finish == true)
                {
                    continue;
                }
                bool ret = _exe_match_by_quot(order, quot,tick_avg_price);

                if (order->is_finish == true)
                {
                    sq_order_record *i = m_contract_order_price_queue[quot->contract.to_string()].remove(order->direction,
                                                                                                         order->price, order->sq_local_id);
                    if (i == nullptr)
                    {
                        assert(false);
                    }
                }
            }
        }
        //记录这笔行情
        m_last_tick_quot.update(quot->contract.data(),*quot);
    }

    bool td_sim_by_quot::_exe_match_by_quot(sq_order_record*order, sq_quot*quot,double tick_avg_price)
    {

        //撮合
        double  match_price = order->price;//成交价先设置为下单价格
        qty_t     match_qty = order->qty - order->match_total;
        assert(match_qty > 0);
       
         //市价单,全部成交
        if (ot_get_type(order->type)==OT_MARKET) {
            if (order->direction == f_buy) {
                if (match_price == 0)
                    match_price = quot->ask_price();
            }
            else {
                if (match_price == 0)
                    match_price = quot->bid_price();
            }

        }
        //对价
        else if (order->direction ==f_buy && order->price >= (quot->ask_price()))
        {
            match_price = quot->ask_price();
        }
        //对价
        else if (order->direction == f_sell && order->price <= (quot->bid_price()))
        {
            match_price = quot->bid_price();
        }
        else if (order->direction == f_buy && 
			order->price >= quot->bid_price() &&order->price < quot->ask_price())
        {
            if(tick_avg_price>0 && order->price>=tick_avg_price){
                match_price=tick_avg_price;
            }
        }
        else if (order->direction == f_sell && 
			order->price <= quot->ask_price() &&order->price > quot->bid_price())
        {
             if(tick_avg_price>0 && order->price<=tick_avg_price){
                match_price=tick_avg_price;
            }
        }
        else {
            match_qty = 0;
        }
        if (match_qty > 0) {
            order->match_price = match_price;
            order->match_qty = match_qty;
            order->match_total += order->match_qty;
            assert(order->match_qty > 0);
            assert(order->match_price > 0);

            if (order->match_total >= order->qty) {
                order->status = status_all_trade;
                order->is_finish = true;
                log_info("all matched for quot change id={}\n", order->sq_local_id);

            }
            else {
                order->status = status_part_trade;
                log_info("part matched for quot change id={}\n", order->sq_local_id);

            }
            add_result(tid_order_match, (char*)order, sizeof(sq_order_record));

        }

        //处理fak
        if (order->qty - order->match_total > 0 && ot_get_attr(order->type)==OT_FAK)
        {
            order->status = status_cancel;
            order->err_code = 0;
            order->err_msg = "fak cancel";
            order->is_finish = true;
            add_result(tid_order_state, (char*)order, sizeof(sq_order_record));
        }

        return true;
    }


	void td_sim_by_quot::on_order_insert(sq_order_record*order)
	{
       // if(m_triger_type=="order"){
         //   match_by_order(order);
       // }
        
	}
}

