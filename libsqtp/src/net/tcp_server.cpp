#include "net/tcp_server.h"
#include  "error/sq_err.h"
namespace sq
{
	int tcp_server::open(const  char* address)
	{
		m_fd = open_tcp_socket();
		unblock_socket(m_fd);
		
		net_address addr;
		addr.parse_ipv4(address);

		if (bind_socket(m_fd, addr.m_ip, addr.m_port) < 0)
		{
			perror("bind");
			throw "hx_net_listener:bind";
		}

		if (::listen(m_fd, 1024) < 0)
		{
			perror("listen");
			throw "tcp_server:listen";
		}
		event_handler::open();
		m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, this);
		
		return ok;
	}
	
	
	void tcp_server::on_read()
	{
		struct sockaddr_in it;
		socklen_t nameLen = sizeof(it);
		fd_t new_socket = ::accept(m_fd, (struct sockaddr *)&it, &nameLen);
		if (new_socket == invalid_fd)
		{
			return;
		}

		on_session_connected(new_socket);
		
	}
}