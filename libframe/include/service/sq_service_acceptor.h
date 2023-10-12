#pragma once

#include "sq_service_base.h"

namespace sq
{
    class ws_server;
    /**
     * @brief 基于websocket 协议的tcp 服务端
     */

    class sq_service_acceptor : public sq_service_base
    {
        DECLARE_SERVER(sq_service_acceptor);

    public:
        int open();
        void close();
        
        void on_session_connected(void *h);
        void on_session_disconnected(void *h);
        void on_ws_msg(char *msg, int size, void *h);
        void on_session_handshake_ok(void *);

        ws_server *m_server = nullptr;
        std::string m_listen_addr="ws://0.0.0.0:5401"
    };

    REGISTER_SERVER(sq_service_acceptor);
}