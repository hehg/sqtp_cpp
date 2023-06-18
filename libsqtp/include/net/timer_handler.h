#pragma once
#include "sq_define.h"
#include "event_handler.h"
#include "sq_macro.h"
namespace sq
{
    using timer_handler_fun_t = std::function<void(int32_t timer_id)>;

	class timer_handler :public event_handler
	{
		DISABLE_COPY_MOVE_AND_ASSIGN(timer_handler);
	public:
		timer_handler(event_reactor*r);
		~timer_handler();
        void open(int32_t id, int64_t interval_ms);
        void close();
		virtual void on_timer(int32_t id) {
            if (m_fun) {
                m_fun(m_id);
            }
        };
        void reg_handler(timer_handler_fun_t fun) { m_fun = fun; }
	protected:

		virtual void on_read() ;
		virtual void on_close();
		
	public:
#ifdef WINDOWS
		HANDLE m_fd=0;
        inline fd_t get_fd() { return 0; }
#else
        inline fd_t get_fd() { return m_fd; }
        fd_t   m_fd=-1;
#endif
        int32_t m_id = 0;
        timer_handler_fun_t m_fun = nullptr;
	};
    

}
