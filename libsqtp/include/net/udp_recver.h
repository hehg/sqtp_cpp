#pragma once
#include <vector>
#include "udp_channel.h"
namespace sq
{
    class udp_recver
    {
         struct channel_info
        {
            udp_channel* channel;
            std::string peer_address;
            int peer_port;
        };
    public:
        udp_recver(on_udp_channel_msg_fun_t on_msg,
                   event_reactor *r = nullptr);
        ~udp_recver();

        int  add_channel(const char* server_address);
    private:
        int on_channel_message(void *channel, void *package, int size);
        event_reactor *             m_eg = nullptr;
        on_udp_channel_msg_fun_t    m_on_msg = nullptr;
        std::vector<channel_info>   m_channels;  
    };
}