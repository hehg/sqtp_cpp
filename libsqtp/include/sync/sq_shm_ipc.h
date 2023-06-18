/** \file
\brief 基于共享内存的信息共享,用来实现进程间的共享内存通讯
*/
#pragma once
#include <string>
#include <map>
#include "os/sq_shm.h"
using namespace std;

namespace sq
{
    template<typename T>
    class sq_shm_struct 
    {
    public:
        //void open();
    };
#if 0
#pragma pack(push,1)

	typedef struct{
		int total_size;
		int block_count;
		int block_size;
		int read_cursor;
		int write_cursor;
		
	}sq_shm_data_meta;

	typedef struct{
		int seq;		//序号号
		int data_len;	//数据区长度
	}sq_shm_data_key;
#pragma pack(pop)
	class sq_shm_ipc
	{
	public:
		sq_shm_ipc() {
			m_data_vector = nullptr;
		}
		~sq_shm_ipc(){ if (m_data_vector) { delete[] m_data_vector; } };
		bool  open(const char* path, int block_size, int block_count);

		bool push_back(char*msg, int len);
		char*front(int &length);
		void pop_front();

	private:
		bool  create(const char*path,int block_size,int block_count);
		inline bool full(){ return (m_meta->write_cursor - m_meta->read_cursor) == m_meta->block_count; };
		inline bool empty(){ return m_meta->read_cursor == m_meta->write_cursor; };
		
	private:
		sq_shm    m_shm;
		sq_shm_data_meta* m_meta;
		char*     m_data_base;
		char*     m_base;
		char**     m_data_vector;
	};
#endif
}



