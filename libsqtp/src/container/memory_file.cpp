#include "container/memory_file.h"

namespace sq
{
	memory_file::memory_file():
		m_map(nullptr)
	{
	}
	memory_file::~memory_file()
	{
		if (m_map) {
			m_map->close();
			delete m_map;
		}
	}
	int memory_file::open(const char*name, int len)
	{
		m_map = new sq_shm_maping();
		int ret=m_map->open(name, len);
		if (ret != 0) {
			assert(false);
			return ret;
		}
		m_base_addr = (char*)m_map->m_base_addr;
		m_capacity = len;
		m_cursor = 0;
		return 0;
	}
	int memory_file::write(char*msg, int len)
	{
		if (m_cursor + len > m_capacity)
			return -1;

		char* cur = m_base_addr + m_cursor;
		memcpy(cur, msg, len);
		m_cursor += len;

		return len;
	}
	int memory_file::read(char*buf, int len)
	{
		if (m_cursor + len > m_cursor)
			return -1;

		char* cur = m_base_addr + m_cursor;
		memcpy(buf, cur, len);
		m_cursor += len;
		return len;
	}
	void memory_file::close()
	{
		m_map->close();
		delete m_map;
		m_map = nullptr;
	}
	int memory_file::seek(int pos, int where)
	{
		//begin
		if (where == 0) {
			m_cursor = pos;
		}
		//cur
		else if (where == 1) {
			m_cursor += pos;
		}
		//end
		else if (where == 2) {
			m_cursor = m_capacity - pos;
		}

		return 0;
	}
}