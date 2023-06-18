#include "plug_base.h"
#include <thread>

namespace sq_plug
{

    class replay_quot : public plug_base
    {
    public:
    replay_quot();
    ~replay_quot();
    
        int open();
        int close();
        void run();
        std::thread *m_engine = nullptr;
        bool run_flag_ = false;
    };
}