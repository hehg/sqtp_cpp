#include "net/timer_poll.h"
#include "time/date_time.h"
#include "net/event_handler.h"
namespace sq
{
	bool time_node::check_time_out(uint64_t time)
	{
		if (time >= expired)
		{
			expired += interval;
			handler->on_read();
			return true;
		}
		return false;
	}
	int timer_poll::add_timer(event_handler*handler, int ms)
	{
		assert(ms>0);
		time_node node;
		node.handler = handler;
		node.time_id = m_timer_id++;
		node.interval = ms;
		node.expired = date_time::get_now_ms() + node.interval;
		m_timer_list[node.time_id] = node;
		return m_timer_id;
	}
	void timer_poll::del_timer(event_handler*h)
	{
		for (auto it : m_timer_list) 
		{
			if (it.second.handler == h) 
			{
				m_timer_list.erase(it.second.time_id);
				break;
			}
		}
	}

	void timer_poll::loop_once()
	{
		uint64_t time=date_time::get_now_ms();
		auto it=m_timer_list.begin();
		while (it != m_timer_list.end())
		{
			it->second.check_time_out(time);
			it++;
		}
		
	}

}