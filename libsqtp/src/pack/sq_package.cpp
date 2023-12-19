#include "pack/sq_package.h"
#include "sq_macro.h"
namespace sq
{
	sq_package::sq_package()
	{
	}
	sq_package::~sq_package()
	{
		safe_delete(m_buffer);
	}
	std::shared_ptr<sq_package> sq_package::create(uint16_t tid, uint32_t topic, int size)
	{
		std::shared_ptr<sq_package> p=std::shared_ptr<sq_package>(new sq_package());
		p->m_buffer = new byte_buffer(size);
		char *data = p->m_buffer->alloc_buf(sizeof(sq_pack_header));
		sq_pack_header *hdr = (sq_pack_header *)data;
		hdr->len = size;
		hdr->tid = tid;
		hdr->topic = topic;

		return p;
	}
	/**
	 * @brief 从已有数据创建一个 package
	*/
	std::shared_ptr<sq_package> sq_package::create(char *msg, int size)
	{
		std::shared_ptr<sq_package> p=std::shared_ptr<sq_package>(new sq_package());
		p->m_buffer = new byte_buffer(msg,size);
		p->m_buffer->init(msg, size);
		//读取包头
		sq_pack_header *header = p->m_buffer->read<sq_pack_header>();
		if(!header ||!header->is_valid()){
			return nullptr;
		}
		if (header->len > size)
		{
			return nullptr;
		}
		//读取包体
		char *body = p->m_buffer->get_cur();
		if(!body){
			return nullptr;
		}
		int body_len = header->len - sizeof(sq_pack_header);
		int read_len = 0;
		//遍历所有域
		while (true)
		{
			//解析域头
			field_info info;
			info.fid = *p->m_buffer->read<unsigned short>();
			info.size = *p->m_buffer->read<unsigned short>();
			info.data = p->m_buffer->get_cur();
            ///解析域体
			char *data = p->m_buffer->read(info.size);
			if (data == nullptr)
				break;
			p->m_field_list.emplace_back(info);

			read_len = read_len + sizeof(int) * 2 + info.size;
			if (read_len >= body_len)
				break;
		}
		return p;
	}
	

	char *sq_package::alloc_field(unsigned short fid, unsigned short size)
	{
		m_buffer->write(fid);
		m_buffer->write(size);
		char *d = m_buffer->alloc_buf(size);
		get_header()->len = m_buffer->size();
		// 加入域队列
		field_info fld;
		fld.data = d;
		fld.fid = fid;
		fld.size = size;
		m_field_list.emplace_back(fld);
		return d;
	}
	field_info *sq_package::get_field_info(int fid)
	{
		auto it = m_field_list.begin();
		for (; it != m_field_list.end(); ++it)
		{
			if (it->fid == fid)
			{
				return &(*it);
			}
		}
		return nullptr;
	}

	//============================= sq_package_js_writer =======

	// void sq_package_js_writer::set_topic(uint32_t t)
	// {
	// 	m_header->topic = t;
	// }
	// void sq_package_js_writer::add_string(const char *ss)
	// {
	// 	int len = strlen(ss);
	// 	strcpy(m_pt, ss);

	// 	m_pt += len;
	// 	m_header->len += len;
	// }
}
