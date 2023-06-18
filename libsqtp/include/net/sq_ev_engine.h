#pragma once
#include "net/reactor.h"
#include "net/timer_handler.h"
#include "net/tcp_client.h"
namespace sq
{
    using check_once_fun_t = std::function<void()>;
    /** 一个引擎，包含了一个线程
    */
    class sq_ev_engine
    {
    public:
        sq_ev_engine() = default;
        ~sq_ev_engine();
        void start();
        void join();
        void stop();
       
        int32_t create_timer(int32_t id,int32_t timeout_ms, timer_handler_fun_t fun);
        tcp_client* create_tcp_client(string server_addr, on_message_fun_t callback,int timeout=5);
      //  void loop() { m_reactor.loop(); }

        sq_reactor m_reactor;
        std::thread* m_thread=nullptr;
        bool m_run_flag = false;

        std::unordered_map<int32_t,event_handler*> m_handlers;
        int  m_id=0;
    };
}