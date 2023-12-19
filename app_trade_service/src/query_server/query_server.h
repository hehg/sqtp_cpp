#pragma once
#include "../base/server.h"
#include "pack/sq_package.h"
/**
 * 查询服务
*/
class query_server:public server
{
    DECLARE_SERVER(query_server);
    public:
    

    query_server();
    ~query_server();
    int open(const char *xml_path)override;
    int stop()override;
    int put(const char*data,int len)override;
    int on_msg(int tid,sq_package::ptr pkg,sq::ws_channel*from_channel) override;
};
