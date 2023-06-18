
#pragma once
#include "sq_define.h"
#include "sq_types.h"
#include "net/timer_poll.h"

namespace sq{
	class event_handler;
	class timer_handler;

#define EVENT_MSG_ADD_HANDLER 1
#define EVENT_MSG_DEL_HANDLER 2
#define EVENT_MSG_ADD_TIMER 3
#define EVENT_MSG_DEL_TIMER 4
#define EVENT_MSG_DESTROYME 5

	struct event_msg
	{
		event_handler* hanlder;
		int   msg_id;
	};
	/** 事件引擎
	*/
	class event_reactor
	{
	public:
		event_reactor(){};
		~event_reactor(){};
		void post_msg(int tid, event_handler*h);

		virtual void add_timer(event_handler*h, int interval_ms);
		virtual void del_timer(event_handler*h);

		virtual void loop_once(int ms = 1) {};
		void loop();
	protected:
		
		std::vector<event_msg> m_cmd_list;
		std::mutex m_mutex;
	};
#if SUPPORT_SELECT==1

	class selecter :public event_reactor
	{
	public:
		selecter();
		~selecter();

		virtual void loop_once(int ms = 1);

		void add_timer(event_handler*h, int interval_ms);
		void del_timer(event_handler*h);
		void handle_cmd();

		list<event_handler*> m_event_list;
		fd_set  m_reader;
		fd_set  m_except;
		
		timer_poll  m_timer_poller;
	};
	using  sq_reactor = selecter;
#endif

#if SUPPORT_EPOLL==1
	class epoller :public event_reactor
	{
	public:
		epoller();
		~epoller();

		virtual void loop_once(int ms = 1);
		void handle_cmd();
		int32_t m_epoll_fd;
		struct epoll_event m_event_list[1024];
	};
	using  sq_reactor = epoller;
#endif


    inline sq_reactor*  get_default_reactor() { static sq_reactor s; return &s; }

	void reactor_run_forever(sq_reactor*r=nullptr);
}