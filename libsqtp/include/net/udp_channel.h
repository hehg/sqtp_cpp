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
	class udp_channel;
	using on_udp_channel_connected_fun_t = std::function<void(udp_channel *channel)>;
	using on_udp_channel_disconnected_fun_t = std::function<void(udp_channel *channel)>;
	using on_udp_channel_msg_fun_t = std::function<int(udp_channel *channel, void *msg, int size)>;
	/**
	 * @brief udp 通道，基于 udp 协议的数据发送，接收
	 */
	class udp_channel : public event_handler
	{
	public:
		udp_channel();
		explicit udp_channel(event_reactor *r);
		udp_channel(event_reactor *r, fd_t fd);
		~udp_channel();

		int send_data(const char *buf, int len, const char *ip, int port);
		int on_message(void *msg, int size, void *from) override;
		void get_peer_addr(string &);
		fd_t get_fd() override { return m_fd; }

		void bind(const char *ip, int port);

	protected:
		virtual void on_read()override;

		fd_t m_fd = invalid_fd;
		struct sockaddr_in m_dest_addr;
		sockaddr m_from;
	};

}
