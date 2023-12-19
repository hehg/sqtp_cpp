#include "net/udp_recver.h"
#include "net/net_address.h"
#include "error/sq_err.h"

namespace sq
{
    udp_recver::udp_recver(on_udp_channel_msg_fun_t on_msg,
                           event_reactor *r)
    {
        m_on_msg = on_msg;
        if (r)
        {
            m_eg = r;
        }
        else
        {
            m_eg = get_default_reactor();
        }
    }
    udp_recver::~udp_recver()
    {
        for(auto it=m_channels.begin();it!=m_channels.end();++it)
		{
			it->channel->close();
		}
		m_channels.clear();
    }
    
    int udp_recver::add_channel(const char *remote_address)
    {
        net_address address;
        address.parse_ipv4(remote_address);
        if (address.m_port == 0)
        {
            throw("Invalid port");
            return err_invalid_param;
        }

        udp_channel *channel = new udp_channel(m_eg);
        channel->reg_on_msg(std::bind(&udp_recver::on_channel_message, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        channel_info info;
        info.channel = channel;
        info.peer_address = address.m_ip;
        info.peer_port = address.m_port;
        channel->bind(address.m_ip.c_str(), address.m_port);
        m_channels.push_back(info);
        m_eg->post_msg(EVENT_MSG_ADD_HANDLER, channel);
        channel->state(handle_state::handle_opened);

        return ok;
    }
    int udp_recver::on_channel_message(void *channel, void *package, int size)
    {
        if (m_on_msg)
        {
            return m_on_msg((udp_channel *)channel, package, size);
        }
        return size;
    }
    
}