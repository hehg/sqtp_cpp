#include "net/sq_ev_engine.h"
#include "net/tcp_handler.h"
namespace sq
{
    sq_ev_engine::~sq_ev_engine()
    {
        auto it = m_handlers.begin();
        for ( ; it!=m_handlers.end();++it) {
            it->second->close();
            //delete it->second;
        }
        m_handlers.clear();
        if(m_thread)
            delete m_thread;
    }
    void sq_ev_engine::start()
    {
        m_run_flag = true;
        m_thread = new thread([&]() {
            while (m_run_flag) {
                m_reactor.loop_once(0);
#ifdef WINDOWS
              //  std::this_thread::sleep_for(std::chrono::milliseconds(10));
#else
               // std::this_thread::sleep_for(std::chrono::microseconds(0));
#endif
            }
        });
    }
    void sq_ev_engine::stop()
    {
        m_run_flag = false;
        m_thread->join();
    }
    void sq_ev_engine::join()
    {
        m_thread->join();
    }
    int32_t sq_ev_engine::create_timer(int32_t id,int32_t timeout_ms, 
        timer_handler_fun_t fun)
    {
        timer_handler* h = new timer_handler(&m_reactor);
       
        h->open(id, timeout_ms);
        h->reg_handler(fun);
        m_id++;
        m_handlers[m_id]=h;
        return m_id;
    }
    tcp_client* sq_ev_engine::create_tcp_client(string server_addr, on_message_fun_t callback,
		int timeout)
    {
        tcp_client*h = new tcp_client(&m_reactor);
        h->reg_on_msg(callback);
        bool ret=h->open(server_addr.c_str());
        if (ret == false) {
            delete h;
            return nullptr;
        }
        
       
        m_id++;
        m_handlers[m_id] = (event_handler*)h;
        return h;
    }
}