#pragma once
#include "net/tcp_channel.h"
#include "http/websocket.h"
namespace sq
{
    class ws_channel;
    using on_ws_channel_connected_fun_t 	= std::function<void(ws_channel*channel)>;
	using on_ws_channel_disconnected_fun_t  = std::function<void(ws_channel*channel)>;
	using on_ws_channel_msg_fun_t 			= std::function<int(ws_channel*channel,void*msg,int size)>;
    
    class ws_channel : public tcp_channel
    {
       
    public:
     enum{
            ws_state_closed,
            ws_state_handshaking,
            ws_state_handshak_ok,
            ws_state_handshak_fail
        };
        ws_channel();
        ws_channel(event_reactor *r);
        ws_channel(event_reactor *r, fd_t fd);
        /** 服务端收到握手请求
         * @return ok 或其它
        */
        int handshake_request(char *msg, int len);

        int handshake_response(char *msg, int len);
        /**
         * @brief 以websocket 格式发送数据包
         * @param opcode 操作码，参看 websocket.h 中定义
         * @param data 数据包地址
         * @param size 数据大小
         * @param fin  结束标识
         * @return ok 成功，否则失败
        */
        int send_ws_package( const char *data, uint32_t size,uint8_t opcode=OPCODE_TEXT, bool fin=true);

        int m_ws_state = ws_state_closed;    //通讯状态 0--握手 1--握手结束
        int m_session_id = 0;
        char        tmp_buf[4096];
    };
}