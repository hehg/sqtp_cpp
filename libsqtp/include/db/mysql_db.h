#pragma once
#include "sq_define.h"
#if 0
#if SUPPORT_MYSQL_DB==1
#define OTL_ODBC_MYSQL
#include "otlv4.h"

namespace sq
{

	class mysql_db
	{
	public:

		static void init_otl(){ otl_connect::otl_initialize(); }
		mysql_db();
		~mysql_db();
		bool connect_db(string instance,string user, string passwd, 
			string host_name="127.0.0.1",int port=3306);
        
		otl_connect&get_db() { return m_db; }
        void insert(string&json);
	private:
		string m_host;
		int    m_host_port;
		string m_user;
		string m_passwd;
		otl_connect m_db;
	};


	
}
#endif

#endif