#pragma once
#include <vector>
#include "udp_channel.h"

namespace sq
{
    class event_reactor;
	class udp_channel;

    class udp_sender
    {
        struct channel_info
        {
            udp_channel* channel=nullptr;
            std::string peer_address;
            int peer_port;
        };
    public:
      explicit  udp_sender(event_reactor *r = nullptr);
        ~udp_sender();
        int  add_channel(const char* server_address);
        void send(const char* data, int size);
	
    private:
        event_reactor* 						m_eg			=nullptr;
		std::vector<udp_sender::channel_info>			m_channels;     //key=socket fd
    };
}