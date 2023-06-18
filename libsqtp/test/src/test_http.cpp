#include "unit_test.h"
#include "net/reactor.h"
#include "http/http_client.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/ws_client.h"
#include <chrono>
using namespace sq;

SQ_TEST(test_http_client)
{
    auto t_fun=[](){
        sq_reactor*r=get_default_reactor();
        http_client cli;
        cli.get("http://127.0.0.1:8000/");
        while (true)
        {
           r->loop_once();
        }
        
    };
    std::thread t(t_fun);

    t.join();
}

SQ_TEST(test_http_parser_req)
{
    const char* http_req="GET / HTTP/1.1\r\n\
Upgrade: websocket\r\n\
Host: 127.0.0.1:1234\r\n\
Origin: http://127.0.0.1:1234\r\n\
Sec-WebSocket-Key: vNTaxsiainDj5USQTfUJ6g==\r\n\
Sec-WebSocket-Version: 13\r\n\
Connection: upgrade\r\n\
\r\n\
this this data";

   http_request http;

    
    int r = http.parser_http(http_req, strlen(http_req));
    cout << "result=" << r << "\n";
    http.dump();
}
SQ_TEST(test_http_parser_rsp)
{
    const char* http_req="HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n";

    http_response http;

    
    int r = http.parser_http(http_req, strlen(http_req));
    cout << "result=" << r << "\n";
    http.dump();
}
