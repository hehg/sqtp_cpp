/**
Copyright (c) 2003-2015 
@brief 支持多生产者多消费者的队列


@author 
*/
#pragma once
#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>
namespace sq
{
	/**支持多生产者多消费者的队列
	*/
	template<typename T>
	class shared_queue
	{
		std::queue<T*> m_queue;
		mutable std::mutex m_mutex;
		std::condition_variable m_data_cond;

		shared_queue &operator=(const shared_queue &) = delete;
		shared_queue(const shared_queue &other) = delete;

	public:
		shared_queue() {}

		void push(T* item) 
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_queue.push(item);
			}
			m_data_cond.notify_one();
		}

		bool try_and_pop(T* &popped_item) {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_queue.empty()) {
				return false;
			}
			popped_item = m_queue.front();
			m_queue.pop();
			return true;
		}

		int wait_and_pop(T* &popped_item) {
			std::unique_lock<std::mutex> lock(m_mutex);
			while (m_data_cond.wait_for(lock,std::chrono::seconds(5))==
			std::cv_status::timeout)
			{
				popped_item=nullptr;
				return 1;//timeout
			}
			
			popped_item = m_queue.front();
			m_queue.pop();
			return 0;
		}

		bool empty() const {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_queue.empty();
		}

		unsigned size() const {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_queue.size();
		}
	};
}