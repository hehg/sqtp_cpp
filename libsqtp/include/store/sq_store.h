/**数据流
数据流由2个文件组成，一个文件用了记录事件日志，利用这个文件可以反演
索引
一个文件记录数据
*/

#pragma once
#include "sq_define.h"

#define STORE_VER '2'
#define MAX_FILE_COUNT 999
#define MAX_FILE_SIZE 524288000
#define MAX_BLOCK_SIZE 4096
#define LOG_CHURK_SIZE 32

#define CMD_TYPE_HDR 'h'
#define CMD_TYPE_ADD_BLOCK 'a'
#define CMD_TYPE_ADD_FILE 'b'

namespace sq
{
#pragma pack(push,1)
    //日志文件头
    struct store_hdr {
        char    type = 'h';
        char ver='1';
      //  char pre_fix[16];
    };
    //事件记录，增加了一个数据块
    struct add_block_info {
        char    type='a';
        int32_t id; //数据块编号，全局唯一
        int32_t file_no; //文件编号
        int64_t file_offset; //在这个文件中的偏移
        int32_t size;  //数据块大小
    };
    //事件记录，增加了一个文件
    struct add_file_info {
        char    type = 'b';
        char    prefix[30]; //filename
    };
#pragma pack(pop)
    struct store_config {
        string dir="./";
        string prefix="sq";
		int32_t reverse_size = 10000;
    };

    
    class sq_store_reader;
    class sq_store
    {
    public:
        virtual int32_t open(store_config&config)=0;
        virtual void    close()=0;

        virtual int32_t put(void*value,int32_t size)=0;
        virtual int32_t get(int32_t key, void*buf, int32_t buf_size)=0;
		virtual void flush() {};
        virtual int32_t get_count() = 0;
        /**注册一个reader
        @return reader编号
        */
        int add_reader(sq_store_reader*r);
        /**删除reader
        */
        void remove_reader(int no);

        void notiy_new_data(int32_t id, void*data, int size);
    private:
        std::unordered_map<int,sq_store_reader*> m_readers;//读者队列
        int m_reader_no = 0;
    };

    /** 同步
    */
    class sq_sync_store:public sq_store
    {
    public:
        sq_sync_store();
        ~sq_sync_store();
        virtual int32_t open(store_config&config);
        virtual void    close();

        virtual int32_t put(void*value, int32_t size);
		/**
		@return 数据包长度
		*/
        virtual int32_t get(int32_t key,void*buf,int32_t buf_size);

		void flush();
        int32_t get_count() { return m_block_no; }
    private:
        FILE* get_file(int no) { return m_file_list[no]; }

        bool  cmd_add_file(add_file_info* info,bool to_disk);
        bool  cmd_add_block(add_block_info*idx_info,bool to_disk=true);
        bool  check_hdr(store_hdr*hdr);

        store_config m_config;
        FILE*  m_cur_file = nullptr;
        FILE*  m_idx_file = nullptr;

        vector<FILE*>  m_file_list;

        int32_t m_block_no =0;
        int32_t m_file_no = -1;
        int64_t m_write_offset = 0;

        char   m_cmd_buf[LOG_CHURK_SIZE];
		std::vector<add_block_info> m_index_map;

    };
}