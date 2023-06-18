#pragma once
#include "sq_define.h"
namespace sq
{
	class net_address
	{
	public:
		net_address(){
			m_port = 0;
			m_protocol = "";
			m_ip = "";
		};
		~net_address(){}

		bool parse_ipv4(string address);
		int get_sockaddr(sockaddr &sock_addr);

		string 		m_ip;
		int			m_port=0;
		string 		m_protocol;
		bool 		m_is_multicast = false;
	};
}