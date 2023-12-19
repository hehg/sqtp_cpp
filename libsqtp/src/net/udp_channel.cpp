#include "net/udp_channel.h"
#include "net/reactor.h"
#include "net/net_socket.h"
#include <sstream>
namespace sq
{
	udp_channel::udp_channel()
	{
		m_fd=open_udp_socket();
	}
	udp_channel::udp_channel(event_reactor*r) :event_handler(r)
	{
		m_fd=open_udp_socket();
		r->post_msg(EVENT_MSG_ADD_HANDLER, this);
	}
	udp_channel::udp_channel(event_reactor*r,fd_t fd):
	m_fd(fd)
	{

	}
	udp_channel::~udp_channel(){
		if (m_fd != invalid_fd) {
			close_socket(m_fd);
			m_fd = invalid_fd;
		}
		
	}

	void udp_channel::on_read()
	{
		char buf[4096];
		struct msghdr msg;
		msg.msg_namelen = sizeof(m_dest_addr);
		msg.msg_name = &m_dest_addr;
		msg.msg_iovlen=1;
		iovec iov;
		iov.iov_base=buf;
		iov.iov_len=sizeof(buf);
		msg.msg_iov=&iov;
		msg.msg_control=NULL;
		msg.msg_controllen=0;

		int len=recvmsg(m_fd, &msg, 0);
		if (len > 0)
		{
			on_message(buf, len,nullptr);
		}
	}
	
	int udp_channel::send_data(const char *buf, int len, const char *ip, int port)
	{
		return net_send_udp_msg(m_fd, buf, len, ip, port);
	}
	int udp_channel::on_message(void *msg, int size, void *from)
	{
		if (m_msg_handler)
		{
			return m_msg_handler(from,msg, size);
		}
		else
		{
			assert(false);
		}
		return 0;
	};
	void udp_channel::get_peer_addr(string &address)
	{
		using namespace std;

		if (m_fd!=invalid_fd)
		{
			string ip;
			int port;
			net_sockaddr_to_string(&m_from, ip, port);
			stringstream ss;
			ss << ip << ":" << port;
			address = ss.str();
		}
	}
	void udp_channel::bind(const char*ip,int port)
	{
		bind_socket(m_fd,ip,port);
	}
}
