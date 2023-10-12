#include "sq_unit_test.h"
#include "http/ws_client.h"
#include "http/ws_server.h"
#include "net/reactor.h"
#include "log/sq_logger.h"
#include <thread>
#include <chrono>
using namespace sq;

SQ_TEST(test_ws_package)
{
    char src_buf[4096]={0};
    char des_buf[4096]={0};
    strcpy(src_buf,"hehong:hello"); //size=12
    int src_len=strlen(src_buf);
    int ws_len=make_ws_package(src_buf,src_len,des_buf,4096);
    assert(ws_len>0);

    ws_package result;
    result=parser_websocket_msg((const unsigned char*)des_buf,ws_len);
    assert(result.result==0);

    assert(result.body_size==src_len);
    assert(strcmp("hehong:hello",result.body)==0);
}
SQ_TEST(test_ws_client)
{
    s_log.add_stdout_target();
    SQ_LOG_LEVEL(sq::level_enum::log_debug);

    auto on_msg=[](const char*msg,int size)->int{
        string body_str(msg,size);
        cout<<body_str<<"\n";
        return 0;
    };
    ws_client ws;
    ws.reg_on_open([](void*){
        printf("connected\n");
    });
    ws.reg_on_ws_msg(on_msg);

    int ret=ws.open("ws://127.0.0.1:30000","/");
    assert(ret==0);

    std::thread t([&]{
        while (1)
        {
            char msg[128]={0};
            sprintf(msg,"{\"name\":\"hehong\"}");
            ws.send_ws_message(msg,strlen(msg));
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
     });

     reactor_run_forever();

    
    
     t.join();
}



