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




