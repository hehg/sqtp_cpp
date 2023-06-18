#pragma once
#include <stdint.h>
#include <string>
#include <functional>
namespace sq
{
	using mq_recv_msg_fun_t = std::function<void(const char*topic,void*msg, int msg_len)>;
    class mq_cli
    {
        public:
        virtual int open(const char *server_addr, const char* group_id)=0;
        virtual int close()=0;
        virtual int send_message(const char*topic,void*msg,int msg_len)=0;
		virtual void sub_topic(const char*topic,int64_t start_offset) {};
		virtual void join() {};

		void reg_handler(mq_recv_msg_fun_t f) { m_hander = f; }
	protected:
		std::string m_server_addr;
		std::string m_group_id;

		mq_recv_msg_fun_t  m_hander = nullptr;
		
    };

	class mq_cli_factory
	{
	public:
		static mq_cli* create_mq_cli(const char*class_name);
	};
}