#pragma once
#include "http/ws_channel.h"
#include <set>
#include "../base/bus_message.h"

namespace bus
{
 class bus_client
 {
 private:
    /* data */
 public:
    bus_client(int id,sq::ws_channel *ch);
    ~bus_client();
    void on_sub_topic(bus_subcribe_req*req);
    void on_pub_topic(sq::sq_package*package,bus_client*sender);

    sq::ws_channel * m_channel=nullptr;
    int m_id=0;                  //id
    std::set<int> m_sub_topics; //订阅的主题
 };
 
 
}
