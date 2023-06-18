#pragma once
#include "sq_define.h"
#include <condition_variable>
namespace sq
{
    

    class sq_cv
    {

    public:
        bool wait(int32_t ms)
        {
            std::unique_lock<std::mutex> plock(m_mlock);
            m_hava_data = false;
            auto ret = m_cv.wait_for(plock, std::chrono::seconds(5), [&] {return m_hava_data; });

            return ret;
        }
        void notify(){
            m_hava_data = true;
            m_cv.notify_all();
        }
        std::mutex m_mlock;
        std::condition_variable m_cv;
        bool m_hava_data = false;
    };

}