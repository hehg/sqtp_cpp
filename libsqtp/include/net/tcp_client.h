/**
Copyright (c) 2003-2015 
@brief 字节数组
@author 
*/
#pragma once
#include "sq_define.h"

#include "net/event_handler.h"
#include "net/tcp_channel.h"
namespace sq
{
	class event_reactor;
	class tcp_channel;
	/**
	 * 基于 tcp 协议的客户端支持功能如下：
	 * -  支持多个通道
	 * @note 非线程安全
	*/
	class tcp_client 
	{
	public :
		tcp_client(
			on_tcp_channel_msg_fun_t on_msg,
			on_tcp_channel_connected_fun_t on_connected = nullptr,
			on_tcp_channel_disconnected_fun_t on_disconnected = nullptr,
			event_reactor *r = nullptr);

		~tcp_client();

		int send(const char*data,int size);
		/**
		 * 增加一个 udp 通道
		 * @param server_address :udp://127.0.0.1:4502
		 * @return 
		*/
		int add_channel(const char* server_address);
		
		void close();
	private:
	 int on_channel_message(void*channel,void*package, int size);
	 void on_channel_connected(void*ch);
	 void on_channel_disconnected(void*ch);
	protected:
		event_reactor* 						m_eg			=nullptr;
		on_tcp_channel_msg_fun_t  			m_on_msg		=nullptr;
		on_tcp_channel_connected_fun_t 		m_on_connected	=nullptr;
		on_tcp_channel_disconnected_fun_t 	m_on_disconnected=nullptr;
		std::map<int,tcp_channel*>			m_channels; //key=socket fd
	};
}