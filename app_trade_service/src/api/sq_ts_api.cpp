#include "sq_ts_api.h"
#include "http/ws_client.h"
#include "error/sq_err.h"
#include <thread>
#include "log/sq_logger.h"
#include "pack/sq_package.h"
#include "sq_tid_define.h"
#include "sq_struct_def.h"

using namespace sq;
static ws_client*       ws_client_ptr = nullptr;
static std::thread*     s_thread = nullptr;
static sq_ts_api_rsp*   s_api_rsp=nullptr;

static int on_server_msg(ws_channel *channel, void *msg, int size)
{
    auto pkg = sq_package::create((char *)msg, size);
    if (pkg)
    {
        if (pkg->get_tid() == tid_query_contract_rsp)
        {
            sq_rsp_msg *rsp_msg = pkg->get_field<sq_rsp_msg>();
            if (rsp_msg)
            {
                log_debug("rsp_msg->code:{},msg={}\n", rsp_msg->code,get_err_name(rsp_msg->code));
            }
            sq_contract*contract=pkg->get_field<sq_contract>();
            s_api_rsp->on_qry_contract_list_rsp(rsp_msg,contract);
        }
    }
    else
    {
        log_warn("invalid package\n");
    }
    char *data = (char *)msg;
    
    return size;
}
static void on_server_connected(ws_channel *channel)
{
    printf("client connected\n");
}

static void on_server_disconnected(ws_channel *channel)
{
    printf("client disconnected\n");
}
int sq_ts_api_connect(const char *address)
{
    s_log.add_stdout_target();
    if (ws_client_ptr == nullptr)
    {
        ws_client_ptr = new ws_client(on_server_msg, on_server_connected,
                                      on_server_disconnected);
    }
    if (s_thread == nullptr)
    {
        s_thread = new std::thread([&]()
                                   { get_default_reactor()->loop(); });
    }
    ws_client_ptr->open(address, "/");

    return ok;
}
void sq_ts_api_set_rsp(sq_ts_api_rsp* rsp)
{
    s_api_rsp=rsp;     
}
int sq_ts_api_disconnect()
{
    if (ws_client_ptr)
    {
        ws_client_ptr->close();
        //  delete ws_client_ptr;
        ws_client_ptr = nullptr;
    }
    return ok;
}

int sq_ts_api_qry_contract_list(const char*market,const char*contract)
{
    log_debug("sq_ts_api_qry_contract_list\n");
    auto pack = sq_package::create(tid_query_contract_req);
    sq_qry_contract_req *info = pack->alloc_field<sq_qry_contract_req>(sq_qry_contract_req::fid);
    info->market_id = market;
    info->contract_id = contract;
    int ret = ws_client_ptr->send_ws_message(pack->get_data(), pack->get_size());
    if (ret != ok)
    {
        log_warn("send ws message error,ret={}\n", ret);
        return ret;
    }
    return ok;
}