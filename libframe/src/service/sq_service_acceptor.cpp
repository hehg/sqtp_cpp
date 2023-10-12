#include "sq_service_acceptor.h"
#include "http/ws_server.h"
namespace sq
{
    int sq_service_acceptor::open()
    {
        m_listen_addr = get_cfg<std::string>("listen_addr", m_listen_addr);

        m_server = new ws_server(std::bind(&sq_service_acceptor::on_ws_msg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                                 std::bind(&sq_service_acceptor::on_session_connected, this, std::placeholders::_1),
                                 std::bind(&sq_service_acceptor::on_session_disconnected, this, std::placeholders::_1),
                                 std::bind(&sq_service_acceptor::on_session_handshake_ok, this, std::placeholders::_1)); );
        m_server->open(m_listen_addr.c_str());
        return ok;
    }
    void sq_service_acceptor::close()
    {
    }

    void sq_service_acceptor::on_session_connected(void *h)
    {
        printf("on_session_connected\n");
    }
    void sq_service_acceptor::on_session_disconnected(void *h)
    {
        printf("on_session_disconnected\n");
    }
    void sq_service_acceptor::on_ws_msg(char *msg, int size, void *h)
    {
        string recv_msg(msg, size);
        printf("on_ws_msg,size=%d,msg=%s\n", size, recv_msg.c_str());

        // m_server->send_msg(h,recv_msg.c_str(),recv_msg.size());
    }
    void sq_service_acceptor::on_session_handshake_ok(void *)
    {
        printf("on_session_handshake_ok\n");
    }
}