#include "net/udp_handler.h"
#include "net/reactor.h"
#include <sstream>
namespace sq
{
	udp_handler::udp_handler(event_reactor*r) :event_handler(r)
	{
		r->post_msg(EVENT_MSG_ADD_HANDLER, this);
	}
	udp_handler::~udp_handler(){
		if(m_udp_socket){
			delete m_udp_socket;
		}
	}

	void udp_handler::open(const char *url)
	{
		m_udp_socket = new udp_socket(url, false);
	}
	void udp_handler::open_as_listener(const char *address)
	{
		m_udp_socket = new udp_socket(address, false);
	}
	void udp_handler::on_read()
	{
		char buf[4096];
		int len = m_udp_socket->recv_from(buf, sizeof(buf));
		if (len > 0)
		{
			on_message(buf, len,&m_udp_socket->m_from);
		}
	}
	bool udp_handler::send_data(const char*buf, int len)
	{
		int send_len=m_udp_socket->send_to_dest(buf, len);
		return send_len == len;
	}
	int udp_handler::send_data(const char *buf, int len, const char *ip, int port)
	{
		return net_send_udp_msg(m_udp_socket->m_socket_fd, buf, len, ip, port);
	}
	int udp_handler::on_message(void *msg, int size, void *from)
	{
		if (m_msg_handler)
		{
			return m_msg_handler(msg, size, from);
		}
		else
		{
			assert(false);
		}
		return 0;
	};
	void udp_handler::get_peer_addr(string &address)
	{
		using namespace std;

		if (m_udp_socket)
		{
			string ip;
			int port;
			net_sockaddr_to_string(&m_udp_socket->m_from, ip, port);
			stringstream ss;
			ss << ip << ":" << port;
			address = ss.str();
		}
	}
}
