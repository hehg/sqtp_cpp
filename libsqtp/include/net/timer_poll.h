#pragma once
#include "sq_define.h"
namespace sq
{
	class event_handler;

	struct time_node {
		int time_id;
		event_handler* handler;
		uint64_t expired;
		uint64_t interval;

		bool check_time_out(uint64_t time);
	};

	class timer_poll 
	{
	public:
		timer_poll() :m_timer_id(0){}
		~timer_poll(){ m_timer_list.clear(); }

		int add_timer(event_handler*handler,int ms);
		void del_timer(event_handler*handler);

		void loop_once();

		std::unordered_map<int,time_node> m_timer_list;
		int m_timer_id;
	};
}