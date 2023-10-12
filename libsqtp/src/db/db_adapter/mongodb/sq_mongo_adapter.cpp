/**
 * @brief mogondbcxx 封装 
*/
#if defined(SUPPORT_DB_MONGO)
#include "db/db_adapter/mongodb/sq_mongo_adapter.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "text/sq_string.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

namespace sq
{
   
     void sq_mongo_adapter::db_init()
    {
        mongocxx::instance instance{}; 
    }
    int sq_mongo_adapter::connect(const char *database, const char *host,
                                  const char *user, const char *pwd)
    {
        char url[128] = {0};
        sprintf(url, "mongodb://%s/%s", host,database);
        mongocxx::uri uri(url);
        db_=new mongocxx::client(uri);

        return ok;
    }
    int sq_mongo_adapter::disconnect()
    {
   
        return ok;
    }
     int sq_mongo_adapter::exec_sql(const char*query)
     {
        
        return ok;
     }
     
}
#endif 