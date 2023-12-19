#pragma once
#include "../base/server.h"
/**
 * 行情数据服务
*/
class md_server:public server
{
    public:
    DECLARE_SERVER(md_server);

    md_server();
    ~md_server();   
    int open(const char *xml_path)override;
    int stop()override;
};
