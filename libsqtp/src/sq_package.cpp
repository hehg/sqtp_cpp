#include "pack/sq_package.h"
namespace sq
{
	sq_package_writer::sq_package_writer(int tid)
	{
		m_buffer = new byte_buffer(4096);
		char*data = m_buffer->alloc_buf(sizeof(sq_pack_header));
		sq_pack_header*hdr = new(data)sq_pack_header();
		hdr->len = m_buffer->size();
		hdr->tid = tid;
	}
	sq_package_writer::~sq_package_writer()
	{
        delete m_buffer;
	}
	void sq_package_writer::reset()
	{
		m_buffer->m_pos = sizeof(sq_pack_header);
		sq_pack_header*hdr = get_header();
		hdr->tid = 0;
		hdr->len = sizeof(sq_pack_header);
		hdr->type = 'b';
		hdr->topic = 0;
	}
	//============================================
	bool sq_package_reader::load(char*msg, int size)
	{
		m_buffer->init(msg, size);
		m_header = m_buffer->read<sq_pack_header>();
		if (m_header == nullptr) {
			return false;
		}

		if (m_header->len > size) {
			return false;
		}

		m_body = m_buffer->get_cur();
		int body_len = m_header->len - sizeof(sq_pack_header);
		int read_len = 0;
		while (true) {
			field_info info;
			info.fid = *m_buffer->read<unsigned short>();
			info.size = *m_buffer->read<unsigned short>();
			info.data = m_buffer->get_cur();

			m_field_list.emplace_back(info);
			char*data=m_buffer->read(info.size);
			if (data == nullptr)
				break;
			read_len = read_len + sizeof(int) * 2 + info.size;
			if (read_len >= body_len)
				break;
		}
		
		return true;
	}
	//============================= sq_package_js_writer =======

	void sq_package_js_writer::set_topic(uint32_t t) {
		m_header->topic=t;
	}
	void sq_package_js_writer::add_string(const char*ss)
	{
		int len= strlen(ss);
		strcpy(m_pt, ss);

		m_pt += len;
		m_header->len += len;
		
	}
}
