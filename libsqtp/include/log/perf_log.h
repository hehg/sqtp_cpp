#pragma once
#include "sq_define.h"
#include <fstream>
#include "sync/sq_spin_lock.h"
namespace sq
{
    const uint32_t perf_type_inc=(1<<2); //增量统计
    //性能指标
    class perf_index
    {
       public:
         int id=0;
         string name;
         int type=0;
         int64_t val=0;
         int64_t pre_val=0;
         int64_t cur_val=0;
    };
    class perf_log
    {
        public:
        perf_log()=default;
        ~perf_log();
        void open(const char*path);
        void add(const char*key);
        void put(const char*key,int64_t val);
        void set(const char*key,uint32_t type);
        void output();
        std::vector<perf_index*> m_index_list;
        std::unordered_map<string,perf_index*> m_map_key;

        std::fstream  m_file;
        sq_spin_lock  m_lock;
    };
}