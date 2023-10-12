#include "log/sq_logger.h"
#include "time/date_time.h"
#include "os/sq_path.h"
namespace sq{

	logger s_log;

    void logger::log(level_enum lvl, const char*file, int line, const char* msg)
    {
		m_lock.lock();
        if (lvl>m_level) {
			m_lock.unlock();
            return;
        }
		sq_time tm;
		date_time::get_sq_time(tm);

        sq_path path(file);
		snprintf(m_tmp_buf, sizeof(m_tmp_buf), "%02d-%02d|%02d:%02d:%02d.%03d|%s:%d|%s|%s",
			tm.m_month,
			tm.m_day,
			tm.m_hour,
			tm.m_minute,
			tm.m_second,
			tm.m_msec,
			path.get_name(),
			line,
			to_level_name(lvl),
			msg);

		m_tmp_buf[sizeof(m_tmp_buf) - 1] = '\0';
        if (!m_targets.empty())
        {
            for (auto &t : m_targets)
            {
                t->log(m_tmp_buf);
                if (lvl <= m_flush_level)
                {
                    t->flush();
                }
            }
        }
        else{
            std::cout<<m_tmp_buf;
        }

        //屏幕输出一下
		if (lvl == log_fata) {
			fprintf(stderr, "%s", m_tmp_buf);
			fflush(stderr);
			abort();
		}
		m_lock.unlock();
    }
    void logger::flush()
    {
        for (auto t : m_targets) {
            t->flush();
        }
    }
    void logger::add_stdout_target()
    {
        m_targets.push_back(std::make_shared<stdout_target>());
    }

    void logger::add_file_target(const char*file_name)
    {
        m_targets.push_back(std::make_shared<file_target>(file_name));
    }

    void logger::add_async_file_target_st(const char*file_name, int queue_size)
    {
        m_targets.push_back(std::make_shared<async_file_target>(file_name, queue_size));
    }
#if 0
	void logger::add_udp_target(const char*ip)
	{
		m_targets.push_back(std::make_shared<udp_target>(ip));

	}
#endif
}
