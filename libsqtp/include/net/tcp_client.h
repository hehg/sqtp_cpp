/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once
#include "sq_define.h"
#include "../net/net_address.h"
#include "net/event_handler.h"
#include "net/tcp_handler.h"
namespace sq
{
	class event_reactor;
	class tcp_handler;
	class tcp_client :public tcp_handler
	{
	public :
		tcp_client(event_reactor*r=nullptr);
		~tcp_client();

		int open(const char* server_address);
		
		void close();

	protected:
		net_address m_server_address;
		event_reactor*m_reactor;
		
	};
}