#pragma once
#include <map>
#include <unordered_map>
#include "error/sq_err.h"
namespace sq
{
   /**
    * key val的基类，其它好多表都是继承于此，提供了一些基础接口
    * 内部有一个map 来作为索引，一个vector 存放数据
   */
   template <typename Key_t, typename Val_t>
   class sq_biz_table_base
   {
   public:
      using IndexTable_t = std::map<Key_t, Val_t *>;
      using Table_t=std::vector<Val_t*>;
      sq_biz_table_base()
      {
         auto it = m_Index.begin();
         for (; it != m_Index.end(); ++it)
         {
            Val_t *v = it->second;
            delete v;
         }
         m_Index.clear();
         m_Table.clear();
      }
      int add(Key_t &key, Val_t &v)
      {
         auto it = m_Index.find(key);
         if (it == m_Index.end())
         {
            Val_t *n_v = new Val_t();
            memcpy(n_v, &v, sizeof(Val_t));
            m_Index[key] = n_v;
            m_Table.push_back(n_v);
         }
         else
         {
            return err_repeat;
         }
         return 0;
      }

      int add(Key_t &key)
      {
         auto it = m_Index.find(key);
         if (it == m_Index.end())
         {
            Val_t *n_v = new Val_t();
            memset(n_v, 0, sizeof(Val_t));
            m_Index[key] = n_v;
            m_Table.push_back(n_v);
         }
         else
         {
            return err_repeat;
         }
         return 0;
      }
      int add(Key_t &key, Val_t *v)
      {
         auto it = m_Index.find(key);
         if (it == m_Index.end())
         {
            m_Index[key] = v;
            m_Table.push_back(v);
         }
         else
         {
            return err_repeat;
         }
         return ok;
      }
      void update(const Key_t &key, Val_t &v)
      {
         auto it = m_Index.find(key);
         if (it == m_Index.end())
         {
            Val_t *n_v = new Val_t();
            memcpy(n_v, &v, sizeof(Val_t));
            m_Index[key] = n_v;
         }
         else
         {
            Val_t *des = it->second;
            memcpy(des, &v, sizeof(Val_t));
         }
      }
#if 0
      void del(const Key_t &key)
      {
      
         auto it = m_Index.find(key);
         if (it != m_Index.end())
         {
            delete it->second;
            m_Index.erase(it);
         }
      }
#endif
      Val_t *find(const Key_t &key)
      {
         auto it = m_Index.find(key);
         if (it != m_Index.end())
         {
            return it->second;
         }
         return nullptr;
      }
      Val_t*at(int index){
         if(index>=0 && index<m_Table.size()){
            return m_Table[index];
         }
         return nullptr;
      }
      int size(){
         return m_Table.size();
      }
      void dump(std::ostream &of)
      {
         auto it = m_Index.begin();
         for (; it != m_Index.end(); ++it)
         {
            it->second->to_string(of);
            of << std::endl;
         }
      }

      IndexTable_t m_Index;
      Table_t      m_Table;
   };

   template <typename Key_t, typename Val_t>
   class sq_biz_unordered_table : public sq_biz_table_base<Key_t, Val_t>
   {
   public:
      using IndexTable_t = std::unordered_map<Key_t, Val_t *>;
   };

}