#pragma once
#include <assert.h>
#include <string.h>
/*
*/
class byte_buffer
{
public:
	byte_buffer() {
		m_data = nullptr;
		m_cap = 0;
		m_pos = 0;
		m_need_free = false;
	}
	byte_buffer(int size) {
		m_data = new char[size];
		m_cap = size;
		m_pos = 0;
		m_need_free = true;
	}

	byte_buffer(char*buf, int size) {
		m_data = buf;
		m_cap = size;
		m_pos = 0;
		m_need_free = false;
	}
	void init(char*buf, int size) {
		m_data = buf;
		m_cap = size;
		m_pos = 0;
		m_need_free = false;
	}
	~byte_buffer() {
		if (m_need_free)
			delete[] m_data;
		m_pos = 0;
	}
	void reset() {
		m_pos = 0;
	}

	char* alloc_buf(int size) {
		if (m_pos + size > m_cap)
			return nullptr;
		char* buf = m_data + m_pos;
		m_pos += size;
		return buf;
	}

	template<typename T>
	int write(T val) {
		if (m_pos + sizeof(T) > m_cap)
			return 0;
		char* buf = m_data + m_pos;
		memcpy(buf, &val, sizeof(T));
		m_pos += sizeof(T);
		return sizeof(T);
	}

	template <typename T>
	T* read()
	{
		char*cur=get_cur();
		if (m_pos + sizeof(T) > m_cap)
			return nullptr;
		m_pos += sizeof(T);
		return (T*)cur;
	}

	char* read(int size) {
		char*cur = get_cur();
		if (m_pos + size > m_cap)
			return nullptr;
		m_pos += size;
		return cur;
	}

	int size() { return m_pos; }
	char* get_cur() { return m_data + m_pos; }
	char *m_data;
	int   m_cap;
	int   m_pos;
	bool  m_need_free;
};