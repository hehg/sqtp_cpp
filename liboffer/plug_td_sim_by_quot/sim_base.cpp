#include "sim_base.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "time/date_time.h"
using namespace sq;
namespace sq_plug
{
    sim_base::sim_base()
    {
        m_input_rb = new ring_buffer<req_msg>();
    }
    sim_base::~sim_base()
    {
        delete m_input_rb;
       
    }

    int  sim_base::place_order(sq_req_order*req)
    {

        req_msg*msg = m_input_rb->back();
        while (msg == nullptr)
        {
            std::this_thread::yield();
            msg = m_input_rb->back();
        }
        msg->msg_type = tid_place_order;
        msg->msg_size = sizeof(sq_req_order);

        sq_order_record *order = (sq_order_record*)msg->msg;
        memset(order, 0, sizeof(sq_order_record));

        order->sq_local_id = req->sq_local_id;
		order->user_id = req->user_id;
        order->contract = req->contract.data();
        order->direction = req->direction;
        order->price = req->price;
        order->qty = req->qty;
        order->offset = req->offset;
        
		sprintf(order->sys_id.data(), "%d", ++m_order_id);
        order->status = status_insert_submit;
        order->is_finish = false;
        order->type = req->type;
        order->match_total = 0;
		assert(order->contract.empty() == false);
        //市价单
        if (ot_get_type(order->type)==OT_MARKET) {
            if (order->price == 0 && order->direction == f_buy) {
                order->price = market_price_up;
            }
            else if (order->price == 0 && order->direction == f_sell) {
                order->price = market_price_down;
            }
        }
       
		log_info("place_order,sq_local_id={},contract={},direction={},offset={},price={},qty={}\n",
			order->sq_local_id,
			req->contract.data(),
			req->direction,
			req->offset,
			req->price,
			req->qty);

        m_input_rb->push();
        
        return ok;
    }
    int sim_base::cancel_order(sq_req_cancel_order*req)
    {
        req_msg*msg = m_input_rb->back();
        while (msg == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            msg = m_input_rb->back();
        }
        msg->msg_type = tid_cancel_order;
        msg->msg_size = sizeof(sq_req_cancel_order);
        memcpy(msg->msg, req, sizeof(sq_req_cancel_order));
        m_input_rb->push();
        log_info("req cancel order sq_local_id={}\n", req->sq_local_id);
        return 0;

    }
    void sim_base::handle_package()
    {
        for (;;) {
            req_msg*cmd = m_input_rb->front();
            if (cmd != nullptr)
            {
                //订单请求
                if (cmd->msg_type == tid_place_order)
                {
                    sq_order_record*req = (sq_order_record*)cmd->msg;
					
                    _exe_place_order(req);

                }
                //撤单
                else if (cmd->msg_type == tid_cancel_order)
                {
                    sq_req_cancel_order*req = (sq_req_cancel_order*)cmd->msg;

                    _exe_cancel_order(req);
                }
                
				else if (cmd->msg_type == tid_query_req) {
					on_query(cmd->msg_type, (char*)cmd->msg, cmd->msg_size);
				}
                
                m_input_rb->pop();
            }
            else {
                break;
            }
        }
    }

    int sim_base::_exe_place_order(sq_order_record *req)
    {
        // 加入订单表
        int ret = m_match_order_table.add(req->sq_local_id, *req);
        if (ret != 0)
        {
            log_fata("repeat sq_local_id={}\n",req->sq_local_id);
            // todo error handler for repeat order;
            return err_repeat_order_id;
        }
        sq_order_record *order = m_match_order_table.find(req->sq_local_id);
        // 平仓单检查持仓是否够用
        if (req->offset == f_close)
        {
            bool check_posi = true;
            sq_contract_posi_table *tb = m_posi_table.find(req->contract.data());
            if (tb == nullptr)
            {
                log_error("no enough position,sq_local_id={}\n", req->sq_local_id);
                check_posi = false;
            }
            else
            {
                // 检查是否有持仓
                if (req->direction == f_buy)
                {
                    int can_close = tb->m_sq_contract_posi.can_buy_close();
                    if (can_close < req->qty)
                    {
                        // 仓位不够平
                        log_error("no enough position,sq_local_id={}\n", req->sq_local_id);
                        check_posi = false;
                    }
                }
                else
                {
                    int can_close = tb->m_sq_contract_posi.can_sell_close();
                    if (can_close < req->qty)
                    {
                        // 仓位不够平
                        log_error("no enough position,sq_local_id={}\n", req->sq_local_id);
                        check_posi = false;
                    }
                }
            }
            if (!check_posi)
            {
                order->err_code = err_no_enough_posi;
                order->err_msg = "no enough position";
                order->status = status_insert_reject;
                order->is_finish = true;
                add_result(tid_order_state, (char *)order, sizeof(sq_order_record));
                return err_no_enough_posi;
            }
        }
        //更新持仓表
        m_posi_table.on_place_order(req->contract.data(),
        req->price,
        req->qty,
        req->direction,
        req->offset
        );

        std::stringstream ss;
        req->to_string(ss);
        log_debug("add to posi_table,{}\n",ss.str());

        //添加到订单价格表
        auto it = m_contract_order_price_queue.find(req->contract.data());
        if (it == m_contract_order_price_queue.end()) {
            m_contract_order_price_queue[req->contract.data()] = price_level_table();
            it = m_contract_order_price_queue.find(req->contract.data());
        }
        it->second.insert(order->sq_local_id, order);
        

        if (req->status == status_insert_submit)
        {
            order->err_code = 0;
            order->err_msg = "ok";
            order->status = status_accept;
			log_info("send order_rsp sq_local_id={},err_code={},err_msg={}\n",
				order->sq_local_id,
				order->err_code,
				order->err_msg.data());

            add_result(tid_order_state, (char*)order, sizeof(sq_order_record));

        }
		on_order_insert(order);
        return ok;
    }
    void sim_base::_exe_cancel_order(sq_req_cancel_order*action)
    {
        sq_order_record*order = NULL;

        order = m_match_order_table.find(action->sq_local_id);
        if (order == nullptr) {
            sq_order_record record;
            record.sq_local_id = action->sq_local_id;
            record.err_code = err_no_exit_order;
            record.err_msg = "not exit this order";
            record.status=status_cancel_reject;
            add_result(tid_order_state, (char*)&record, sizeof(sq_order_record));
        }
        else {
            if (order->status == status_accept || order->status == status_part_trade||
            order->status==status_cancel_submit) {
                order->status = status_cancel;
                order->cancel_qty=order->qty-order->match_total;
                order->err_code = 0;
                order->err_msg = "";
                order->is_finish = true;
                log_debug( "send cancel rsp, order_id={}\n", order->sq_local_id);

				//持仓管理
                m_posi_table.on_order_state(order->contract.data(),
                order->status,
                order->qty,
                order->cancel_qty,
                order->direction,
                order->offset);
				on_order_cancel(order);
                add_result(tid_order_state, (char*)order, sizeof(sq_order_record));

            }
            else {
                if (order->status == status_all_trade) {
                    log_debug("order {},cancel fail,all traded\n", action->sq_local_id);
                }
                else if (order->status == status_cancel)
                {
                    log_warn("order {},cancel fail,has caneled\n", action->sq_local_id);
                }
                else {
                    log_warn("order {},cancel fail,status={}\n", action->sq_local_id, order->status);
                }

                order->err_code = 1;
                std::string ss=fmt::format("order cancel fail,sq_local_id={},status={}",action->sq_local_id, order->status);
                order->err_msg=ss;
                order->status = status_cancel_reject;
                order->is_finish = true;
				//on_order_cancel(order);
                add_result(tid_order_state, (char*)order, sizeof(sq_order_record));
            }

          
            m_contract_order_price_queue[order->contract.data()].remove(order->direction, order->price, order->sq_local_id);
        }

    }

    void sim_base::match_by_order(sq_order_record *order)
    {
        // 订单触发的成交，会自成交
        order->match_price = order->price;
        order->match_qty = order->qty;
        order->match_total = order->match_qty;
        order->is_finish = true;
        order->status = status_all_trade;
        add_result(tid_order_match, (char *)order, sizeof(sq_order_record));
    }

    void sim_base::add_result(int msg_type, char*msg, int size)
    {
        if(msg_type==tid_order_state){
            sq_order_record*order = (sq_order_record*)msg;
            sq_order_state_ntf ntf;
            ntf.sq_local_id=order->sq_local_id;
            ntf.user_id=order->user_id;
            ntf.status=order->status;
            ntf.sys_id=order->sys_id;
			ntf.contract = order->contract;
			ntf.direction = order->direction;
			ntf.offset = order->offset;
            if (ntf.status == order_status_t::status_cancel)
            {
                ntf.cancel_qty = order->cancel_qty;
            }
            else{
                ntf.cancel_qty=0;
            }
            m_posi_table.on_order_state(order->contract.data(),
            ntf.status,
            order->qty,
            order->cancel_qty,
            ntf.direction,
            ntf.offset);
            m_call_back(tid_order_state, (char*)&ntf, sizeof(ntf),m_call_back_param,m_id);

        }
		else if (msg_type == tid_order_match) {
			sq_order_record*order = (sq_order_record*)msg;
            m_match_id++;
            m_posi_table.on_order_match(order->contract.data(),
            order->match_price,
            order->match_qty,
            order->direction,
            order->offset);

            sq_order_match_ntf ntf;
            ntf.sq_local_id=order->sq_local_id;
            ntf.user_id=order->user_id;
            ntf.match_id=m_match_id;
            ntf.offer_id="";
			ntf.contract = order->contract;
            ntf.match_price=order->match_price;
            ntf.match_qty=order->match_qty;
            ntf.direction=order->direction;
            ntf.offset=order->offset;
            sq::date_time now=date_time::now();
            ntf.date=now.format("YYYYMMDD");
            ntf.time=now.format("HH:mm:SS.sss");
			 m_call_back(tid_order_match, (char*)&ntf, sizeof(ntf),m_call_back_param,m_id);
		}
        else{
            if(m_call_back){
                m_call_back(msg_type, msg, size,m_call_back_param,m_id);
            }
               
        }
		
    }
    //收到查询请求
	void sim_base::on_query(int tid, char*select, int size)
	{
#if 0
		json_conf conf;
		string s(select);
		conf.load_str(select);
		string table = conf.get_string("table");
		//int req_id = gen_request_id();
		//{"table":"contract",contract:"","trade_type":"option|stock|futrue"}

		//合约查询
		if (table == "contract")
		{
			for (auto s : m_sim_data->m_tbl_contract) {
                add_result(tid_contract_info, (char*)&s, sizeof(sq_contract));
            }
		}
		//{"table":"order",contract:"","trade_type":"option|stock|futrue"}
		else if (table == "order") {
        
		}
		else if (table == "match") {

		}
		else if (table == "position") {
            auto it=m_position_table->m_positions.begin();
            for (;it!=m_position_table->m_positions.end();++it) {
                
                if(it->second.buy_qty>0 || it->second.last_buy_qty>0)
                {
                    sq_position s;
                    s.contract=it->first;
                    s.comb_position=0;
                    s.avg_price=it->second.buy_avg_price;
                    s.direction=f_buy;
                    s.is_last=false;
                    s.market="";
                    s.today_posi=it->second.buy_qty;
                    s.position=it->second.buy_qty;
                    s.avg_price=it->second.buy_avg_price;
                    s.req_id=0;
                    s.variety="";
                    s.yestoday_posi=it->second.last_buy_qty;
                    add_result(tid_position_rsp, (char*)&s, sizeof(sq_position));
                }
                if (it->second.sell_qty>0 || it->second.last_sell_qty>0){
                    sq_position s;
                    s.contract=it->first;
                    s.comb_position=0;
                    s.avg_price=it->second.sell_avg_price;
                    s.direction=f_sell;
                    s.is_last=false;
                    s.market="";
                    s.today_posi=it->second.sell_qty;
                    s.position=it->second.sell_qty;
                    s.avg_price=it->second.sell_avg_price;
                    s.req_id=0;
                    s.variety="";
                    s.yestoday_posi=it->second.last_sell_qty;
                    add_result(tid_position_rsp, (char*)&s, sizeof(sq_position));
                }
            }
           
		}
		else if (table == "account") {
			sq_fund_info info;
			memset(&info, 0, sizeof(info));
			info.tid = tid_query_money_rsp;
			info.date = m_trade_date;
			info.user_id ="sim";

			info.today_profit =0;
			info.balance = 1;
			info.margin_exchage = 2;
			info.margin_used = 3;
			info.withdraw = 4;
			info.deposit = 5;
			info.fee = 6;
            info.avali_money = m_sim_data->m_tbl_money.avali_money;
			info.pre_balance = 8;
			info.cash_in = 9;
			info.pre_margin_exchage = 10;

			add_result(tid_query_money_rsp, (char*)&info, sizeof(sq_fund_info));

		}
	#endif
    }
	void sim_base::query(const char*select)
	{
		req_msg*msg = m_input_rb->back();
		while (msg == nullptr)
		{
			// sq_util::sleep_ms(1);
			std::this_thread::yield();
			msg = m_input_rb->back();
		}
		msg->msg_type = tid_query_req;
		msg->msg_size = strlen(select);
		strcpy(msg->msg, select);
		m_input_rb->push();
	

	}
}