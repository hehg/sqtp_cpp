#if SUPPORT_REDIS==1
#include "db/sq_redis.h"
#include "hiredis/hiredis.h"
#define NO_QFORKIMPL //这一行必须加才能正常使用

#pragma comment(lib,"hiredis.lib")
#pragma comment(lib,"Win32_Interop.lib")


namespace sq
{
	sq_redis::sq_redis():m_context(nullptr)
	{

	}
	sq_redis::~sq_redis()
	{
		if (m_context)
		{
			redisFree(m_context);
		}
	}
	void sq_redis::connect(int db_index,string ip, int port)
	{
		m_context= redisConnect(ip.c_str(), port);
		if (m_context->err)
		{
			redisFree(m_context);
			printf("Connect to redisServer faile\n");
			return;
		}
		char cmd[32] = { 0 };
		sprintf(cmd, "select %d", db_index);
		send_cmd(cmd);
	}
	void sq_redis::set_key_val(string&key, string&val)
	{
		char cmd[1024] = { 0 };
		sprintf(cmd, "set %s %s", key.c_str(),val.c_str());
		send_cmd(cmd);
	}
	void sq_redis::set_key_val(const char*key, const char*val)
	{
		char cmd[1024] = { 0 };
		sprintf(cmd, "set %s %s", key, val);
		send_cmd(cmd);
	}
	void sq_redis::send_cmd(const char* cmd)
	{
		redisReply *reply = (redisReply *)redisCommand(m_context,cmd);
		freeReplyObject(reply);
	}

	void sq_redis::publish(const char*topic, const char*val)
	{
		char cmd[1024] = { 0 };
		sprintf(cmd, "publish %s %s", topic, val);
		send_cmd(cmd);
	}
	void sq_redis::subsrcibe(const char*topic)
	{
		char cmd[1024] = { 0 };
		sprintf(cmd, "subscribe %s", topic);
		redisReply *reply = (redisReply *)redisCommand(m_context, cmd);
		//freeReplyObject(reply);
		freeReplyObject(reply);
		while (redisGetReply(m_context,(void**)&reply) == REDIS_OK) {
			// consume message
			freeReplyObject(reply);
		}
	}
}
#endif