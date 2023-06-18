#include "store/sq_store.h"
#include "os/sq_path.h"
#include "error/sq_err.h"
namespace sq
{
    sq_sync_store::sq_sync_store()
    {
		
    }
    sq_sync_store::~sq_sync_store()
    {
       
    }
    int32_t sq_sync_store::open(store_config&config)
    {
        m_config = config;
		if(m_config.reverse_size>0)
			m_index_map.reserve(m_config.reverse_size);
        if (sq_path::exist(m_config.dir.c_str()) == false)
        {
			printf("not exist store folder\n");
            return err_not_exist;
        }
#define COUNT 100000
        char* read_buf=new char[COUNT*LOG_CHURK_SIZE];
        string file_path = m_config.dir + m_config.prefix + "_index.idx";

        if (sq_path::exist(file_path.c_str()) == true) {
            //reopen
            m_idx_file = fopen(file_path.c_str(), "a+b");
           
            while (true) {
                int ret= fread(read_buf, LOG_CHURK_SIZE, COUNT, m_idx_file);
                if (ret>0) 
				{
					for (int i = 0; i < ret; i++) {
						char* buf = read_buf + i*LOG_CHURK_SIZE;
						if (*buf == CMD_TYPE_ADD_BLOCK) {
							add_block_info*p = (add_block_info*)buf;
							cmd_add_block(p, false);
						}
						else if (*buf == CMD_TYPE_ADD_FILE) {
							add_file_info*p = (add_file_info*)buf;
							cmd_add_file(p, false);
						}
						else if (*buf == CMD_TYPE_HDR) {
							check_hdr((store_hdr*)buf);
						}
					}
                }
                else {
					break;
                }
               
            }
        }
        else {

            m_idx_file = fopen(file_path.c_str(), "a+b");
            //写头
            store_hdr*hdr = (store_hdr*)read_buf;
            hdr->type = CMD_TYPE_HDR;
            hdr->ver = STORE_VER;
            fwrite(hdr, LOG_CHURK_SIZE, 1, m_idx_file);

            //创建第一个文件
            add_file_info*file_info = (add_file_info*)read_buf;
            file_info->type = CMD_TYPE_ADD_FILE;
            strncpy(file_info->prefix, m_config.prefix.c_str(), sizeof(file_info->prefix));
            cmd_add_file(file_info, true);
            
        }
       
		delete[] read_buf;
        return ok;
    }
    void    sq_sync_store::close()
    {
        fclose(m_cur_file);
        fclose(m_idx_file);
        m_idx_file = nullptr;
        m_cur_file = nullptr;
    }

    int32_t sq_sync_store::put(void*value, int32_t size)
    {
        if (size > MAX_BLOCK_SIZE)
        {
            assert(false);
            return 0;
        }
		if (m_write_offset >= MAX_FILE_SIZE) {
			//创建一个文件
			add_file_info file_info;
			file_info.type = CMD_TYPE_ADD_FILE;
			strncpy(file_info.prefix, m_config.prefix.c_str(), sizeof(file_info.prefix));
			cmd_add_file(&file_info, true);

		}
        //块信息  
        add_block_info*idx_info = (add_block_info*)m_cmd_buf;
        idx_info->type = CMD_TYPE_ADD_BLOCK;
        idx_info->file_no = m_file_no;
        idx_info->file_offset = m_write_offset;
        idx_info->id = m_block_no;
        idx_info->size = size;


        //写数据
		fseek(m_cur_file, m_write_offset, SEEK_SET);
        int ret_d=fwrite((char*)value, size, 1, m_cur_file);

        bool ret=cmd_add_block(idx_info);
		if (ret) {

		}
        
		
        //通知给订阅者
        //notiy_new_data(idx_info->id, idx_info->size);

        return size;

    }
	void sq_sync_store::flush()
	{
		fflush(m_cur_file);
	}
	int32_t   sq_sync_store::get(int32_t key, void*buf, int32_t buf_size)
	{
		if (key < 0 || key >= m_index_map.size())
			return -1;

		add_block_info* it = &m_index_map[key];

		FILE*f = get_file(it->file_no);
		if (f == nullptr)
			return -1;
		fseek(f, it->file_offset, SEEK_SET);
		if (buf_size >= it->size) {
			int rsize = fread(buf, it->size, 1, f);
			if (rsize !=1) {
				assert(false);
				return -1;
			}
			return it->size;
		}
		else {
			return -1;
		}

		return it->size;

		//return 0;
	}

    bool  sq_sync_store::cmd_add_block(add_block_info*idx_info, bool to_disk )
    {
        static_assert(sizeof(add_block_info) < sizeof(m_cmd_buf),"valid size");

        if (to_disk) {
            //写索引
            int ret_idx = fwrite(idx_info, sizeof(m_cmd_buf), 1, m_idx_file);
            if (ret_idx == 1) {
				m_index_map.push_back(*idx_info);
            }
            else {
				assert(false);
                return false;
            }
        }
        else {
			m_index_map.push_back(*idx_info);
        }
       

        m_block_no++;
        m_write_offset += idx_info->size;

        return true;
    }

    bool  sq_sync_store::cmd_add_file(add_file_info* info, bool to_disk)
    {
		m_file_no++;
        char file_name_buf[512] = { 0 };
        sprintf(file_name_buf, "%s%s_%03d.data", m_config.dir.c_str(), info->prefix, m_file_no);
        m_cur_file = fopen(file_name_buf, "a+b");

        //加入文件列表
        m_file_list.push_back(m_cur_file);
        if (to_disk) {
            fwrite(info, LOG_CHURK_SIZE, 1, m_idx_file);
        }
        m_write_offset = 0;
      

        return true;
    }
    bool  sq_sync_store::check_hdr(store_hdr*hdr)
    {
        return true;
    }
    /*int32_t sq_sync_store::write_cmd_log(void*cmd)
    {
       return fwrite(cmd, LOG_CHURK_SIZE, 1, m_idx_file);
    }*/
}