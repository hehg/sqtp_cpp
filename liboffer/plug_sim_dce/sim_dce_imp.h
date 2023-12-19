#pragma once
#include "plug_base.h"
#include "sq_contract_table.h"
#include <thread>
using namespace sq;

namespace sq_plug
{
    class sim_dce_imp : public plug_base
    {
    public:
        sim_dce_imp();
        ~sim_dce_imp();

        int open();
        int close();
        void run();
int put(uint16_t tid, char* data, uint16_t size);
    private:
        void init_data();
        std::thread *m_engine = nullptr;
        bool run_flag_ = false;
        
        sq_contract_table m_contract_table;
    };
}