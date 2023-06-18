#pragma once
#include "sq_define.h"
#include "sq_types.h"
namespace sq
{
    class  sq_mem_mappable
    {
    public:
        virtual int open(string path, uint64_t size) { return 0; };
       
        virtual void close()=0;
        int	m_fd = 0;
        uint64_t m_size;
        void*   m_base_addr=nullptr;
        string m_path;
#ifdef WINDOWS
        HANDLE m_file_hand;
        HANDLE m_map_hand;
#else
        
#endif

    };
    //文件mapping
    class sq_file_maping:public sq_mem_mappable
    {
      public:
          sq_file_maping() = default;
          virtual int open(string path, uint64_t size);
          virtual void close();
		  void* map_addr();
    };
    //共享内存mapping
    class sq_shm_maping :public sq_mem_mappable
    {
        public:
            sq_shm_maping() = default;
            virtual int open(string name, uint64_t size);
           void* map_addr();
            virtual void close();
    };

}