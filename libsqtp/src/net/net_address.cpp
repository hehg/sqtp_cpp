#include "net/net_address.h"

namespace sq
{
	bool net_address::parse_ipv4(string address)
	{
		/* tcp://127.0.0.1:8080  */
		int ip_pos = 0;
		int pro_pos = address.find("://");
		if (pro_pos >1) {
			m_protocol = address.substr(0, pro_pos);
			ip_pos = pro_pos+ 3;
		}
		else{
			return false;
		}
		
		int port_pos = address.find(':', ip_pos);
		if (port_pos > 0) {
			string str_port = address.substr(port_pos + 1, address.length()- port_pos);
			m_port = atoi(str_port.c_str());
		}
		else {
			port_pos = address.length();
		}
		m_ip = address.substr(ip_pos, port_pos- ip_pos);
		if (m_ip == "*")
			m_ip = "";
		if(m_protocol=="udp"){
			int first=atoi(m_ip.c_str());
			if(first>=224&&first<=239){
				m_is_multicast = true;
			}
			else{
				
			}
		}
		else if(m_protocol=="ump"){
			int first=atoi(m_ip.c_str());
			if(first>=224&&first<=239){

			}
			
			m_is_multicast = true;
		}
		if(m_port==0){
			if(m_protocol=="ws"){
				m_port=80;
			}
			else if(m_protocol=="wss"){
				m_port=433;
			}
			else{
				return false;
			}
		}
		
		return true;
	}

	int net_address::get_sockaddr(sockaddr &sock_addr)
	{
		struct sockaddr_in* listen_addr = (sockaddr_in*)&sock_addr;
		
	//	memset(&listen_addr, 0, sizeof(listen_addr));
		listen_addr->sin_family = AF_INET;
		
		if (m_ip.empty())
			listen_addr->sin_addr.s_addr = INADDR_ANY;
		else
			listen_addr->sin_addr.s_addr = inet_addr(m_ip.c_str());
		listen_addr->sin_port = htons(m_port);

		return sizeof(sockaddr);
	}
}