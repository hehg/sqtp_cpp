#pragma once
#include <string>
#include "../base/server.h"
namespace sq
{
    class ws_server;
    class ws_channel;
}

class gate_server:public server
{
public:
    DECLARE_SERVER(gate_server);

    gate_server();
    ~gate_server();

    int open(const char *xml_path)override;
    int start()override;
    int stop()override;

    void on_channel_conneted(sq::ws_channel *channel);
    void on_channel_disconneted(sq::ws_channel *channel);
    void on_channel_msg(sq::ws_channel*channel,char*msg,int size);

private:
    std::string listen_addr_;
    sq::ws_server *m_server = nullptr;
};

