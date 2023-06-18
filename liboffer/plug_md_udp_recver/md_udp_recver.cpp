
#include "md_udp_recver.h"
#include "log/sq_logger.h"
#include "time/date_time.h"
#include "sq_struct_def.h"
#include "sq_macro.h"
#include "net/net_socket.h"
using namespace sq;

/**
 * 实现udp各种数据的接收，转换为内部格式
*/
namespace sq_plug
{
    md_udp_recver::md_udp_recver()
    {
        m_name="md_udp_recver";
        m_udp_socket=new udp_socket();
    }
    md_udp_recver::~md_udp_recver()
    {
        safe_delete(m_udp_socket);
    }
    int md_udp_recver::open()
    {
    
        plug_base::open();
        std::string url=get_cfg_string("server");
        std::string local_ip=get_cfg_string("local_ip");

        m_udp_socket->open(url.c_str(),false,local_ip.c_str());
        m_udp_socket->set_unblock();
        int send_buf_size=get_cfg_int("recv_buf_size",10);
        m_udp_socket->set_recv_buf_size(send_buf_size*1024);

        m_run_flag=true;
        m_engine = new std::thread(&md_udp_recver::run, this);

        log_info("open plug md_udp_recver\n");
        SQ_LOG_FLUSH();
        return 0;
    }
    int md_udp_recver::close()
    {
        m_run_flag=false;
      
        if (m_engine && m_engine->joinable())
        {
            m_engine->join();
        }
        if (m_engine)
        {
            delete m_engine;
            m_engine = nullptr;
        }
        return 0;
    }
    int md_udp_recver::put(uint16_t tid, char *data, uint16_t size)
    {
      
        return ok;
    }
    void md_udp_recver::run()
    {
        char buffer[4096];
        while (m_run_flag)
        {
           int count = m_udp_socket->recv_from(buffer, sizeof(buffer));
            if (count == 0)
            {
               std::this_thread::yield();
                continue;
            }

            if (count == -1)
            {
                int nErrno = GET_LAST_SOCK_ERROR();
                if (nErrno == 0 || nErrno == 251 || nErrno == EWOULDBLOCK)
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                    continue;
                }

                else
                {
                    log_warn("Error in recvFrom,ErrNo={}\n", nErrno);
                    return;
                }
            }

            //m_counter++;

            int *type = (int *)buffer;
            int tid = *type;

            if (m_call_back)
            {
                m_call_back(tid, (char *)buffer, count, m_call_back_param, m_id);
            }
          
        }
    }
}