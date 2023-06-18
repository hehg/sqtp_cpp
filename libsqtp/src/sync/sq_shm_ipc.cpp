#include "sync/sq_shm_ipc.h"
#if 0
namespace sq
{
	bool   sq_shm_ipc::create(const char*path, int block_size, int block_count)
	{
		int total_size = block_count*block_size + block_count*sizeof(sq_shm_data_key)+sizeof(sq_shm_data_meta);
		
		m_base = (char*)m_shm.shm_create(path, total_size);
		if (m_base == NULL)
			return false;
		m_meta = (sq_shm_data_meta*)m_base;
		m_meta->block_count = block_count;
		m_meta->block_size = block_size;
		m_meta->read_cursor = 0;
		m_meta->write_cursor = 0;
		m_meta->total_size = total_size;

		m_data_base=(char*)m_base + sizeof(sq_shm_data_meta);
		m_data_vector = new char*[block_count];
		for (int i = 0; i < block_count; i++)
		{
			m_data_vector[i] = m_data_base + i*(block_size + sizeof(sq_shm_data_key));
		}
		return true;
	}
	bool  sq_shm_ipc::open(const char* path, int block_size, int block_count)
	{
		
		if (access(path, 0) == -1)
		{
			return create(path,block_size,block_count);
		}
			
		int total_size = block_count*block_size + block_count*sizeof(sq_shm_data_key)+sizeof(sq_shm_data_meta);
		
		m_base = (char*)m_shm.shm_open(path, total_size);
		if (!m_base)
			return false;

		m_meta = (sq_shm_data_meta*)m_base;

		assert(m_meta->block_count == block_count);
		assert(m_meta->block_size == block_size);

		m_data_base = (char*)m_base + sizeof(sq_shm_data_meta);

		m_data_vector = new char*[block_count];
		for (int i = 0; i < block_count; i++)
		{
			m_data_vector[i] = m_data_base + i*(block_size + sizeof(sq_shm_data_key));
		}
		return true;
		
	}

	bool sq_shm_ipc::push_back(char*msg, int len)
	{
		if (full())return false;
		char*data = m_data_vector[m_meta->write_cursor%m_meta->block_count];
		sq_shm_data_key*key = (sq_shm_data_key*)data;
		key->data_len = len;
		key->seq = m_meta->write_cursor;
		memcpy(data + sizeof(sq_shm_data_key), msg, len);
		m_meta->write_cursor++;
		return true;
	}
	char*sq_shm_ipc::front(int &length)
	{
		if (empty())return nullptr;
		char*data = m_data_vector[m_meta->read_cursor%m_meta->block_count];
		sq_shm_data_key*key = (sq_shm_data_key*)data;
		length = key->data_len;
		return data + sizeof(sq_shm_data_key);
	}
	void sq_shm_ipc::pop_front()
	{
		if (empty())return;
		m_meta->read_cursor++;
	}
	
	
}

#endif