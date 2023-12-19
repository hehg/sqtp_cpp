#include "net/tcp_server.h"
#include  "error/sq_err.h"
#include "net/tcp_channel.h"
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
	void tcp_server::run()
	{
		m_reactor->loop();
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
		tcp_channel* new_handler = new tcp_channel(m_reactor,new_socket);
		new_handler->reg_on_close(std::bind(&tcp_server::on_session_close, this, std::placeholders::_1));
		new_handler->reg_on_msg(std::bind(&tcp_server::on_session_message, this, std::placeholders::_1,
                                std::placeholders::_2, std::placeholders::_3));
        if(m_session_connected_callback)
		{
			m_session_connected_callback(new_handler);
		}
		
		
	}
	void tcp_server::on_session_close(void *handler)
    {
        //m_sessions.erase(h->m_session_id);
        if(m_session_disconnected_callback){
            m_session_disconnected_callback((tcp_channel*)handler);
        }
       
       // h->m_state=ws_channel::ws_state_closed;
    }
	int tcp_server::on_session_message(void*handler,void *msg, int size)
	{
		tcp_channel*session=(tcp_channel*)handler;
		if(m_on_msg_callback)
		{
			return m_on_msg_callback(session,msg,size);
		}
		return size;
	}
}