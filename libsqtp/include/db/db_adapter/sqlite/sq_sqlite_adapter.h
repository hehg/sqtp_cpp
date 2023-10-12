/**
 * @brief 基于otl 封装sqlite ，实现一个适配层
*/
#pragma once
#if defined(SUPPORT_DB_SQLITE)
//#include "pack/struct_reflect.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "db/db_adapter/sqlite/sqlite3.h"
#include "db/db_adapter/db_adapter.h"
class sqlite3;
namespace sq
{
   
    /**
     * 对sqlite3封装实现 基本功能
    */
   
    class sq_sqlite_adapter
    {
    public:
        static void db_init();

        /**
         * @param connect_info 连接串
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
        sqlite3 *m_db = nullptr;

        // 缓存stmt
        std::unordered_map<std::string, std::shared_ptr<sqlite3_stmt>> m_stmt_cache;
    };

    using sq_sqlite_db=sq_sqlite_adapter;
}
#endif 