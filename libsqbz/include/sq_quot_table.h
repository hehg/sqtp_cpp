#pragma once
#include "sq_struct_def.h"
#include "sq_biz_table_base.h"
using namespace std;
namespace sq {

    /**各个合约的行情快照
    */
    class sq_quot_table:public sq_biz_table_base<std::string,sq_quot,std::unordered_map<std::string,sq_quot*>>
    {
    public:
        
    };

}