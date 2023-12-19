#pragma once
#include <string>
#include "http/ws_client.h"
using namespace sq;
namespace bus
{
    namespace api
    {
        class  bus_api_rsp;
        
        void init_api(bus_api_rsp*rsp);
        int  connect(const char* address);
        void disconnect();
        int  subscribe(unsigned int topic);
        int  publish(unsigned int topic, const char* data,int size);
        int send_to_bus(unsigned short tid,unsigned int topic, const char* data,int size);

        class bus_api_rsp
        {
            public:
            virtual void on_connected(){};
            virtual void on_disconnected(){};
            virtual void on_topic(unsigned short topic, const char *data, int size){};
            virtual void on_message(const char* data,int size){};
        };
    }
}