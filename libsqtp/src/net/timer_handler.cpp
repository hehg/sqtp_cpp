#include "net/timer_handler.h"
#include "net/net_socket.h"
#include "net/reactor.h"
namespace sq
{
	timer_handler::timer_handler(event_reactor*r) :event_handler(r)
	{
#if SUPPORT_TIMER_FD==1
		m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
		if (m_fd == -1) {
			assert(errno == ENFILE || errno == EMFILE);
		}
#endif
	}
	timer_handler::~timer_handler()
	{

	}

	void timer_handler::open(int32_t id,int64_t interval_ms)
	{
        m_id = id;

#if SUPPORT_SELECT==1
		m_reactor->add_timer(this, interval_ms);
#else
        struct itimerspec new_time;
		
		new_time.it_interval.tv_sec = 1;
		new_time.it_interval.tv_nsec = 0;
		new_time.it_value.tv_sec = interval_ms/1000;
		new_time.it_value.tv_nsec = (interval_ms % 1000) * 1000 * 1000;

        //相对定时器
		timerfd_settime(get_fd(), 0, &new_time, nullptr);
        m_reactor->post_msg(EVENT_MSG_ADD_HANDLER,this);
#endif
       
	}
	void timer_handler::close()
	{

#if SUPPORT_SELECT
		m_reactor->del_timer(this);
#else
		struct itimerspec new_time;
		new_time.it_interval.tv_sec = 0;
		new_time.it_interval.tv_nsec = 0;
		new_time.it_value.tv_sec = 0;
		new_time.it_value.tv_nsec = 0;

		timerfd_settime(get_fd(), 0, &new_time, nullptr);
        m_reactor->post_msg(EVENT_MSG_DEL_HANDLER,this);
#endif
		
	}
	void timer_handler::on_read()
	{ 
#ifdef WINDOWS
		
#else
		uint64_t val;
		int ret = read(m_fd, &val, sizeof(uint64_t));
#endif
		
		on_timer(m_id);
	}
	void timer_handler::on_close()
	{
#if SUPPORT_TIMER_FD==1
		::close(m_fd);
#endif
	}
}
