#include "query_server.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_utils.hpp"
#include "../data_server/data_server.h"
#include "../app.h"
#include "http/ws_channel.h"

static sq_memory_db *s_memory_db = nullptr;
using namespace sq;
query_server::query_server()
{
    m_id = ID::query_server;
}
query_server::~query_server()
{
}
int query_server::open(const char *xml_path)
{

    s_memory_db = the_app.m_memory_db;
    log_info("query_server open ok\n");
    return ok;
}
int query_server::stop()
{
    return ok;
}

int query_server::put(const char *data, int len)
{
    static char rsp_buf[4096];
    server_cmd *cmd = (server_cmd *)data;
    log_debug("query_server::put,tid={},from={},target={}\n", cmd->tid, cmd->from_server, cmd->target_server);

    

    return ok;
}
int query_server::on_msg(int tid, sq_package::ptr pkg, sq::ws_channel *from_channel)
{
    // 通过数据服务进行查询
    if (tid == tid_query_contract_req)
    {
        std::shared_ptr<sq::sq_package> rsp_pack = nullptr;
        rsp_pack = sq_package::create(tid_query_contract_rsp, 4096);
        sq_qry_contract_req *req = pkg->get_field<sq_qry_contract_req>();
        if (req)
        {
            std::stringstream ss;
            req->to_string(ss);
            log_debug("sq_qry_contract_req={}\n", ss.str().c_str());

            std::string key = req->market_id.to_string() + "_" + req->contract_id.to_string();
            sq_contract *contract = s_memory_db->m_contract_table->find(key);
            if (contract)
            {
                // 发送应答
                sq_rsp_msg *rsp_msg = rsp_pack->alloc_field<sq_rsp_msg>();
                rsp_msg->code = ok;
                sq_contract *des_contract = rsp_pack->alloc_field<sq_contract>();
                memcpy(des_contract, contract, sizeof(sq_contract));
                from_channel->send_ws_package(rsp_pack->get_data(), rsp_pack->get_size());
            }
            else
            {
                log_info("no any contract be fund\n");

                sq_rsp_msg *rsp_msg = rsp_pack->alloc_field<sq_rsp_msg>();
                rsp_msg->code = err_no_data;
                from_channel->send_ws_package(rsp_pack->get_data(), rsp_pack->get_size());
            }
        }
        else
        {
            log_warn("invalid request\n");
        }
    }
     return ok;
}
