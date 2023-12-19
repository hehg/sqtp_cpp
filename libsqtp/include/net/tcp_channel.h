/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once
#include "error/sq_err.h"
#include "event_handler.h"
#include "net/reactor.h"
#include "net_socket.h"
#include "net_cache.hpp"
#include  <mutex>

namespace sq
{
	class tcp_channel;
	using on_tcp_channel_connected_fun_t 	= std::function<void(tcp_channel*channel)>;
	using on_tcp_channel_disconnected_fun_t = std::function<void(tcp_channel*channel)>;
	using on_tcp_channel_msg_fun_t 			= std::function<int(tcp_channel*channel,void*msg,int size)>;
	/**
	 * 代表 一个 socket tcp连接 不管是客户端，还是服务端
	*/
  
	class tcp_channel :public event_handler
	{
		typedef event_handler base;
	public:
		tcp_channel();
		tcp_channel(event_reactor*r);
		tcp_channel(event_reactor*r, fd_t fd);
		~tcp_channel();

        fd_t get_fd() override { return m_fd; };
		void close()  override;
		/**
		 * @brief 写入数据到 socket，直到全部写入为止
		*/
		int send_data(const char*buf, int len);
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