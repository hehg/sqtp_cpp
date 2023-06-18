#include "net/reactor.h"
#include "net/event_handler.h"
#include "sq_define.h"
#include "sq_types.h"
#include "net/net_socket.h"
#include "sq_macro.h"
#include "base/sq_util.h"
#include "net/timer_handler.h"
namespace sq
{
	void event_reactor::post_msg(int tid, event_handler*h)
	{
		std::lock_guard<std::mutex>lk(m_mutex);
		event_msg t;
		t.hanlder = h;
		t.msg_id = tid;
		m_cmd_list.push_back(t);
	}
	
	void event_reactor::add_timer(event_handler*h, int interval_ms)
	{

	}
	void event_reactor::del_timer(event_handler*h)
	{

	}

	void event_reactor::loop()
	{
		while (true)
		{
			loop_once();
		}
	}
	//=====================selecter====================
#if SUPPORT_SELECT==1
	selecter::selecter()
	{
		FD_ZERO(&m_reader);
		FD_ZERO(&m_except);

	}
	selecter::~selecter()
	{
	}
	void selecter::add_timer(event_handler*h, int interval_ms)
	{
		m_timer_poller.add_timer(h, interval_ms);
	}
	void selecter::del_timer(event_handler*h)
	{
		m_timer_poller.del_timer(h);
	}
	void selecter::loop_once(int ms)
	{
		list<event_handler*>::iterator it = m_event_list.begin();
		fd_t max_fd = 0;
		FD_ZERO(&m_reader);
		FD_ZERO(&m_except);
		for (; it != m_event_list.end(); ++it)
		{
			fd_t id = (*it)->get_fd();
			FD_SET(id, &m_reader);
			FD_SET(id, &m_except);
#ifndef WINDOWS
			if (id > max_fd) {
				max_fd = id;
			}
#endif
		}

		if (m_event_list.empty())
		{
			sq_util::sleep_ms(10);
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
		else
		{
			int r = 0;
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 1000*(ms%1000);

			r = select(max_fd+1, &m_reader, NULL, &m_except, &timeout);
			if (r>0)
			{
				list<event_handler*>::iterator it = m_event_list.begin();
				for (; it != m_event_list.end(); ++it)
				{
					event_handler*ev = *it;
					fd_t id = ev->get_fd();
					if (FD_ISSET(id, &m_reader))
					{
						ev->on_read();
					}
					else if (FD_ISSET(id, &m_except))
					{
						printf("on_except\n");
					}

				}
			}
		}
		
		m_timer_poller.loop_once();
		handle_cmd();
	}
	void  selecter::handle_cmd()
	{
		std::vector<event_msg> cmd_tmp;
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_cmd_list.swap(cmd_tmp);
			m_cmd_list.clear();
		}

		if (!cmd_tmp.empty())
		{
			for (size_t i = 0; i < cmd_tmp.size(); i++)
			{
				event_msg*t = &cmd_tmp[i];

				if (t->msg_id == EVENT_MSG_ADD_HANDLER) {
					event_handler*h = t->hanlder;
					fd_t id = h->get_fd();
					m_event_list.push_back(h);
					h->on_open();
					
				}
				else if (t->msg_id == EVENT_MSG_DEL_HANDLER) {
					event_handler*h = t->hanlder;
					
					list<event_handler*>::iterator it =
						std::find(m_event_list.begin(), m_event_list.end(), h);
					if (it != m_event_list.end())
					{
						//FD_CLR((*it)->get_fd(), &m_reader);
						event_handler*h = (*it);
						m_event_list.erase(it);
						//销毁
						post_msg(EVENT_MSG_DESTROYME, h);
						
						
					}

				}
				else if (t->msg_id == EVENT_MSG_DESTROYME) {
					event_handler*h = t->hanlder;
					
					t->hanlder->on_close();

					delete t->hanlder;
					t->hanlder = nullptr;
					
				}
				// else if (t->msg_id == EVENT_MSG_CONNECTED) {
				// 	t->hanlder->on_open();
				// }
			}
		}
	}
#endif
	//=========================================epoller===========
#if SUPPORT_EPOLL==1
	epoller::epoller()
	{
		m_epoll_fd = epoll_create(1024);
		assert(m_epoll_fd >0);
	}
	epoller::~epoller()
	{
		if (m_epoll_fd > 0)
			close(m_epoll_fd);
	}
	void epoller::loop_once(int timeout)
	{
		int count = epoll_wait(m_epoll_fd, &m_event_list[0], 1024, timeout);
		for (int i = 0; i < count; ++i)
		{
			event_handler*h = static_cast<event_handler*>(m_event_list[i].data.ptr);
			h->on_read();
		}

		handle_cmd();

	}
	void  epoller::handle_cmd()
	{
		std::vector<event_msg> cmd_tmp;
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_cmd_list.swap(cmd_tmp);
			m_cmd_list.clear();
		}

		if (!cmd_tmp.empty())
		{
			for (size_t i = 0; i < cmd_tmp.size(); i++)
			{
				event_msg*t = &cmd_tmp[i];

				if (t->msg_id == EVENT_MSG_ADD_HANDLER) {
					event_handler*h = t->hanlder;
					fd_t id = h->get_fd();
					struct epoll_event ev { EPOLLIN , { h } };
					epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, h->get_fd(), &ev);
					h->on_open();
					//printf("add handler \n");
				}
				else if (t->msg_id == EVENT_MSG_DEL_HANDLER) {
					event_handler*h = t->hanlder;
					epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, h->get_fd(), nullptr);
printf("remove handler \n");
post_msg(EVENT_MSG_DESTROYME, h);
				}
				else if (t->msg_id == EVENT_MSG_DESTROYME) {
					event_handler*h = t->hanlder;
					h->acquire_lock();
					t->hanlder->on_close();
					h->release_lock();
					delete t->hanlder;
					t->hanlder = nullptr;
printf("destroy hanlder \n");
				}
				
				// else if (t->msg_id == EVENT_MSG_CONNECTED) {
				// 	t->hanlder->on_open();
				// }
			}
			cmd_tmp.clear();
	}
	}
#endif

	void reactor_run_forever(sq_reactor *r)
	{
		if (r == nullptr)
		{
			r = get_default_reactor();
		}
		if (r)
		{
			while (true)
			{
				r->loop();
			}
		}
	}
}
