
#include "bz_md_post.h"
#include "log/sq_logger.h"
#include "time/date_time.h"
#include "sq_struct_def.h"
#include "sq_macro.h"
#include "net/net_socket.h"
using namespace sq;
namespace sq_plug
{
    bz_md_post::bz_md_post()
    {
        m_name="bz_md_post";
        m_udp_socket=new udp_socket();
    }
    bz_md_post::~bz_md_post()
    {
        safe_delete(m_udp_socket);
    }
    int bz_md_post::open()
    {
        plug_base::open();
        std::string url=get_cfg_string("target");
        std::string local_ip=get_cfg_string("local_ip");

        
        int ttl=get_cfg_int("ttl");
        if(ttl){
            m_udp_socket->set_multicast_ttl(ttl);
        }

       
        m_udp_socket->open(url.c_str(),true,local_ip.c_str());

        int send_buf_size=get_cfg_int("send_buf_size",10);
        m_udp_socket->set_send_buf_size(send_buf_size*1024);

        //run_flag_.store(true);
        //m_engine = new std::thread(&bz_md_post::run, this);

          
        log_info("open plug bz_md_post\n");
        SQ_LOG_FLUSH();
        return 0;
    }
    int bz_md_post::close()
    {
        //run_flag_.store(false);
        // if (m_engine && m_engine->joinable())
        // {
        //     m_engine->join();
        // }
        // if (m_engine)
        // {
        //     delete m_engine;
        //     m_engine = nullptr;
        // }
        return 0;
    }
    int bz_md_post::put(uint16_t tid, char *data, uint16_t size)
    {
        m_udp_socket->send_to_dest(data,size);
        log_debug("post msg tid={},size={}\n",tid,size);
        return ok;
    }
    void bz_md_post::run()
    {
        // while (run_flag_.load() )
        // {
           
        //   os_sleep_ms(1000);
        // }
    }
}