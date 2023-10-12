/**
 * @brief sqlite 封装 基于otl
 */
#if defined(SUPPORT_DB_SQLITE)
#include "db/db_adapter/sqlite/sq_sqlite_adapter.h"
#include "db/db_adapter/sqlite/sqlite3.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "text/sq_string.h"

namespace sq
{
    void sq_sqlite_adapter::db_init()
    {

    }
    static int callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
    {
        db_result_table*result=(db_result_table*)p_data;
        //记录列名字
        if(result->col_names_.empty()){
            for(int i=0;i<num_fields;i++){
                result->col_names_.push_back(p_col_names[i]);   
            }
        }   

        db_result_row row;
        for(int i=0;i<num_fields;i++){
            row.fields_.push_back(p_fields[i]);
        }
        result->rows_.push_back(row);

        return 0;
    }
    void sq_sqlite_adapter::on_db_error(const char *sql, int err_code)
    {
        const char *err_msg = sqlite3_errmsg(m_db);
        if (err_msg)
        {
            log_error("exec sqlite3 error,sql={},err_code={},err_msg={}\n", 
            sql,
            err_code, err_msg);
        }
        else
        {
            log_error("exec sqlite3 error,sql={},err_code={},err_msg={}\n",
            sql,
             err_code, "unkown");
        }
    }
    
    int sq_sqlite_adapter::connect(const char *database, const char *host,
                                   const char *user, const char *pwd)
    {
        sqlite3_open(database, &m_db);
        int ret = sqlite3_errcode(m_db);
        if (ret != SQLITE_OK)
        {
            sq_panic("open sqlite db error\n");
            return ret;
        }
        return ok;
    }
    int sq_sqlite_adapter::disconnect()
    {
        if (m_db)
        {
            sqlite3_close(m_db);
        }
        m_db = nullptr;

        return ok;
    }
    int sq_sqlite_adapter::exec_sql(const char *sql,db_result_func_t cb)
    {
        char *err_msg=nullptr;
        db_result_table result;
        int ret = sqlite3_exec(m_db, sql, callback, &result, &err_msg);
        if (ret != SQLITE_OK)
        {
            on_db_error(sql, ret);
            ret= err_sql_exec;
        }
        else{
            ret=ok;
        }
        if(err_msg)
        {
            sqlite3_free(err_msg);
        }
        if(cb)
        {
            cb(std::move(result));
        }
        return ret;
    }
    int sq_sqlite_adapter::begin_trans()
    {
        return exec_sql("begin;",nullptr);
    }
    int sq_sqlite_adapter::commit()
    {
        return exec_sql("commit;",nullptr);
    }
    int sq_sqlite_adapter::rollback()
    {
        return exec_sql("rollback;",nullptr);
    }
    int sq_sqlite_adapter::exec_sql(const char *sql, std::vector<db_bind_info> &params,
                                    db_result_func_t cb)
    {
        std::shared_ptr<sqlite3_stmt> stmt_ptr = nullptr;
        int ret = SQLITE_OK;
        // get stmt
        auto it = m_stmt_cache.find(sql);
        if (it != m_stmt_cache.end())
        {
            stmt_ptr = it->second;
        }
        else
        {
            // create new stmt
            sqlite3_stmt *stmt = nullptr;
            const char *remaining;
            ret = sqlite3_prepare_v2(m_db, sql, -1, &stmt, &remaining);
            if (ret != SQLITE_OK || !stmt)
            {
                on_db_error(sql, ret);
                return err_sql_exec;
            }
            stmt_ptr = std::shared_ptr<sqlite3_stmt>(stmt,
                                                     [](sqlite3_stmt *p)
                                                     {
                                                         sqlite3_finalize(p);
                                                     });
        }

        // bind param
        sqlite3_stmt *stmt = stmt_ptr.get();

        for (int i = 0; i < (int)params.size(); ++i)
        {
            field_type_t type = params[i].type;
            if (type == field_type_t::type_int64 || type == field_type_t::type_uint64)
            {
                ret = sqlite3_bind_int64(stmt, i + 1, params[i].bind_data.int64_data);
            }
            else if (type > field_type_t::type_number_begin && type < field_type_t::type_number_end)
            {
                ret = sqlite3_bind_int(stmt, i + 1, params[i].bind_data.int_data);
            }
            else if (type > field_type_t::type_real_begin && type < field_type_t::type_real_end)
            {
                ret = sqlite3_bind_double(stmt, i + 1, params[i].bind_data.double_data);
            }
            else if (type > field_type_t::type_text_begin && type < field_type_t::type_text_end)
            {
                ret=sqlite3_bind_text(stmt, i + 1, params[i].bind_data.text_data, 
                -1, SQLITE_STATIC);
            }
            else
            {
                log_fata("not support type {}\n", type);
            }

            if (ret != SQLITE_OK)
            {
                on_db_error(sql, ret);
                sqlite3_reset(stmt);

                return err_sql_exec;
            }
        }
        // 只读命令 未来通过读写锁可以分别控制
        // if (sqlite3_stmt_readonly(stmt))
        
        // 收集结果
        int column_num = sqlite3_column_count(stmt);
        db_result_table result;
        for (int i = 0; i < column_num; ++i)
        {
            auto name = std::string(sqlite3_column_name(stmt, i));
            std::transform(name.begin(),
                           name.end(),
                           name.begin(),
                           [](unsigned char c)
                           { return tolower(c); });
            result.col_names_.push_back(name);
        }

        while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            db_result_row row;
            for (int i = 0; i < column_num; ++i)
            {
                int type = sqlite3_column_type(stmt, i);
                switch (type)
                {
                case SQLITE_INTEGER:
                {
                    row.fields_.push_back(std::to_string(sqlite3_column_int64(stmt, i)));
                    break;
                }
                case SQLITE_FLOAT:
                {
                    row.fields_.push_back(std::to_string(sqlite3_column_double(stmt, i)));
                    break;
                }
                case SQLITE_TEXT:
                {
                    std::string r((const char *)sqlite3_column_text(stmt, i),
                                  (size_t)sqlite3_column_bytes(stmt, i));
                    row.fields_.push_back(r);
                    break;
                }
                case SQLITE_BLOB:
                {
                    const char *buf = (const char *)sqlite3_column_blob(stmt, i);
                    size_t len = sqlite3_column_bytes(stmt, i);
                    if (buf)
                    {
                        row.fields_.push_back(std::string(buf, len));
                    }
                    else
                    {
                        row.fields_.push_back("");
                    }
                    break;
                }

                case SQLITE_NULL:
                    row.fields_.push_back("");
                    break;
                }
            }
            result.rows_.push_back(std::move(row));
        }

        if (ret == SQLITE_DONE)
        {
            sqlite3_reset(stmt);
            m_stmt_cache[sql] = stmt_ptr;
            result.effect_rows_= sqlite3_changes(m_db);
            // callback result
            if (cb)
            {
                cb(std::move(result));
            }
            return ok;
        }
        else
        {
            on_db_error(sql, ret);
            return err_sql_exec;
        }
    }

#if 0
     int sq_sqlite_adapter::create_table(struct_reflect&table_desc)
     {
        //构造sql语句
        std::stringstream create_table_sql;
        create_table_sql<<"CREATE TABLE IF NOT EXISTS "<<table_desc.m_name<<" (";
        int count=table_desc.m_fields.size();
        std::vector<string> keys;
        //输出各个列
        for(int i=0;i<count;++i)
        {
            auto& col=table_desc.m_fields[i];
            create_table_sql<<col.m_name<<" ";
            if(col.m_class_type==field_class_t::class_text)
            {
              string ty=fmt::format("varchar({})",col.m_array_item_count);
              create_table_sql<<ty;
            }
            else{
                create_table_sql<<col.m_type_name;
            }
           
            
            if(col.m_is_primary_key){
                //create_table_sql<<" PRIMARY KEY ";
                keys.push_back(col.m_name);
            }
            
            if (i != count - 1)
            {
                create_table_sql<<",";
                
            }
        }
        // 输出主键
        if (!keys.empty())
        {
            create_table_sql << ", primary key (";
            for (int j = 0; j < keys.size(); j++)
            {
                create_table_sql << keys[j];
                if (j < keys.size() - 1)
                {
                    create_table_sql << ",";
                }
            }
            create_table_sql << ")\n";
        }
         create_table_sql<<")\n;";
         string ss=create_table_sql.str().c_str();
        log_debug("create table sql={}",ss.c_str());
        //create table
        int ret = exec_query(ss.c_str());
        return ret;
     }

     int sq_sqlite_adapter::drop_talble(const char*table_name)
     {
        std::string ss=fmt::format("drop table {}",table_name);
        int ret = exec_query(ss.c_str());
        return ret;
     }

     int sq_sqlite_adapter::insert(struct_reflect&table_desc,void*obj)
     {
        std::stringstream insert_table_sql;
        //构造sql

        std::string sql=fmt::format("replace into {} values (",table_desc.m_name);
        insert_table_sql<<sql;
        
        for(int i=0;i<table_desc.m_fields.size();i++){
            insert_table_sql<<"? ";
            if(i<table_desc.m_fields.size()-1){
                insert_table_sql<<",";
            }
        }
        insert_table_sql<<");";

        sql=insert_table_sql.str().c_str(); 
        log_debug("insert table sql={}",sql.c_str());
#if 0
        //todo 优化
        sqlite3_stmt *stmt=nullptr;
        int ret = sqlite3_prepare(m_db, sql.c_str(), -1, &stmt, 0);
        if (ret != SQLITE_OK)
        {
           return err_sql_exec;
        }
       
        for (int i = 0; i < table_desc.m_columns.size(); i++)
        {
           auto &item = table_desc.m_columns[i];
           if (item.m_type == "INTEGER")
           {
                sqlite3_bind_int64(stmt, i + 1, item.m_value)
           }
           else if(item.m_type=="REAL"){
                //sqlite3_bind_double
           }
           else if(item.m_type=="TEXT"){
                sqlite3_bind_text(stmt, i + 1,
                             value.c_str(),
                             value.length(),
                             SQLITE_STATIC);
           }
           else{
            sq_panic("not support m_type\n");
           }
           
          
        }

        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);
#endif
        return ok;
     }
#endif
}
#endif