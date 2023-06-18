#pragma once

#include "container/ring_buffer.h"
#include "sq_order_table.h"
#include "sq_price_level_table.h"
#include "sq_type_define.h"
#include "sq_quot_table.h"
#include "sq_posi_table.h"
#include "plug_base.h"
using namespace sq;
namespace sq_plug
{
    const price_t  market_price_up = 10000000000;
    const price_t  market_price_down = 0;
    struct rsp_msg {
        int msg_type;
        int msg_size;
        char msg[512];
    };
    struct req_msg {
        int msg_type;
        int msg_size;
        char msg[512];
    };
    enum {
        match_price_otherside = 0, //对手
        match_price_handup = 1, //挂单价
    };

   
    class sim_base: public plug_base
    {
    public:
        sim_base();
        ~sim_base();
       
        int place_order(sq_req_order*order);
        int cancel_order(sq_req_cancel_order*action);

        //订单插入的撮合队列
        virtual void on_order_insert(sq_order_record*order) {}
		virtual void on_order_cancel(sq_order_record*order) {}

		void query(const char*select);
		void on_query(int tid, char*select, int size);
        
protected:
        int _exe_place_order(sq_order_record*order);
        virtual void _exe_cancel_order(sq_req_cancel_order*action);
		void match_by_order(sq_order_record*order);
        void add_result(int msg_type, char*msg, int size);
        void handle_package();
public:
        int     m_order_id=1;
        int     m_match_id=1;
       
        //所有合约的队列(按价格排序)
        std::unordered_map<string, price_level_table> m_contract_order_price_queue; //价位表
        sq_order_table	    m_match_order_table;     ///撮合引擎使用的订单表
        sq_quot_table   m_quot_table;
        sq_posi_table   m_posi_table;

        ring_buffer<req_msg>* m_input_rb;
    };
}