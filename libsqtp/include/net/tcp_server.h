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
namespace sq
{
	

	class event_reactor;
	class tcp_handler;
	/**
	 * tcp监听服务
	*/
	class tcp_server:public  event_handler
	{
	public:
		tcp_server(){}
		tcp_server(event_reactor*r):event_handler(r){  
		};
		virtual ~tcp_server(){};
		int open(const  char* address);
		void close();

		//连接建立之后此函数被回调
		virtual void on_session_connected(fd_t socket)=0;
		fd_t get_fd(){return m_fd;}

		tcp_server& reg_session_connected(on_session_connected_fun_t f){
			m_session_connected_callback=f;
			return *this;
		}
		tcp_server& reg_session_disconnected(on_session_disconnected_fun_t f){
			m_session_disconnected_callback=f;
			return *this;
		}
	protected:
		void on_read();
		fd_t m_fd= invalid_fd;
		on_session_connected_fun_t m_session_connected_callback=nullptr;
		on_session_disconnected_fun_t m_session_disconnected_callback=nullptr;
	};

}