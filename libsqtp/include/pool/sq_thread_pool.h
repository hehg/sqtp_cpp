#pragma once
/** 线程池
 * 
 */ 
#include <thread>
#include <atomic>
#include <queue>
#include "sq_define.h"
using namespace std;
namespace sq
{
    struct task
    {
        virtual ~task(){};
        virtual void run() = 0;
    };

    class thread_pool
    {
        class work_thread
        {
        public:
            work_thread(int id, thread_pool *pool)
            {
                m_id = id;
                m_pool = pool;
                m_thread = new std::thread(std::bind(&work_thread::run, this));
            }

            void wait()
            {
                if (m_thread && m_thread->joinable())
                {
                    m_thread->join();
                }
            }
            void run()
            {
                while (m_pool->m_run_flag)
                {
                    //get task
                    task *t = m_pool->pop_task();
                    if (t)
                    {
                        printf("run task %d\n", m_id);
                        t->run();
                        printf("run task finish %d\n", m_id);
                        delete t;
                    }
                    else
                    {
                        //wait signal
                        std::unique_lock<std::mutex> lock(m_pool->m_cv_lock);
                        m_pool->m_cv.wait(lock);
                        //printf("no task %d\n",m_id);
                        //std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            }
            thread_pool *m_pool = nullptr;
            std::thread *m_thread = nullptr;
            int m_id;
        };

    public:
        thread_pool(int count)
        {
            m_thread_count = count;
        }
        ~thread_pool()
        {
            stop();
            for (int i = 0; i < m_thread_list.size(); i++)
            {
                m_thread_list[i]->wait();
                delete m_thread_list[i];
            }
            m_thread_list.clear();
            //清理task
            while (!m_task_list.empty())
            {
                task *t = m_task_list.front();
                delete t;
                m_task_list.pop();
            }
        }
        void add_task(task *t)
        {
            std::lock_guard<std::mutex> lock(m_task_lock);
            m_task_list.push(t);
            m_cv.notify_all();
        }
        task *pop_task()
        {
            std::lock_guard<std::mutex> lock(m_task_lock);
            if (!m_task_list.empty())
            {
                task *t = m_task_list.front();
                m_task_list.pop();
                return t;
            }
            return nullptr;
        }
        void wait()
        {
            for (int i = 0; i < m_thread_list.size(); i++)
            {
                m_thread_list[i]->wait();
            }
        }
        void start()
        {
            m_run_flag = 1;
            for (int i = 0; i < m_thread_count; i++)
            {
                m_thread_list.push_back(new work_thread(i, this));
            }
        }
        void stop()
        {
            m_run_flag = 0;
            m_cv.notify_all();
        }
        std::queue<task *> m_task_list;
        std::vector<work_thread *> m_thread_list;
        int m_run_flag = 0;
        int m_thread_count = 0;
        std::condition_variable m_cv;
        std::mutex m_cv_lock;
        std::mutex m_task_lock;
    };
}
