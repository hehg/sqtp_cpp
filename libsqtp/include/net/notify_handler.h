/** 事件通知
*/

#pragma once
#include "sq_define.h"
#include "sq_types.h"
#include "sq_macro.h"
#include "event_handler.h"

namespace sq
{

  class notify_handler : public event_handler
  {
    DISABLE_COPY_MOVE_AND_ASSIGN(notify_handler);

  public:
    notify_handler(event_reactor *r);
    ~notify_handler();

    inline fd_t get_fd() { return m_fd; }
    void send();
    int wait(int timeout);
    virtual void on_read();

  private:
#ifdef WINDOWS
    HANDLE m_event;
#endif
    fd_t m_fd = 0;
  };
}
