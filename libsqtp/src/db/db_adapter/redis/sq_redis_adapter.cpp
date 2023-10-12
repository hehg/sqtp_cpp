#if defined(SUPPORT_DB_REDIS)
#include "db/db_adapter/redis/sq_redis_adapter.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "text/sq_string.h"
#include "net/net_address.h"
#include <hiredis/hiredis.h>

namespace sq
{
    sq_redis_adapter::sq_redis_adapter()
    {
    }

    sq_redis_adapter::~sq_redis_adapter()
    {
        if (m_context)
        {
            redisFree(m_context);
        }
    }

    int sq_redis_adapter::connect(const char *database, const char *host, const char *user, const char *pwd)
    {
        net_address addr;
		if(addr.parse_ipv4(host)==false)
		{
			log_error("sq_mysql_adapter::connect parse host address error\n");
			return err_db_connect;
		}

        m_context= redisConnect(addr.m_ip.c_str(), addr.m_port);
        if(m_context==nullptr || m_context->err)
        {
            log_error("sq_mysql_adapter::connect connect to redisServer error\n");
            return err_db_connect;
        }
		
        //选择数据库
        char cmd[32] = {0};
        if (database)
        {
            sprintf(cmd, "select %s", database);
        }
        else
        {
            strcpy(cmd, "select 0");
        }

        send_cmd(cmd);
        return ok;
    }
    int sq_redis_adapter::disconnect()
    {
        return ok;
    }

	void sq_redis_adapter::set_key_val(const char*key, const char*val)
	{
		redisReply *reply = (redisReply *)redisCommand(m_context,"set %s %s", key, val);
        if (reply)
        {
            freeReplyObject(reply);
        }
    }
    int sq_redis_adapter::get_key_val(const char*key,std::string&val)
    {
        redisReply* reply = (redisReply *)redisCommand(m_context,"get %s", key);
        if (reply && reply->type == REDIS_REPLY_STRING)
        {
            val = reply->str;
            
        }
        if (reply)
        {
            freeReplyObject(reply);
        }
        return ok;
    }
    void sq_redis_adapter::send_cmd(const char *cmd)
    {
        redisReply *reply = (redisReply *)redisCommand(m_context, cmd);
        if (reply)
        {
            freeReplyObject(reply);
        }
    }

    void sq_redis_adapter::publish(const char*topic, const char*val)
	{
		char cmd[1024] = { 0 };
		sprintf(cmd, "publish %s %s", topic, val);
		send_cmd(cmd);
	}
	void sq_redis_adapter::subsrcibe(const char*topic)
	{
        if(m_listen_thread==nullptr)
        {
            m_listen_thread=new thread(std::bind(&sq_redis_adapter::subsrcibe_thread,this));
        }
		char cmd[1024] = { 0 };
		sprintf(cmd, "subscribe %s", topic);
		redisReply *reply = (redisReply *)redisCommand(m_context, cmd);
		freeReplyObject(reply);

    }

    void sq_redis_adapter::subsrcibe_thread()
    {
        // 下面这段代码放到一个线程中执行
        redisReply *reply=nullptr;
        while (redisGetReply(m_context, (void **)&reply) == REDIS_OK)
        {
            // 订阅收到的消息是一个带三元素的数组
            if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
            {
                // 给业务层上报通道上发生的消息
                if (callback_fun_)
                {
                    callback_fun_(reply->element[1]->str, reply->element[2]->str);
                }
            }
            freeReplyObject(reply);
        }
    }
}

#endif