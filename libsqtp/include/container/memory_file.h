#pragma once
#include "os/sq_shm.h"
namespace sq
{
	class memory_file
	{
	public:
		memory_file();
		~memory_file();
		int open(const char*name, int capactiy);

		int write(char*msg, int len);
		int seek(int pos, int where);
		int read(char*buf, int len);
		void close();
	private:
		sq_shm_maping* m_map;
		int m_capacity = 0;
		int m_cursor = 0;
		//int m_r_cursor = 0;
		char* m_base_addr = nullptr;
		
	};
}