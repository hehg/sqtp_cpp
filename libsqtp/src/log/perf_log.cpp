#include "log/perf_log.h"
#include "time/date_time.h"
namespace sq
{
    perf_log::~perf_log()
    {
        auto it = m_index_list.begin();
        for (; it != m_index_list.end(); ++it)
        {
            delete *it;
        }
        m_index_list.clear();
        m_map_key.clear();
    }
    void perf_log::open(const char *path)
    {
        m_file.open(path,ios::out|ios::trunc);
        if(!m_file.is_open()){
            assert(false);
        }
    }
    void perf_log::add(const char *key)
    {
        auto it = m_map_key.find(key);
        if (it == m_map_key.end())
        {
            perf_index *index = new perf_index();
            index->name = key;
            m_map_key[index->name] = index;
            m_index_list.push_back(index);
        }
    }
    void perf_log::put(const char *key, int64_t val)
    {
        m_lock.lock();
        auto it = m_map_key.find(key);
        if (it != m_map_key.end())
        {
            perf_index *p = it->second;
            if (p->cur_val == 0)
            {
                p->cur_val = val;
                p->pre_val = val;
            }
            else
            {
                p->pre_val = p->cur_val;
                p->cur_val = val;
            }

            if (p->type == perf_type_inc)
            {
                p->val = p->cur_val - p->pre_val;
            }
            else
            {
                it->second->val = val;
            }
        }
        m_lock.unlock();
    }
    void perf_log::set(const char *key, uint32_t type)
    {
         auto it = m_map_key.find(key);
        if (it != m_map_key.end()){
            it->second->type=type;
        }
    }

    void perf_log::output(){
        if(!m_file.is_open()){
            return;
        }
        date_time now=date_time::now();
        string tm=now.format("HH:mm:SS");
        m_lock.lock();
        m_file<<"tm="<<tm<<"\t";
        auto it=m_index_list.begin();
        for(;it!=m_index_list.end();++it){
            m_file<<(*it)->name<<"="<<(*it)->val<<"\t";
        }
        m_file<<"\n";
        m_lock.unlock();
    }
}