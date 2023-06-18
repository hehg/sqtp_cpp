#pragma once
#include <unordered_map>
#include "sq_struct_def.h"
namespace sq
{
    #if 0
    class sq_contract_table {
    public:
        contract_table() = default;
        ~contract_table();
        sq_contract*find(const char* contract);
        void clear();

        std::unordered_map<string, sq_contract*> m_table;
    };
    #endif 
}