#include "sq_unit_test.h"
#include "sq_struct_def.h"
#include "sq_struct_des.h"
#include <typeinfo>  
using namespace sq;

SQ_TEST(test_struct_des_contract_info)
{
    sq_contract info;
    bzero(&info, sizeof(info));
    info.contract="m1234";
    info.variety="m";
    info.base_contract="";
    info.benchmark_price=123.4;
    info.market="dce";
    info.expiry_date="20220405";
    info.unit=10;
    info.tick=2;
    info.deliv_month="03";
    info.deliv_year="2022";
    info.deliv_date_begin="05";
    info.strike_price=123.4;
    info.call_put='C';    



    sq_contract_desc des;
    des.to_string(&info);
    
    // std::cout<<typeid(int).name()<<std::endl;
    // std::cout<<typeid(float).name()<<std::endl;
    // std::cout<<typeid(contract_t).name()<<std::endl;
    // std::cout<<typeid(byte_array<21>).name()<<std::endl;
    // std::cout<<typeid(byte_array<32>).name()<<std::endl;
}

