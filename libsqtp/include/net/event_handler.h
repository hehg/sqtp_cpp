#pragma once
#include<functional>
#include "sq_define.h"
#include "sq_types.h"
#include "sync/sq_spin_lock.h"
#include "reactor.h"
using namespace std;

using on_message_fun_t 	= std::function<int(void*handler,void*package, int size)>;
using on_open_fun_t 	= std::function<void(void*handler)>;
using on_close_fun_t 	= std::function<void(void*handler)>;


namespace sq
{
	enum class handle_state:uint8_t{
		handle_init=0,
		handle_opened,
		handle_opening,
		handle_closing,
		handle_closed,
	};
	class event_reactor;
	/**
	 * 事件处理对象，负责与事件相关的处理
	 * 事件包括：网络IO，定时器 等等
	*/
	class event_handler
	{
	public:
		event_handler();
		event_handler(event_reactor *r);
		virtual ~event_handler(){};
		
		void  	set_id(int32_t id){m_id=id;}
		int32_t get_id(){return m_id;}

		virtual fd_t get_fd()=0;
		
		virtual int  open();
		virtual void close();
		/** 调用时机：对象加入引擎
		 */
		virtual void on_open();
		/**
		 * 调用时机： 有数据
		*/
		virtual void on_read() {};
		/** 调用时机：关闭对象，从引擎删除，对象即将被销毁
		 */
		virtual void on_close();
		
		//事件发生时的回调处理函数
		event_handler& reg_on_msg(on_message_fun_t t) { m_msg_handler = t; return *this;}
		event_handler& reg_on_open(on_open_fun_t t) { m_open_handler = t; return *this;}
		event_handler& reg_on_close(on_close_fun_t t) { m_close_handler = t; return *this;}

		virtual int on_message(void*msg, int size,void* from);

		event_reactor*m_reactor=nullptr;
		on_message_fun_t m_msg_handler{ nullptr };
		on_open_fun_t  m_open_handler{ nullptr };
		on_close_fun_t  m_close_handler{ nullptr };
		
		void state(handle_state val) {
			m_state = val;
		}
		handle_state state() {
			return m_state;
		}
		void acquire_lock() { m_lock.lock(); };
		void release_lock() { m_lock.unlock(); };
		

	protected:
		std::mutex  m_lock;
		handle_state m_state=handle_state::handle_init;
		int m_id=0;
	};
}