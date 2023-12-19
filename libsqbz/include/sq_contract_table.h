#pragma once
#include <unordered_map>
#include "sq_struct_def.h"
#include "sq_biz_table_base.h"
namespace sq
{
    /**
     * @合约表
    */
    class sq_contract_table:public sq_biz_table_base<std::string,sq_contract> {
    public:
        sq_contract_table(){
            m_table_name="contract_table";
        };
        ~sq_contract_table(){};

        bool load_from_text(const char* table_name);
      
    };
    
}