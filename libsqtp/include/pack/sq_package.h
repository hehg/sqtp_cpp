/**
 * @brief 数据序列化，按域的方式进行
 */

#pragma once
#include <stdint.h>
#include <memory.h>
#include <memory>
#include <vector>
#include "container/byte_buffer.h"

namespace sq
{
#pragma pack(push, 1)
	/**
	 * @brief 数据包包头
	 */
	struct sq_pack_header
	{
		sq_pack_header()
		{
			len = 0;
			tid = 0;
			topic = 0;
		}
		unsigned short len = 0; // 包长度，包括包头
		unsigned short tid = 0;
		char type = 'b'; //'b--二进制  s--字符串 j--json
		uint32_t topic;
		static int size() { return sizeof(sq_pack_header); }
		bool is_valid()
		{
			return len >= sizeof(sq_pack_header) && tid > 0;
		};
	};
	struct sq_pack_field_header
	{
		unsigned short fid;
		unsigned short size;
	};

#pragma pack(pop)
	// 域头
	struct field_info
	{
		unsigned short fid;
		unsigned short size;
		char *data;
	};
	/**
	 * @brief 一个 package 有一个header+body 组成
	 * 其中 body 由各个域组成，每一个域由域头+域体组成
	 * |header|域头1|域体1|域头2|域体2|......
	 * 二进制格式的数据包,系统内部使用这个二进制数据包
	 */
	struct sq_package
	{
		using ptr=std::shared_ptr<sq_package> ;
		sq_package();
		~sq_package();
		/**
		 * 创建一个空的包
		*/
		static std::shared_ptr<sq_package> create(uint16_t tid, uint32_t topic = 0, int size = 4096);
		/**
		 * 从已有的数据中解析出包
		*/
		static std::shared_ptr<sq_package> create(char *msg, int size);

		sq_pack_header *get_header() { return (sq_pack_header *)m_buffer->m_data; }
		char *get_body() { return m_buffer->m_data + sizeof(sq_pack_header); }
		char *get_data() { return m_buffer->m_data; }
		uint16_t get_size() { return m_buffer->size(); }
		uint16_t get_body_size() { return get_size() - sizeof(sq_pack_header); }
		void set_topic(uint32_t t)
		{
			sq_pack_header *hdr = get_header();
			hdr->topic = t;
		}
		uint32_t get_topic()
		{
			sq_pack_header *hdr = get_header();
			return hdr->topic;
		}
		void set_tid(uint16_t tid)
		{
			sq_pack_header *hdr = get_header();
			hdr->tid = tid;
		}
		uint16_t get_tid()
		{
			sq_pack_header *hdr = get_header();
			return hdr->tid;
		}

		template <typename T>
		T *alloc_field(int fid = T::fid)
		{
			return (T *)alloc_field(fid, sizeof(T));
		}
		template <typename T>
		T *get_field(int fid=T::fid)
		{
			field_info *fld = get_field_info(fid);
			if (fld)
			{
				return (T *)fld->data;
			}
			else
			{
				return nullptr;
			}
		}

		char *alloc_field(unsigned short fid, unsigned short size);

		field_info *get_field_info(int fid);

		void reset(){
			if(m_buffer){
				m_buffer->reset();
			}
			m_field_list.clear();
		}

		// 每个域的信息
		std::vector<field_info> m_field_list;
		byte_buffer *m_buffer = nullptr;
	};

}