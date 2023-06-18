/**
Copyright (c) 2003-2015 
@brief 原子操作相关


@author 
*/
#pragma once
#include "sq_define.h"
namespace sq
{
	/** 原子计数器
	*/
	class atomic_counter
	{
		atomic_size_t   m_counter;
	public:
		typedef size_t      value_type;
		atomic_counter() throw() : m_counter(size_t(0)){}
		value_type operator =(value_type n) throw()
		{
			m_counter.exchange(n, std::memory_order_relaxed);
			return n;
		}
		size_t operator +=(size_t n) throw()
		{
			return m_counter.fetch_add(n, std::memory_order_relaxed) + n;
		}
		size_t operator -=(size_t n) throw()
		{
			return m_counter.fetch_sub(n, std::memory_order_relaxed) - n;
		}
		/// Get current value of the counter
		operator size_t () const throw()
		{
			return m_counter.load(std::memory_order_relaxed);
		}
		/// Preincrement
		size_t operator ++() throw()
		{
			return m_counter.fetch_add(1, std::memory_order_relaxed) + 1;
		}
		/// Postincrement
		size_t operator ++(int)throw()
		{
			return m_counter.fetch_add(1, std::memory_order_relaxed);
		}

		/// Predecrement
		size_t operator --() throw()
		{
			return m_counter.fetch_sub(1, std::memory_order_relaxed) - 1;
		}
		/// Postdecrement
		size_t operator --(int)throw()
		{
			return m_counter.fetch_sub(1, std::memory_order_relaxed);
		}

		/// Get current value of the counter
		size_t get() const throw()
		{
			return m_counter.load(std::memory_order_relaxed);
		}

		/// Resets the counter to 0
		void reset() throw()
		{
			m_counter.store(0, std::memory_order_release);
		}

	};


	
#if 0

	/**\brief 指针对象原子操作
	*/
	template <typename T>
	class atomic_ptr
	{
	public:
		inline atomic_ptr()
		{
		}

		inline ~atomic_ptr()
		{
		}

		inline void set(T *ptr)
		{
			m_ptr = ptr;
		}
		inline T*get()
		{
			return m_ptr.load();
		}

		/** @brief 用ptr指向的值与cmp_指向的值进行比较,如果相等那么就替换为
		val_指向的值
		*/
		inline void cas(T *cmp, T *val)
		{
			while (m_ptr.compare_exchange_weak(cmp, val,
				std::memory_order_release,
				std::memory_order_relaxed)) {}
		}

	private:
		volatile std::atomic<T*>  m_ptr;
		atomic_ptr(const atomic_ptr&) = delete;
		const atomic_ptr &operator = (const atomic_ptr&) = delete;
	};
	#endif
}