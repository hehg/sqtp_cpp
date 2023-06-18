#include "net/tcp_client.h"
#include "net/reactor.h"
#include "sq_macro.h"
#include "net/tcp_handler.h"
namespace sq
{

	tcp_client::tcp_client(event_reactor *r)
	{
		if (r)
		{
			m_reactor = r;
		}
		else{
			m_reactor=get_default_reactor();
		}
	}

	tcp_client::~tcp_client()
	{
      
	}
	void tcp_client::close()
	{

	}

	int tcp_client::open(const char *server_address)
	{
		create();

		m_server_address.parse_ipv4(server_address);
		if (m_server_address.m_port == 0)
			throw("Invalid port");

		int ret = net_connect(m_fd, m_server_address.m_ip.c_str(), m_server_address.m_port, 5);

		if (ret == ok)
		{
			m_reactor->post_msg(EVENT_MSG_ADD_HANDLER, this);
			state(handle_state::handle_opened);
		}
		return ret;
	}
}