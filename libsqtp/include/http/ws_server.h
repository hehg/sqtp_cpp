#pragma once

#include "net/tcp_handler.h"
#include "net/reactor.h"
#include "net/net_socket.h"
#include  <unordered_map>
namespace sq
{
    using on_ws_msg_fun_t = std::function<void(char*msg,int size,void*handler)>;
    using on_ws_handshake_ok_fun_t = std::function<void(void*handler)>;

    class ws_handler;
    /**
     * websocket 监听服务端
     * 建立的连接都是一个ws_handler 实例
    */
    class ws_server : public event_handler
    {
    public:
        ws_server(on_ws_msg_fun_t on_msg,
                  on_session_connected_fun_t f1,
                  on_session_disconnected_fun_t f2,
                  on_ws_handshake_ok_fun_t f3 = nullptr)
        {
            m_on_ws_msg_fun = on_msg;
            m_on_session_connected_fun = f1;
            m_on_session_disconnected_fun = f2;
            m_on_ws_handshake_ok_fun = f3;
        };

        virtual ~ws_server();
        int open(const  char* address);
        void on_read();
        fd_t get_fd(){return m_fd;}

        void send_msg(void*handler,const char*msg,int size);
    protected:
        int  on_handshake_message(void *msg, int size, void *handler);
        void on_session_close(void *handler);
        using session_map=std::unordered_map<int, ws_handler *>;
        session_map m_sessions;
        int         m_session_id    = 0;
        fd_t        m_fd            = invalid_fd;
        //回调函数
        on_ws_msg_fun_t                 m_on_ws_msg_fun                 =nullptr;
        on_session_connected_fun_t      m_on_session_connected_fun      =nullptr;
        on_session_disconnected_fun_t   m_on_session_disconnected_fun   =nullptr;
        on_ws_handshake_ok_fun_t        m_on_ws_handshake_ok_fun        =nullptr;
        char tmp_buf[4096];
    };
}