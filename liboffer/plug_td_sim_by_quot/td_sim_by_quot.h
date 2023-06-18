#pragma once
#include "sim_base.h"
#include <fstream>
#include <thread>
#include "sq_quot_file.h"
using namespace sq;

namespace sq_plug{
    
    /** 基于行情的模拟撮合
    */
	class td_sim_by_quot :public sim_base
	{
	public:
		td_sim_by_quot();
		~td_sim_by_quot();
		
		int open();
        int close();
		int put(uint16_t tid, char* data,uint16_t size);

		void run();
	private:
		sq_quot* read_quot();
        bool _exe_match_by_quot(sq_order_record*order,sq_quot*quot,double tick_avg_price);
		void on_order_insert(sq_order_record*order)override;

	private:
		void match(sq_quot *quot);
		std::thread 		*m_eg = nullptr;
		bool   				m_run_flag=false;

		sq_quot_file_reader m_quot_reader;
		bool 				m_quot_file_have_header;
		string 				m_quot_file_header;

		sq_quot 			m_cur_quot;
		int 				m_multi = 1;
		string 				m_quot_file_path;
		sq_quot_table 		m_last_tick_quot; // 上一笔行情
		int 				read_quot_wait_time = 0;
	};
}

