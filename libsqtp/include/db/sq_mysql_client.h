#pragma once
#include "sq_define.h"
#include <vector>
#if SUPPORT_MYSQL_DB==1

namespace sq
{
	class sq_mysql_client;
	
	class sq_mysql_client
	{
	public:
		static void init_mysql();
		static void finish_mysql();

		class mysql_result
		{
		public:
			mysql_result();
			~mysql_result();
			bool next();
			
			int ret_len = 0;
			int m_num_fields=0;
			void *m_result=nullptr;

			std::vector<int>   m_fiels_len;
			std::vector<std::string> m_files_name;
			std::vector<std::string> m_files_value;
		};

		sq_mysql_client();

		int  open(const char* host_name,  const char* user, const char* passwd,
			const char* db_name=nullptr, int port = 3306);
		//执行指令没有返回结果
		int excute(const char* cmd);
		//执行查询有返回结果
		//返回查询结果个数
		mysql_result* select(const char*  query);
		void close();

		void* m_connector;
	};

}
#endif