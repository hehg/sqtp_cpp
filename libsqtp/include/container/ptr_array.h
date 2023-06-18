/**
Copyright (c) 2003-2015 
@brief 动态数组，优化了添加和删除


@author 
*/

#pragma once
#include <vector>
#include <list>
namespace sq
{
	/** 所有使用ptr_array的对象要继承于ptr_array_item
	*/
	 class ptr_array_item
	{
	public:

		inline ptr_array_item() :
			array_index(-1)
		{
		}

		inline virtual ~ptr_array_item()
		{
		}

		inline void set_array_index(int index_)
		{
			array_index = index_;
		}

		inline int get_array_index()
		{
			return array_index;
		}

	private:

		int array_index;

		ptr_array_item(const ptr_array_item&);
		const ptr_array_item &operator = (const ptr_array_item&);
	};

	 /**\brief 存储指针类型的数组,能够快速的插入，删除
	 存储的指针对象必须继承于ptr_array_item
	 对数组进行了优化,并提供了删除操作
	 */
	 template <typename T>
	class ptr_array
	{
	private:

		typedef ptr_array_item  item_t;
		std::list<int> m_pool;
	public:

		typedef typename std::vector <T*>::size_type size_type;

		inline ptr_array()
		{
		}

		inline ~ptr_array()
		{
		}

		inline size_type size()
		{
			return items.size();
		}

		inline bool empty()
		{
			return items.empty();
		}

		inline T *&operator [] (size_type index_)
		{
			return items[index_];
		}

		inline void push_back(T *item_)
		{
			int idx = 0;
			if (!m_pool.empty()) {
				idx = m_pool.front();
				m_pool.pop_front();
				items[idx] = item_;
			}
			else {
				idx = (int)items.size();
				items.push_back(item_);
			}
			if (item_)
				((item_t*)item_)->set_array_index(idx);
			
		}

		inline void erase(T *item_) {
			erase(((item_t*)item_)->get_array_index());
		}

		inline T* erase(size_type index_) {
			
			m_pool.push_back(index_);
			item_t*will_del = items[index_];
			items[index_] = nullptr;

			return(T*)(will_del);
		}

		/*inline void swap(size_type index1_, size_type index2_)
		{
			if (items[index1_])
				((item_t*)items[index1_])->set_array_index((int)index2_);
			if (items[index2_])
				((item_t*)items[index2_])->set_array_index((int)index1_);
			std::swap(items[index1_], items[index2_]);
		}*/

		inline void clear()
		{
			items.clear();
		}

		inline size_type index(T *item_)
		{
			return (size_type)((item_t*)item_)->get_array_index();
		}

	private:

		typedef std::vector <T*> items_t;
		items_t items;

		ptr_array(const ptr_array&);
		const ptr_array &operator = (const ptr_array&);
	};
}