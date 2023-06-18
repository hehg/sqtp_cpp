#pragma once
#include <string>
#include "sq_define.h"
#if SUPPORT_SQLITE
#include "SQLiteWrapper.h"
using namespace std;
namespace sq
{
	/**
	@brief 基于sqlite的内存数据库
	*/
	class mem_db_lite
	{
	public:
		mem_db_lite();
		~mem_db_lite();
		//bool create(const char*name = ":memory:");
		bool open(const char*name = "./sqlite.db");
		void close();

		void create_table(const char *sql);
		void clear_table(const char*tb_name);
		SQLiteWrapper* sqlite;
		
		string m_name;

	};
}
#endif