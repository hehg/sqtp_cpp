#pragma once
#include <memory>
#include <thread>
#include "container/ring_buffer.h"
#include <string.h>
#include <mutex>
#include "net/net_socket.h"
namespace sq {

  
    /** 目前基类
    */
    class log_target {
    public:
        log_target() = default;
       
        
        virtual void log(const char* log) 
		{
			(void)log;
		}
        virtual void flush() {}
        virtual void close() {}

		std::mutex  m_lock;
    };
    //标准输出
    
    class stdout_target:public log_target
    {
    public:
        stdout_target() = default;
		virtual void log(const char* log) override;

		void flush()override;
       
    };

   
    class file_target :public log_target
    {
    public:
        file_target(const char*file_name) {
            m_file = fopen(file_name, "w+");
        }
        ~file_target() {
            if (m_file)
                fclose(m_file);
        }
		virtual void log(const char* log) override;

		void flush()override;
          
        FILE* m_file;
       
    };
    
    /**异步文件输出，如果有锁，就支持多写一读
    如果没锁，就支持一读一些
    */
  
    class async_file_target :public log_target
    {
    public:
        async_file_target(const char*file_name,int queue_size=4096):
            m_thread(&async_file_target::loop,this)
        {
            m_file = fopen(file_name, "w+");
            m_msg_queue = new ring_buffer<string*>(queue_size);
        }
		~async_file_target();
		virtual void log(const char*log) override;

		void loop();
		void flush()override;
        ring_buffer<string*> *m_msg_queue=nullptr;//队列
        FILE* m_file;
      
        std::thread  m_thread;
        bool m_stop = false;
    };
#if 0
	class udp_target:public log_target
	{
		public:
			udp_target(const char*ip);
			virtual void log(const char* log) override;

			udp_socket m_udp;
	};
#endif
}
