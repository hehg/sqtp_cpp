#pragma once
#include <string.h>
#include "base/sq_util.h"
namespace sq
{
	template < int64_t SIZE=40960>
	class net_cache
	{
	public:
		net_cache(){
			char*buf = (char*)sq_util::aligned_malloc(SIZE);
			m_write = buf;
			m_read = buf;
			m_head = buf;
		    m_tail = buf+SIZE;
		};
		~net_cache(){ sq_util::aligned_free(m_head); }
        void reset() {
            m_write = m_head;
            m_read = m_head;
            m_tail = m_head + SIZE;
        }
        //获取可写空间
		char* write_space(int64_t &len)
		{
			if (m_tail - m_write >= len)
			{
				len = m_tail - m_write;
				return m_write;
			}
			else
			{
                int64_t space = m_write - m_read;
				memcpy(m_head, m_read,space);
				m_read = m_head;
				m_write = m_head+space;
				len = m_tail - m_write;
				return m_write;
			}
				
		}
		void push_back(int64_t len)
		{
			m_write += len;
		}
        //获取可读空间
		char* read_space(int64_t &len){
			if (m_write - m_read < len)
				return nullptr;
			len= m_write - m_read;
			return m_read;
		}
		void pop_front(int64_t size){ m_read += size; }
	private:
		//char buf[SIZE];
		char* m_write;
		char* m_read;
		char* m_head;
		char* m_tail;
	};
}