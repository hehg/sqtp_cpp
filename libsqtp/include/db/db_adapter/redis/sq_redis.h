#pragma once
#include "sq_define.h"
#if SUPPORT_REDIS==1

class redisContext;
namespace sq
{
	class sq_redis
	{
	public:
		sq_redis();
		~sq_redis();
		void connect(int db_index=0,string ip="127.0.0.1", int port=6379);
		void set_key_val(string&key, string&val);
		void set_key_val(const char*key, const char*val);

		void publish(const char*topic, const char*val);
		void subsrcibe(const char*topic);
	private:
		void send_cmd(const char* cmd);
		redisContext* m_context;
	};


}
#endif