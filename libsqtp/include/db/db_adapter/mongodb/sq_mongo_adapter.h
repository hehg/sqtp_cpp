/**
 * @brief 基于 mongocxx封装 ，实现一个适配层
 */
#pragma once
#include "pack/struct_reflect.h"
#if defined(SUPPORT_DB_MONGO)
 class mongocxx::client;
namespace sq
{
    /**
     * mongodb 适配
     */
    class sq_mongo_adapter
    {
    public:
        static void db_init();

        /**
         * @param connect_info 连接串
         */
        int connect(const char *database, const char *host, const char *user, const char *pwd);
        int disconnect();
       
        /**
         * @brief 执行sql命令
         */
        int exec_sql(const char *query);
        mongocxx::client* db_=nullptr;
    };

    using sq_mongo_db = sq_mongo_adapter;
}
#endif