#pragma once
#include "../base/server.h"
#include "sq_contract_table.h"
/**
 * 从数据库，或者文件中加载初始数据
 * 内存中维护所有实时数据
*/
class data_server:public server
{
    public:
    DECLARE_SERVER(data_server);

    data_server();
    ~data_server();
    int open(const char *xml_path)override;
    int start()override;
    int stop()override;
    int put(const char*data,int len)override;
   
   //静态数据部分
    sq::sq_contract_table m_contract_table;
};
