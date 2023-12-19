#pragma once
#include <vector>
#include "log_common.h"
#include "log_targer.h"
#include "sync/sq_spin_lock.h"
#include "fmt/format.h"
namespace sq
{

    /** 同步日志
    */
    class logger
    {
    public:
        logger() {}

        void log(level_enum lvl, const char *file, int line,const char *msg);
        void flush();

        void set_level(level_enum lvl) { m_level = lvl; }
        void set_flush_level(level_enum lvl) { m_flush_level = lvl; }
        //标准输出
        void add_stdout_target();
        //文件输出
        void add_file_target(const char *file_name);

        //异步文件输出
        void add_async_file_target_st(const char *file_name, int queue_size);
#if 0
		void add_udp_target(const char*ip);
#endif
        std::vector<log_target_ptr> m_targets;
        level_enum m_level = log_debug;
        level_enum m_flush_level = log_warn;
        char m_tmp_buf[1024];
        sq_spin_lock m_lock;
    };
    //全局日志对象
    extern logger s_log;
    
#define log_info(...)                                             \
    {                                                             \
        if (log_info <= s_log.m_level)                            \
        {                                                         \
            std::string __s = fmt::format(__VA_ARGS__);           \
            s_log.log(log_info, __FILE__, __LINE__, __s.c_str()); \
        }                                                         \
    }

#define log_warn(...)                                             \
    {                                                             \
        if (log_warn <= s_log.m_level)                            \
        {                                                         \
            std::string __s = fmt::format(__VA_ARGS__);           \
            s_log.log(log_warn, __FILE__, __LINE__, __s.c_str()); \
        }                                                         \
    }

#define log_debug(...)                                             \
    {                                                              \
        if (log_debug <= s_log.m_level)                            \
        {                                                          \
            std::string __s = fmt::format(__VA_ARGS__);            \
            s_log.log(log_debug, __FILE__, __LINE__, __s.c_str()); \
        }                                                          \
    }

#define log_error(...)                                                \
    {                                                                 \
        fprintf(stdout, PRINT_COLOR_RED);                             \
        std::string __s = fmt::format(__VA_ARGS__);                   \
        s_log.log(log_err, __FILE__, __LINE__, __s.c_str());          \
        fprintf(stderr, "%s:%d|%s", __FILE__, __LINE__, __s.c_str()); \
        fprintf(stdout, PRINT_COLOR_NONE);                            \
    }

#define log_fata(...)                                        \
    {                                                        \
        std::string __s = fmt::format(__VA_ARGS__);          \
        s_log.log(log_err, __FILE__, __LINE__, __s.c_str()); \
        std::cout << __s << std::endl;                       \
        s_log.flush();                                       \
        assert(false);                                       \
    }
//============指定日志对象==============
#define m_log_info(mlog, ...)                                    \
    {                                                            \
        if (log_info <= log.m_level)                             \
        {                                                        \
            std::string __s = fmt::format(__VA_ARGS__);          \
            mlog.log(log_info, __FILE__, __LINE__, __s.c_str()); \
        }                                                        \
    }
#define m_log_debug(mlog, ...)                                    \
    {                                                             \
        if (log_debug <= log.m_level)                             \
        {                                                         \
            std::string __s = fmt::format(__VA_ARGS__);           \
            mlog.log(log_debug, __FILE__, __LINE__, __s.c_str()); \
        }                                                         \
    }
#define m_log_error(mlog, ...)                                    \
    {                                                             \
        if (log_error <= log.m_level)                             \
        {                                                         \
            std::string __s = fmt::format(__VA_ARGS__);           \
            mlog.log(log_error, __FILE__, __LINE__, __s.c_str()); \
        }                                                         \
    }
#define m_log_fata(mlog, ...)                                    \
    {                                                            \
        if (log_fata <= log.m_level)                             \
        {                                                        \
            std::string __s = fmt::format(__VA_ARGS__);          \
            mlog.log(log_fata, __FILE__, __LINE__, __s.c_str()); \
            std::cerr << __s << std::endl;                       \
            assert(false);                                       \
        }                                                        \
    }

    //========不建议使用的日志函数=======================


#define SQ_LOG_LEVEL(l)     \
    do                      \
    {                       \
        s_log.set_level(l); \
    } while (0)

#define sq_panic(...)                                   \
    do                                                   \
    {                                                    \
        fprintf(stdout, PRINT_COLOR_RED);                \
        char _buf_[1024];                                \
        snprintf(_buf_, sizeof(_buf_), __VA_ARGS__);     \
            std::cerr<< __FILE__<<":"<<__LINE__<<"|"<<_buf_; \
        fprintf(stdout, PRINT_COLOR_NONE);               \
    } while (0)

#define SQ_LOG_FLUSH() \
    do                 \
    {                  \
        s_log.flush(); \
    } while (0)

   
}
