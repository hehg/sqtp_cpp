#pragma once
#include <stdint.h>
#include <memory.h>
#include <vector>
#include "container/byte_buffer.h"

namespace sq
{
#pragma pack(push ,1)
	//const int topic_size = 8;
	class sq_pack_header
	{
	public:
		sq_pack_header() {
			len = 0;
			tid = 0;
			topic = 0;
		}
		unsigned short	len = 0;//包长度，包括包头
		unsigned short	tid = 0;
		char		    type = 'b'; //'b--二进制  s--字符串 j--json
		uint32_t		topic;
		static int size() { return sizeof(sq_pack_header); }
	};

#pragma pack(pop)
	//域
	struct field_info
	{
		unsigned short fid;
		unsigned short size;
		char* data;
	};
	//2进制格式，支持多域
	class sq_package_writer
	{
	public:
		sq_package_writer(int tid);
		~sq_package_writer();

		sq_pack_header* get_header() { return (sq_pack_header*)m_buffer->m_data; }
		char* get_body() { return m_buffer->m_data+sizeof(sq_pack_header); }
		char* get_data() { return m_buffer->m_data; }
		uint16_t   get_size() { return m_buffer->size(); }

		void reset();

		void set_topic(uint32_t t) {
			sq_pack_header*hdr = get_header();
			hdr->topic=t;
		}
        template<typename T>
        T* alloc_field(int fid=0)
        {
			return (T*)alloc_field(fid, sizeof(T));
        }
        char* alloc_field(unsigned short fid,unsigned short size){
			m_buffer->write(fid);
			m_buffer->write(size);
			char*d= m_buffer->alloc_buf(size);
			get_header()->len = m_buffer->size();
			return d;
        }
	private:
		byte_buffer*    m_buffer;
	};

	class sq_package_reader
	{
	public:
		sq_package_reader():m_header(nullptr)
		{  
			m_buffer = new byte_buffer();
		}
		~sq_package_reader() {
			if (m_buffer) {
				delete m_buffer;
			}
        }
		/**
		 * @param msg  数据
		 * @param size  数据大小
		 * @return true 成功 否则失败
		 */
		bool load(char*msg, int size);

        template<typename T>
        T* get_field(int fid=0)
        {
			auto it = m_field_list.begin();
			for (; it != m_field_list.end(); ++it) {
				if (it->fid == fid) {
					return (T*)it->data;
				}
			}
			return nullptr;
           
        }
		field_info* get_field_info(int fid) {
			auto it = m_field_list.begin();
			for (; it != m_field_list.end(); ++it) {
				if (it->fid == fid) {
					return &(*it);
				}
			}
			return nullptr;
		}
		
        void* get_data() { return m_header; }
		void* get_body() { return m_body; }
		int32_t get_tid() { return m_header->tid; }
        int32_t get_size() { return m_header->len; }
		sq_pack_header*get_hdr() { return m_header; }
		std::vector<field_info>  m_field_list;
		sq_pack_header* m_header;
		char* m_body;
		byte_buffer* m_buffer;
	};


	class sq_package_js_writer
	{
	public:
		sq_package_js_writer() {
			m_data = new char[4096];
		};
		~sq_package_js_writer() {
			delete[]m_data;
		};

		void  init(int32_t tid)
		{
			m_header = new(m_data)sq_pack_header();
			//m_header->init();
			m_header->tid = tid;
			m_header->type = 'j';
			m_header->len = sq_pack_header::size();
			m_body = m_data + sq_pack_header::size();
			m_pt = m_body;
		}

		sq_pack_header* get_header() { return m_header; }
		char* get_body() { return m_body; }
		char* get_data() { return m_data; }
		uint16_t   get_size() { return m_header->len; }

		void add_string(const char*ss);
		void set_topic(uint32_t t);
		
	private:
		char* m_pt;
		int  m_type;
		sq_pack_header* m_header;
		char* m_body;
		char *m_data;

	

	};
}