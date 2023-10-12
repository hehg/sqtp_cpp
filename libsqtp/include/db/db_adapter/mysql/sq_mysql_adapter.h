#pragma once
#include "sq_define.h"
#include "db/db_adapter/db_adapter.h"
#include <vector>
#if SUPPORT_DB_MYSQL

namespace sq
{

	class sq_mysql_adapter
	{
	public:
		static void db_init();
		static void db_uninit();
		
		sq_mysql_adapter();
		~sq_mysql_adapter();
        /**
		 * @brief 连接 mysql 数据库
         * @param database 数据库名称
         * @param host  数据库地址 例如 mysql://127.0.0.1:3366
         * @param user   用户名
         * @param pwd    密码
         */
        int connect(const char *database, const char *host, const char *user, const char *pwd);
        int disconnect();

        int begin_trans();
        int commit();
        int rollback();

        /**
         * @brief 执行sql命令
         */
        int exec_sql(const char*sql,std::vector<db_bind_info> &params,db_result_func_t cb);
      
        int exec_sql(const char*sql,db_result_func_t cb);
private:
        void on_db_error(const char *sql, int err_code);

		void* m_db=nullptr;
		void* m_connector=nullptr;
	};

}
#endif