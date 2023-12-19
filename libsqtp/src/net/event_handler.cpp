#include "net/event_handler.h"
#include "net/reactor.h"

namespace sq
{
	event_handler::event_handler()
	{
		m_reactor = get_default_reactor();
	}
	event_handler::event_handler(event_reactor *r)
	{
		if (r == nullptr)
		{
			m_reactor = get_default_reactor();
		}
		else
		{
			m_reactor = r;
		}
	};
	int event_handler::open()
	{
		state(handle_state::handle_opening);
		return 0;
	}
	void event_handler::on_open()
	{
		state(handle_state::handle_opened);
		if (m_open_handler)
		{
			m_open_handler(this);
		}
	}
	void event_handler::on_close()
	{
		state(handle_state::handle_closed);
		if (m_close_handler)
		{
			m_close_handler(this);
		}

		m_reactor = nullptr;
	}
	void event_handler::close()
	{
		std::lock_guard<std::mutex> lk(m_lock);
		if (state() == handle_state::handle_opened)
		{
			if (m_reactor)
				m_reactor->post_msg(EVENT_MSG_DEL_HANDLER, this);
			state(handle_state::handle_closing);
		}
	}
	int event_handler::on_message(void *msg, int size, void *from)
	{
		if (m_msg_handler)
		{
			return m_msg_handler(from,msg, size);
		}
		else
		{
			assert(false);
		}
		return 0;
	};
}