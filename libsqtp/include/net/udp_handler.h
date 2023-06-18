/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once
#include "event_handler.h"
#include "../net/net_socket.h"
namespace sq
{
	/** 接收udp 数据
	*/
	class udp_handler :public event_handler
	{
    public:
        
        udp_handler(event_reactor*r);
		~udp_handler();
		fd_t get_fd() { return m_udp_socket->m_socket_fd; }
        void open(const char*url);
		void open_as_listener(const char*address);
		
        bool send_data(const char*buf, int len);
		int send_data(const char*buf, int len,const char*ip,int port);
		virtual int on_message(void*msg, int size,void*from);
		void get_peer_addr(string&);
	protected:
		virtual void on_read();
		udp_socket* m_udp_socket=nullptr;
        
	};

    
}
