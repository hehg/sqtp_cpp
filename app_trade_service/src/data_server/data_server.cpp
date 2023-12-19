#include "data_server.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "../app.h"
using namespace sq;
data_server::data_server()
{
    m_id=ID::data_server;
}
data_server::~data_server()
{
}
int data_server::open(const char *xml_path)
{
    log_info("data_server open ok\n");
    return ok;
}
int data_server::stop()
{
    return ok;
}
int data_server::start()
{
    //加载初始数据
    std::string text_file_path="../db/tb_contract.txt";
    the_app.m_memory_db->m_contract_table->load_from_text(text_file_path.c_str());
    log_debug("load contract ,count={}\n",the_app.m_memory_db->m_contract_table->size());
    
    return ok;
}
int data_server::put(const char*data,int len)
{
    server_cmd*cmd=(server_cmd*)data;
    return ok;
}
