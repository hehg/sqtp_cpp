#include "bus_server.h"
#include "log/sq_logger.h"
#include "bus_client.h"
#include "../base/bus_message.h"
using namespace sq;
namespace bus
{

    bus_server::bus_server(/* args */)
    {
        auto on_msg_fun=std::bind(&bus_server::on_client_msg,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
        auto on_connected_fun=std::bind(&bus_server::on_client_connected,this,std::placeholders::_1);
        auto on_disconnected_fun=std::bind(&bus_server::on_client_disconnected,this,std::placeholders::_1);
        m_ws_server=new ws_server(on_msg_fun,on_connected_fun,on_disconnected_fun,nullptr);
    }

    bus_server::~bus_server()
    {
        if(m_ws_server)
        {
            m_ws_server->close();
        }
    }
    void bus_server::open(const char *listen_addr)
    {
      int ret= m_ws_server->open(listen_addr);
      if(ret!=ok)
      {
        log_fata("open server failed");
      }
      log_info("open server success,listen to {}\n",listen_addr);
    }

    void bus_server::on_client_msg(ws_channel *channel, char *msg, int size)
    {
        
        sq_package* r=sq_package::create(msg,size);
        if(r==nullptr){
            std::string peer_addr;
            channel->get_peer_addr(peer_addr);
            log_warn("parse data from ws_channel error,address={}\n",peer_addr);
            return;
        }
        
        bus_client*cli=get_client(channel->get_id());
        if(cli==nullptr){
            return;
        }
        unsigned short tid=r->get_tid();
        log_info("recv msg:size={},tid={},topic={}\n",size,tid,r->get_topic());

        if(tid==bus_tid::bus_tid_sub_topic){
            bus_subcribe_req*f=r->get_field<bus_subcribe_req>(bus_subcribe_req::fid);
            if(f){
                log_info("recv bus_subcribe_req,topic={}\n",f->topic);
                cli->on_sub_topic(f);
            }else{
                log_warn("recv bus_subcribe_req,but no field\n");
            }
        }
        else if(tid==bus_tid::bus_tid_pub_topic)
        {
            //遍历
            for(auto&it:m_bus_client_map){
                it.second->on_pub_topic(r,cli);
            }
        }
        
        
    }
    
    void bus_server::on_client_connected(ws_channel *channel)
    {
        printf("client connected\n");
        bus_client_id++;
    
        bus_client*a= new bus_client(bus_client_id,channel);
        channel->set_id(bus_client_id);
        m_bus_client_map[bus_client_id]=a;

    }
    void bus_server::on_client_disconnected(ws_channel *channel)
    {
        log_info("client disconnected\n"); 
        auto it=m_bus_client_map.find(channel->get_id());
        if(it!=m_bus_client_map.end())
        {
            delete it->second;
            m_bus_client_map.erase(it);
        }
    }
    bus_client* bus_server::get_client(int client_id)
    {
        auto it=m_bus_client_map.find(client_id);
        if(it!=m_bus_client_map.end())
        {
            return it->second;
        }
        return nullptr;
    }

}