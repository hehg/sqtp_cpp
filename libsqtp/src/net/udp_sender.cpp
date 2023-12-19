#include "net/udp_sender.h"
#include "net/net_address.h"
#include "error/sq_err.h"
namespace sq
{
    udp_sender::udp_sender(event_reactor *r)
    {
        if (r)
        {
            m_eg = r;
        }
        else
        {
            m_eg = get_default_reactor();
        }
    }
    udp_sender::~udp_sender()
    {
        for(auto it=m_channels.begin();it!=m_channels.end();++it)
		{
			it->channel->close();
		}
		m_channels.clear();
    }
    int udp_sender::add_channel(const char* server_address)
	{
		net_address address;
		address.parse_ipv4(server_address);
		if (address.m_port == 0){
			throw("Invalid port");
			return err_invalid_param;
		}
			
		udp_channel*channel=new udp_channel(m_eg);
		
        udp_sender::channel_info info;
        info.channel=channel;
        info.peer_address=address.m_ip;
        info.peer_port=address.m_port;
		m_channels.push_back(info);
        

		m_eg->post_msg(EVENT_MSG_ADD_HANDLER, channel);
		channel->state(handle_state::handle_opened);
		

		return ok;
	}
     
    void udp_sender::send(const char *data, int size)
    {
         for(auto it=m_channels.begin();it!=m_channels.end();++it)
		{
			it->channel->send_data(data, size, it->peer_address.c_str(), it->peer_port);
		}
    }
}