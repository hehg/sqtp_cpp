/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once
#include "sq_define.h"
#include "event_handler.h"
#include "../net/net_address.h"
#include "reactor.h"
#include "net_socket.h"
#include "log/sq_logger.h"
#include "net/tcp_channel.h"
namespace sq
{
	
	class event_reactor;

	
	/**
	 * tcp监听服务
	*/
	class tcp_server:public  event_handler
	{
	public:
		tcp_server(on_tcp_channel_msg_fun_t on_msg,
				   event_reactor *r = nullptr) : event_handler(r)
		{
			m_on_msg_callback = on_msg;
		};
		virtual ~tcp_server(){};
		/**
		 * @brief 打开监听地址 ex:tcp://127.0.0.1:1234
		*/
		int open(const  char* address);

		//void close();

		void run();

		fd_t get_fd(){return m_fd;}

		tcp_server& reg_session_connected(on_tcp_channel_connected_fun_t f){
			m_session_connected_callback=f;
			return *this;
		}
		tcp_server& reg_session_disconnected(on_tcp_channel_disconnected_fun_t f){
			m_session_disconnected_callback=f;
			return *this;
		}
	protected:
		void on_session_close(void *handler);
		int on_session_message(void*handler,void *msg, int size);
		void on_read();
		fd_t m_fd = invalid_fd;
		on_tcp_channel_msg_fun_t            m_on_msg_callback			    =nullptr;
		on_tcp_channel_connected_fun_t 		m_session_connected_callback	=nullptr;
		on_tcp_channel_disconnected_fun_t 	m_session_disconnected_callback	=nullptr;
	};

}