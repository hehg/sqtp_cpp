#pragma once
#include <atomic>
#include <mutex>
namespace sq
{
    /**
     * @brief 单例模式
    */
    template<typename T>
    class sq_singleton
    {
    public:
        static T& instance()
        {
            static std::atomic<T*> x;
            static std::mutex mtx;

            T*p = x.load(std::memory_order_acquire);
            if (!p) {
                std::lock_guard<std::mutex> loc(mtx);
                p = x.load(std::memory_order_relaxed);
                if (!p) {
                    p = new T;
                    x.store(p, std::memory_order_release);
                }
            }
            return *p;
        }
    };
}
