/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once
#include "error/sq_err.h"
#include "event_handler.h"
#include "net_socket.h"
#include "net_cache.hpp"
#include  <mutex>

namespace sq
{
	/**
	 * 代表 一个 socket tcp连接 不管是客户端，还是服务端
	*/
  
	class tcp_handler :public event_handler
	{
		typedef event_handler base;
	public:
		tcp_handler();
		tcp_handler(event_reactor*r);
		tcp_handler(event_reactor*r, fd_t fd);
		~tcp_handler();

        fd_t get_fd() override { return m_fd; };
		void close()  override;
		bool send_data(const char*buf, int len);
        void get_peer_addr(string& peer_addr);
	protected:
		void create();
		void on_read() override;
		void dispatch();
		fd_t m_fd= invalid_fd;
		net_cache<> m_net_cahce;
		std::mutex m_write_mtx;
		
	};
}