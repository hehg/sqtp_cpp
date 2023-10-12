/**
 * @brief
*/
#include "db/db_adapter/sqlite/sq_sqlite_adapter.h"
#include "error/sq_err.h"
namespace sq
{
    template <typename db_t>
    class sq_orm{
        public:
        sq_orm(){}
        ~sq_orm(){}

        int connect(const char*database,const char*host,const char*user,const char*pwd){
            return m_db.connect(database,host,user,pwd);
        }
        int disconnect(){
            return m_db.disconnect();
        }
        template<typename table_t>
        int create_table()
        {
            struct_reflect& table_des=table_t::get_desc();
           // m_db.create_table(table_des);
            return  ok;
        }
        template<typename table_t>
        int drop_table(){
            struct_reflect& table_des=table_t::get_desc();
          //  m_db.drop_table(table_des.m_name.c_str());
            return  ok;
        }
        // template<typename table_t>
        // int insert(table_t*obj){
        //     struct_reflect& table_des=table_t::get_desc();
        //     return m_db.insert(table_des,obj);
        // }
        db_t  m_db;
    };
}