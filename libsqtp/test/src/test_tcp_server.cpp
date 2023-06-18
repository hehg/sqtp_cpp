#include "unit_test.h"
#include "log/sq_logger.h"
#include "net/tcp_server.h"
#include "http/ws_server.h"
using namespace sq;

class my_tcp_server : public tcp_server
{
public:
    virtual int on_client_message(void *msg, int size, void *handler)
    {
        printf("on_client_message:size=%d,msg=%s\n", size, (char *)msg);
        tcp_handler *rsp = (tcp_handler *)handler;
        rsp->send_data("nihao\n", 6);
        return size;
    }
    virtual void on_client_open(void *handler)
    {
        printf("on_client_open\n");
    }
    virtual void on_client_close(void *handler)
    {
        printf("on_client_close\n");
    }
};

SQ_TEST(test_net_case1)
{
    //my_tcp_server server;
    //server.open("tcp://127.0.0.1:5401");

    //get_default_reactor()->loop();
}

SQ_TEST(test_net_wss)
{
   //  my_tcp_server server;
   // server.listen("ws://127.0.0.1:5401");

    //get_default_reactor()->loop();
}

SQ_TEST(test_ws_server)
{
    s_log.add_stdout_target();
    SQ_LOG_LEVEL(sq::level_enum::log_debug);
    ws_server s([](char*msg,int size,void*from)->int{
        printf("on msg size=%d,[%s]\n",size,(char*)msg);
        return size;
    },
    [](void*h){
        printf("on cli  connected\n");
    },
    [](void*h){
        printf("on cli  disconnecte\n");
    },
    nullptr
    );
    s.open("ws://127.0.0.1:30000");
    
    reactor_run_forever();
}