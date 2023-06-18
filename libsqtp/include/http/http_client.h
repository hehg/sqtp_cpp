#pragma once 
#include "net/tcp_client.h"
namespace sq
{
    class tcp_client;

    class http_client
    {
    public:
        http_client();
        ~http_client();
        void get(const char *url);
        int on_message(void*package, int size,void*from);
    private:
        tcp_client *m_tcp;
        int rsp_state=0;
    };
}