#pragma once
#include "sq_contract_table.h"

namespace sq
{
    class sq_memory_db
    {
        public:
        sq_memory_db();
        ~sq_memory_db();
        //void load(const char*dir);

        sq_contract_table*m_contract_table=nullptr;
    };
}