#include "gate_server.h"
#include "http/ws_server.h"
#include "log/sq_logger.h"
#include "error/sq_err.h"
#include "sq_macro.h"
#include "pack/sq_package.h"
#include "sq_tid_define.h"

using namespace sq;

gate_server::gate_server()
{
    m_id=ID::gate_server;
}
gate_server::~gate_server()
{
  safe_delete(m_server);
}
int gate_server::open(const char*xml_path)
{
    
    listen_addr_=get_cfg_string("listen_addr","ws://0.0.0.0:5401");
    m_server=new ws_server(
        std::bind(&gate_server::on_channel_msg,this,std::placeholders::_1,
        std::placeholders::_2,std::placeholders::_3),
        std::bind(&gate_server::on_channel_conneted,this,std::placeholders::_1),
        std::bind(&gate_server::on_channel_disconneted,this,std::placeholders::_1),
        nullptr
      );

  
  log_info("gate_server::open ok\n");
  return ok;
}
int gate_server::start()
{
    m_server->open(listen_addr_.c_str());
   log_info("gate_server::start ok,listen {}\n",listen_addr_.c_str());

    return ok;
}
int gate_server::stop()
{
    m_server->close();
    return ok;
}

void gate_server::on_channel_conneted(sq::ws_channel*channel)
{
    log_info("gate_server::on_channel_conneted\n");
}

void gate_server::on_channel_disconneted(sq::ws_channel*channel)
{
    log_info("gate_server::on_channel_disconneted\n");
}

void gate_server::on_channel_msg(sq::ws_channel*channel,char*msg,int size)
{
   
    auto pack=sq_package::create(msg,size);
    
    if(pack==nullptr){
        log_warn("parser package error\n");
        return;
    }
    uint16_t tid=pack->get_tid();
    log_info("gate_server::on_channel_msg,tid={}\n",tid);
    //收到请求
    if(tid==tid_query_contract_req)
    {
        server::post_msg(pack->get_tid(),pack,channel);
        // //把这个包转给查询服务
        // server_cmd cmd;
        // cmd.from_server=m_id;
        // cmd.target_server=server::ID::query_server;
        // cmd.pkg=pack;
        // cmd.tid=tid;
        // cmd.from_channel=channel;
        // post_cmd(&cmd);
    }
    
}