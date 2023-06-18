#pragma once
#include <atomic>
#include <thread>
#include "error/sq_err.h"
using namespace std;
namespace sq
{
    //读写锁
    class sq_rwlock
    {
    private:
        /**< -1 when W lock held, > 0 when R locks held. */
        atomic<int32_t> cnt;

    public:
        sq_rwlock(/* args */);
        ~sq_rwlock();
        void read_lock();
        void read_unlock();
        void write_lock();
        void write_unlock();

        int read_try_lock();
        int write_try_lock();
    };

    sq_rwlock::sq_rwlock(/* args */)
    {
        cnt.store(0);
    }

    sq_rwlock::~sq_rwlock()
    {
        
    }
    void sq_rwlock::read_lock()
    {
        bool success = false;
        while (!success)
        {
            int32_t x=cnt.load();
            if(x<0){
                //is writing
                std::this_thread::yield();
            }

            success=cnt.compare_exchange_weak(x,x+1);
        }
    }
    void sq_rwlock::read_unlock()
    {
        cnt.fetch_sub(1);
    }
    void sq_rwlock::write_lock()
    {
        bool success = false;
        while (!success)
        {
            int32_t x=cnt.load();
            if(x!=0){
                //is writing or reading
                std::this_thread::yield();
            }

            success=cnt.compare_exchange_weak(x,-1);
        }
    }
    void sq_rwlock::write_unlock()
    {
        cnt.store(0);
    }

    int sq_rwlock::read_try_lock()
    {
        bool success = false;
        while (!success)
        {
            int32_t x=cnt.load();
            if(x<0){
                return err_fail;
            }

            success=cnt.compare_exchange_weak(x,x+1);
        }   
        return ok;
    }
    int sq_rwlock::write_try_lock()
    {
        int32_t x = cnt.load();

        if (x != 0 || cnt.compare_exchange_strong(x, -1) == false)
            return err_fail;

        return ok;
    }
}