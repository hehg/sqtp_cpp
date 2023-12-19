#include "db/db_adapter/mysql/sq_mysql_adapter.h"
#include "sq_define.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "net/net_address.h"

#ifdef SUPPORT_DB_MYSQL
#include <mysql.h>
namespace sq
{
	void sq_mysql_adapter::db_init()
	{
		if (mysql_library_init(0, nullptr, nullptr)) {
			printf("init mysql lib error\n");
		}
	}
	void sq_mysql_adapter::db_uninit()
	{
		mysql_library_end();
	}

	sq_mysql_adapter::sq_mysql_adapter()
	{
		m_db = new MYSQL();
    }
	sq_mysql_adapter::~sq_mysql_adapter()
	{
		delete (MYSQL*)m_db;
	}
	int sq_mysql_adapter::connect(const char *database, const char *host, const char *user, const char *pwd)
	{
		net_address addr;
		if(addr.parse_ipv4(host)==false)
		{
			log_error("sq_mysql_adapter::connect parse host address error\n");
			return err_db_connect;
		}

		MYSQL *mysql = mysql_init((MYSQL *)m_db);
		int port = addr.m_port;

		MYSQL *conn = mysql_real_connect(mysql, addr.m_ip.c_str(), user, pwd, database, port, nullptr, 0);
		if (conn == nullptr)
		{
			log_error("connect mysql error code={},msg={}\n", mysql_errno(mysql), mysql_error(mysql));
			return err_db_connect;
		}
		m_connector = conn;
		return ok;
	}
	int sq_mysql_adapter::exec_sql(const char*sql,db_result_func_t cb)
	{
		try
		{
			if (0 != mysql_query((MYSQL*)m_connector, sql))
			{
				log_error("mysql exec error,msg={}\n",mysql_error((MYSQL*)m_connector));
				return err_sql_exec;
			}
		}
		catch (std::exception &e)
		{
			log_error("mysql exec error,msg={}\n", e.what());
			return err_sql_exec;
		}
		catch (...)
		{
			log_error("Unknown exception:\n");
			return err_sql_exec;
		}
		db_result_table result;
		MYSQL_RES*res=mysql_store_result((MYSQL*)m_connector);
		if (res)
		{
			MYSQL_FIELD *field;

			while ((field = mysql_fetch_field(res)))
			{
				// printf("field name %s\n", field->name);
				auto name = std::string(field->name);
				std::transform(name.begin(),
							   name.end(),
							   name.begin(),
							   [](unsigned char c)
							   { return tolower(c); });
				result.col_names_.push_back(name);
			}
			int filed_count=mysql_field_count((MYSQL*)m_connector);
			//no result set
			if (filed_count == 0)
			{
				result.effect_rows_=0;
				
			}
			else{
				result.effect_rows_= mysql_affected_rows((MYSQL*)m_connector);
			}
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res)) != NULL)
			{
				db_result_row result_row;
				for(int i=0;i<filed_count;++i)
				{
					result_row.fields_.push_back(row[i]==nullptr?std::string(""):row[i]);
				}
				result.rows_.push_back(result_row);
			}
			mysql_free_result(res);
		}
		else{
			if(mysql_errno((MYSQL*)m_connector)!=0){
				log_error("mysql exec error,msg={}\n",mysql_error((MYSQL*)m_connector));
				return err_sql_exec;
			}
			else{
				result.effect_rows_=0;
			}
		}

		if (cb)
		{
			cb(std::move(result));
		}

		return ok;
	}
	int sq_mysql_adapter::begin_trans()
    {
        return exec_sql("begin;",nullptr);
    }
    int sq_mysql_adapter::commit()
    {
        return exec_sql("commit;",nullptr);
    }
    int sq_mysql_adapter::rollback()
    {
        return exec_sql("rollback;",nullptr);
    }
	int sq_mysql_adapter::disconnect() {
		
		mysql_close((MYSQL*)(m_db));
		m_db = nullptr;
		return ok;
	}
}
#endif