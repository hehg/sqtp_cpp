#pragma once
#include <map>
#include <unordered_map>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
using namespace std;
namespace sq
{
    template <typename k_type,typename v_type>
    class kv_table
    {
    public:
        kv_table()
        {

        }
        ~kv_table()
        {
            clear();
        }
        void clear()
        {
            auto it = items_.begin();
            for (; it != items_.end(); ++it)
            {
                delete it->second;
            }
            items_.clear();
        }
        int put(k_type k,v_type*val)
        {
           // assert(size<=cap_size);
            auto it=items_.find(k);
            if(it!=items_.end()){
                memcpy(it->second,(char*)val,sizeof(v_type));
            }
            else{
                v_type* item=new v_type;
                memcpy(item,(char*)val,sizeof(v_type));
                items_[k]=item;
            }
            return 0;
        }
        
        v_type*get(k_type k)
        {
            auto it=items_.find(k);
            if(it!=items_.end()){
                return it->second;
            }
            return nullptr;
        }
        
        v_type* begin(){
            m_it=items_.begin();
            if(m_it==items_.end()){
                return nullptr;
            }
            return m_it->second;
        }
        v_type*next()
        {
            ++m_it;
            if(m_it==items_.end()){
                return nullptr;
            }
            return m_it->second;
        }
        int size(){return items_.size();}
        std::map<k_type, v_type *> items_;
        typename std::map<k_type, v_type *>::iterator m_it;
    };


#if 1
    //特化,固定大小内存
    template <typename k_type>
    class kv_table<k_type,char>
    {
    public:
        kv_table()
        {
        }
        ~kv_table()
        {
            auto it=items_.begin();
            for(;it!=items_.end();++it){
                delete[] it->second;
            }
            items_.clear();
        }
        int put(k_type k,void*val,int size)
        {
            assert(size<=cap_size);
            auto it=items_.find(k);
            if(it!=items_.end()){
                memcpy(it->second,(char*)val,size);
            }
            else{
                char* item=new char[cap_size];
                memcpy(item,(char*)val,size);
                items_[k]=item;
            }
            return 0;
        }
        
        char*get(k_type k)
        {
            auto it=items_.find(k);
            if(it!=items_.end()){
                return it->second;
            }
            return nullptr;
        }

        std::map<k_type, char *> items_;
        int cap_size=512;
        int size(){return items_.size();}
    };

#endif
}