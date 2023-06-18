/**
Copyright (c) 2003-2015 
@brief 对象池
@author 
*/
#pragma once
#include "sq_define.h"
#include <mutex>
namespace sq
{
	/**
	 * 对象池
	*/
	template<typename T,typename lock_t=std::mutex>
	class object_pool
	{
	public:
	/**
	 * init_size 初始对象个数
	 * increment 消耗光后,增加的数量
	 */
		object_pool(int init_size,int increment):m_init_size(init_size),
		m_increment(increment){
			for (int i = 0; i < m_init_size; i++)
			{
				m_pool.push_back(new T());
			}
		};
		~object_pool(){
			typename list<T*>::iterator it = m_pool.begin();
			for (; it != m_pool.end(); ++it)
			{
				delete *it;
			}
			m_pool.clear();
		};

		T*alloc(){
			if (m_pool.empty()){
				realloc();
			}
			m_lock.lock();
			T*obj=m_pool.front();
			m_pool.pop_front();
			m_lock.unlock();
			return obj;
		}
		void free(T*obj){
			m_lock.lock();
			m_pool.push_back(obj);
			m_lock.unlock();
		}

	private:
		void realloc(){
			for (int i = 0; i < m_increment; i++)
			{
				m_pool.push_back(new T());
			}
		}
		list<T*> m_pool;
		int m_init_size;
		int m_increment;
		lock_t m_lock;
	private:
		//DISABLE_COPY_MOVE_AND_ASSIGN(object_pool);
	};
}