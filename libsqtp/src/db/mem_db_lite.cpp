#include "db/mem_db_lite.h"
#include "db/SQLiteWrapper.h"
#if SUPPORT_SQLITE
namespace sq
{
	mem_db_lite::mem_db_lite()
	{
		sqlite = new SQLiteWrapper();
	}

	mem_db_lite::~mem_db_lite()
	{
		if(sqlite!=nullptr)
		   delete sqlite;
	}
	bool mem_db_lite::open(const char*name)
	{
		m_name = name;
		return sqlite->Open(m_name);
	}
	void mem_db_lite::close()
	{
		delete sqlite;
		sqlite = nullptr;
	}
	void mem_db_lite::create_table(const char *sql)
	{
		sqlite->DirectStatement(sql);
	}
	void mem_db_lite::clear_table(const char*tb_name)
	{
		char cmd[128];
		sprintf(cmd, "delete from  %s", tb_name);
		sqlite->DirectStatement(cmd);
	}
	
}

#endif