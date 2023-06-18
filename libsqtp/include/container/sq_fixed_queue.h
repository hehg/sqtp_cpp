#pragma once
#include "sq_define.h"
#include <deque>
#include "pool/object_poll.h"
namespace sq
{
    using namespace std;
    /** 
     * 固定大小的队列，数据量超出指定大小后，最先压入队列
    的数据被弹出
    */
    template<typename T>
    class sq_fixed_queue_obj
    {
    public:
        sq_fixed_queue_obj(){
            m_max_size = 0;
            m_pool = new object_pool<T>(100, 10);
        }
        sq_fixed_queue_obj(int size) {
            m_max_size = size;
            m_data_list.reserve(size);
            m_pool = new object_pool<T>(size, 10);
        }
        ~sq_fixed_queue_obj() {
            for (auto i : m_data_list) {
                delete i;
            }
            m_data_list.clear();
        }
        void init(int size) {
            m_max_size = size;
            if (m_pool)
                delete m_pool;
            m_pool = new object_pool<T>(size, 10);
        }
        int32_t size() {
            return m_data_list.size();
        }
        void reset() {
            m_data_list.clear();
           
        }
		T* first() {
			if (!m_data_list.empty()) {
				return m_data_list.front();
			}
			else {
				return nullptr;
			}
		}
        T* last() {
            if (!m_data_list.empty()) {
                return m_data_list.back();
            }
            else {
                return nullptr;
            }
        }
		T* alloc() {
			T* item = m_pool->alloc();
			return item;
		}
        void push_back(T* t) {
            if (m_data_list.size() == m_max_size) {
                T* item = m_data_list.front();
                m_pool->free(item);
                m_data_list.pop_front();
            }
           
           /* T* item = m_pool->alloc();
            memcpy(item, t, sizeof(T));*/
            m_data_list.push_back(t);
           
        }
        
        T* operator[](int32_t idx) {
            return m_data_list[idx];
            
        }
        object_pool<T>*  m_pool;
        std::deque<T*> m_data_list;
        size_t   m_max_size{ 0 };
    };

    //必须为基本数据类型
    template<typename T>
    class sq_fixed_queue
    {
    public:
        sq_fixed_queue() {
            m_max_size = 0;
        }
        sq_fixed_queue(int size) {
            m_max_size = size;
            m_data_list.reserve(size);
        }
        ~sq_fixed_queue() {
            m_data_list.clear();
        }
        void init(int size) {
            m_max_size = size;
        }
        int32_t size() {
            return m_data_list.size();
        }
        void reset() {
            m_data_list.clear();

        }
        T last() {
            if (!m_data_list.empty()) {
                return m_data_list.back();
            }
            else {
                return nullptr;
            }
        }
        void push_back(T t) {
            if (m_data_list.size() == m_max_size) {
                m_data_list.pop_front();
            }
            m_data_list.push_back(t);

        }

        T operator[](int32_t idx) {
            return m_data_list[idx];

        }
        std::deque<T> m_data_list;
        size_t   m_max_size{ 0 };
    };
}