#pragma once
#include <map>
#include "http/ws_server.h"
using namespace sq;
namespace bus
{
    class bus_client;
    class bus_server
    {
    private:
        
    public:
        bus_server();
        ~bus_server();
        void open(const char*listen_addr);
        
        void on_client_msg(ws_channel*channel,char*msg,int size);
        void on_client_connected(ws_channel*channel);
        void on_client_disconnected(ws_channel*channel);

        bus_client* get_client(int client_id);



        ws_server* m_ws_server=nullptr;
        int bus_client_id=0;
        std::map<int,bus_client*> m_bus_client_map;
       
    };
    
    
    
}