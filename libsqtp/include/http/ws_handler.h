#pragma once
#include "net/tcp_handler.h"
namespace sq
{
    class ws_handler : public tcp_handler
    {
       
    public:
     enum{
            ws_state_closed,
            ws_state_handshaking,
            ws_state_handshak_ok,
        };
        ws_handler();
        ws_handler(event_reactor *r);
        ws_handler(event_reactor *r, fd_t fd);
        /** 处理握手请求
         * @return 0-ok 
        */
        int handshake_request(char *msg, int len);

        int m_state = ws_state_closed;    //通讯状态 0--握手 1--握手结束
        int m_session_id = 0;
    };
}