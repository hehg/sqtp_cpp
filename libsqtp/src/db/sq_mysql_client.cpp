#include "db/sq_mysql_client.h"
#include "sq_define.h"
#if SUPPORT_MYSQL_DB==1
#include "mysql/mysql.h"
namespace sq
{
	void sq_mysql_client::init_mysql()
	{
		if (mysql_library_init(0, nullptr, nullptr)) {
			printf("init mysql lib error\n");
		}
	}
	void sq_mysql_client::finish_mysql()
	{
		mysql_library_end();
	}


	//===================================
	sq_mysql_client::mysql_result::mysql_result()
	{

	}
	sq_mysql_client::mysql_result::~mysql_result()
	{
		if (m_result)
		{
			mysql_free_result((MYSQL_RES*)m_result);
		}
	}
	bool sq_mysql_client::mysql_result::next()
	{
		MYSQL_ROW row;
		row = mysql_fetch_row((MYSQL_RES*)m_result);
		if (row) 
		{
			unsigned long *lengths;
			lengths = mysql_fetch_lengths((MYSQL_RES*)m_result);
			for (int i = 0; i < m_num_fields; i++)
			{
				m_fiels_len.push_back((int)lengths[i]);
				if (row[i] != nullptr)
					m_files_value.push_back(row[i]);
				else
					m_files_value.push_back("NULL");
				printf("len=%d [%s] ", (int)lengths[i], row[i] ? row[i] : "NULL");
			}
			printf("\n");
			return true;
		}
		else {
			return false;
		}
	}
	//======================================
	sq_mysql_client::sq_mysql_client()
	{
		m_connector = new MYSQL();
    }

	int  sq_mysql_client::open(const char* host_name, const char* user, const char* passwd,
		const char* db_name,int port)
	{
		MYSQL *mysql = mysql_init((MYSQL*)m_connector);

		m_connector =mysql_real_connect(mysql, host_name, user, passwd, db_name, port, nullptr, 0);
		if (m_connector == nullptr) {
			printf("connect mysql error code=%d,msg=%s\n", mysql_errno(mysql), mysql_error(mysql));
			return 1;
		}
		return 0;

	}
	int sq_mysql_client::excute(const char* cmd)
	{
		try
		{
			if (0 != mysql_query((MYSQL*)m_connector, cmd))
			{
				printf("mysql error:%s\n",mysql_error((MYSQL*)m_connector));
				return 1;
			}
		}
		catch (std::exception &e)
		{
			printf("exception:%s\n", e.what());
			return 1;
		}
		catch (...)
		{
			printf("Unknown exception:\n");
			return 1;
		}

		if (mysql_field_count((MYSQL*)m_connector) == 0)
		{
		
			int num_rows = mysql_affected_rows((MYSQL*)m_connector);
			return num_rows > 0 ? 0 : 1;
		}
		else // mysql_store_result() should have returned data
		{
			fprintf(stderr, "Error: %s\n", mysql_error((MYSQL*)m_connector));
			return 1;
		}
		return 0;
	}

	sq_mysql_client::mysql_result* sq_mysql_client::select(const char*  query)
	{
		try
		{
			if (0 != mysql_query((MYSQL*)m_connector, query))
			{
				printf("mysql error:%s\n", mysql_error((MYSQL*)m_connector));
				return  nullptr;
			}
		}
		catch (std::exception &e)
		{
			printf("exception:%s\n", e.what());
			return  nullptr;
		}
		catch (...)
		{
			printf("Unknown exception:\n");
			return  nullptr;
		}
		mysql_result *result = new mysql_result();
		
		
		//unsigned int num_fields;

		result->m_result = mysql_store_result((MYSQL*)m_connector);
		result->m_num_fields = mysql_num_fields((MYSQL_RES*)result->m_result);

		MYSQL_FIELD *field;
		while ((field = mysql_fetch_field((MYSQL_RES*)result->m_result)))
		{
			result->m_files_name.push_back(field->name);
		}

		if (result->m_result) // there are rows
		{
			return result;
		}
		else {
			return nullptr;
		}
	}
	void sq_mysql_client::close() {
		
		mysql_close((MYSQL*)(m_connector));
		m_connector = nullptr;
	}
}
#endif