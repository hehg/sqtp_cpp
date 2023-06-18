#include "net/notify_handler.h"
#include "net/net_socket.h"
#include "error/sq_err.h"
#include "base/sq_util.h"
#include "net/reactor.h"
#include "sq_define.h"

namespace sq
{
	notify_handler::notify_handler(event_reactor*r):
        event_handler(r)
	{
#ifdef LINUX
#if SUPPORT_EVENT_FD==1
		m_fd = eventfd(0, 0);
		if (m_fd == -1) {
			assert(errno == ENFILE || errno == EMFILE);
		}
#endif
#endif

#if(defined WINDOWS)
		m_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_event == NULL)
		{
			assert(false);
		}
#endif
	}

	notify_handler::~notify_handler()
	{
#ifdef LINUX
#if SUPPORT_EVENT_FD==1
		int rc = ::close(m_fd);
#endif
#endif

#if(defined WINDOWS)
		CloseHandle(m_event);
#endif
	}

	void notify_handler::on_read()
	{
#ifdef LINUX
#if SUPPORT_EVENT_FD==1 
		uint64_t dummy;
		ssize_t sz = read(m_fd, &dummy, sizeof(dummy));
		assert(sz == sizeof(dummy));
#endif
#endif

#if(defined WINDOWS)
		SetEvent(m_event);
#endif
	}

	void notify_handler::send()
	{
#ifdef LINUX
#if  SUPPORT_EVENT_FD==1
		const uint64_t inc = 1;
		ssize_t sz = write(m_fd, &inc, sizeof(inc));
#endif
#endif

#if(defined WINDOWS)

#endif
	}

	int notify_handler::wait(int timeout_)
	{
#if defined(WINDOWS)
		DWORD ret=WaitForSingleObject(m_event, timeout_);
		ResetEvent(m_event);
		if (ret == WAIT_OBJECT_0)
		{
			return ok;
		}
		else if (WAIT_TIMEOUT == ret)
		{
			return err_time_out;
		}
		else {
			return err_fail;
		}
#else
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(m_fd, &fds);
		struct timeval timeout;
		if (timeout_ >= 0) {
			timeout.tv_sec = timeout_ / 1000;
			timeout.tv_usec = timeout_ % 1000 * 1000;
		}
		int rc = select (m_fd + 1, &fds, NULL, NULL,
			timeout_ >= 0 ? &timeout : NULL);
		if  (rc < 0) {
			return err_fail;
		}
		if (rc == 0) {
			return err_time_out;
		}
		else {
			return ok;
		}
#endif
	}

}
