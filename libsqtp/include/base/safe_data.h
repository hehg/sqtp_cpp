/**
 * @brief 对一个数据读写的封装 ，这个数在锁保护下
*/
#pragma once
#include <mutex>

namespace sq
{
    template <typename data_t, typename lock_t = std::mutex>
    class safe_data
    {
    private:
        lock_t m_lock;
        data_t m_data;

    public:
        safe_data(const safe_data &d)
        {
            m_data = d;
        }
        void set(data_t &d)
        {
            m_lock.lock();
            m_data = d;
            m_lock.unlock();
        }
        void operator=(data_t d)
        {
            m_lock.lock();
            m_data = d;
            m_lock.unlock();
        }

        bool operator==(data_t d)
        {
            return d == get();
        }
        data_t get()
        {
            m_lock.lock();
            data_t d = m_data;
            m_lock.unlock();
            return d;
        }
    };

}