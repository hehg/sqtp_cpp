#pragma once
#include <string>
#include "sq_define.h"
#include "db/db_adapter/db_adapter.h"
#include <vector>
#include <thread>
#if SUPPORT_DB_REDIS
class redisContext;

namespace sq {
     using redis_msg_callback_func_t=std::function<void(char*topic,char*msg)>;
    class sq_redis_adapter 
    {
        public:
        sq_redis_adapter();
        ~sq_redis_adapter();

        int connect(const char *database, const char *host, const char *user, const char *pwd);
        int disconnect();

		void set_key_val(const char*key, const char*val);
		int get_key_val(const char*key,std::string&val);

		void publish(const char*topic, const char*val);
		void subsrcibe(const char*topic);

		void send_cmd(const char* cmd);

        void regist_callback(redis_msg_callback_func_t callback){
            callback_fun_=callback;
        }
        void subsrcibe_thread();
        
        std::thread*  m_listen_thread=nullptr;
        redisContext* m_context=nullptr;
        redis_msg_callback_func_t callback_fun_=nullptr;

    };
}
#endif