#include "sq_struct_def.h"
#include "sq_tid_define.h"
#include "sq_unit_test.h"
#include "match/sq_match_core.h"
using namespace sq;
//订单插入,查看价位队列是否正确
SQ_TEST(test_match_core_case1)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_buy, 101, 2);
    m.insert_order("m2305", 3, f_buy, 101, 3);
    m.insert_order("m2305", 4, f_buy, 101, 4);

    m.insert_order("m2305", 11, f_sell, 110, 1);
    m.insert_order("m2305", 12, f_sell, 111, 2);
    m.insert_order("m2305", 13, f_sell, 111, 3);
    m.insert_order("m2305", 14, f_sell, 111, 4);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}

//撤销订单
SQ_TEST(test_match_core_case2)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_buy, 101, 2);

    m.insert_order("m2305", 11, f_sell, 110, 1);
    m.insert_order("m2305", 12, f_sell, 111, 2);

    m.cancel_order("m2305", 1,f_buy, 100);
    m.cancel_order("m2305", 2,f_buy, 101);

    m.cancel_order("m2305", 12,f_sell, 111);
    m.cancel_order("m2305", 11,f_sell, 110);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}

//测试成交 1 个档位
SQ_TEST(test_match_core_case3)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_sell,99, 1);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}
SQ_TEST(test_match_core_case4)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 2);
    m.insert_order("m2305", 2, f_sell,99, 1);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}

SQ_TEST(test_match_core_case5)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_buy, 100, 1);
    m.insert_order("m2305", 3, f_sell,99, 2);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}


SQ_TEST(test_match_core_case6)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_buy, 100, 1);
    m.insert_order("m2305", 3, f_sell,99, 3);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}



SQ_TEST(test_match_core_case7)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 1, f_buy, 100, 2);
    m.insert_order("m2305", 2, f_buy, 100, 2);
    m.insert_order("m2305", 3, f_sell,99, 3);

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}





//=====买触发=====

//测试成交 1 个档位
SQ_TEST(test_match_core_case43)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 2, f_sell,99, 1);
    m.insert_order("m2305", 1, f_buy, 100, 1);
    

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}
SQ_TEST(test_match_core_case44)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 2, f_sell,99, 1);
    m.insert_order("m2305", 1, f_buy, 100, 2);
    

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}

SQ_TEST(test_match_core_case45)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 3, f_sell,99, 2);
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_buy, 100, 1);
    

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}


SQ_TEST(test_match_core_case46)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 3, f_sell,99, 3);
    m.insert_order("m2305", 1, f_buy, 100, 1);
    m.insert_order("m2305", 2, f_buy, 100, 1);
    

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}



SQ_TEST(test_match_core_case47)
{
    sq_match_core m;
    sq_contract contract_info;
    contract_info.contract = "m2305";
    contract_info.market = "dce";
    contract_info.tick=10;
    m.add_contract(&contract_info);
   
    m.begin_match();
    m.insert_order("m2305", 3, f_sell,99, 3);
    m.insert_order("m2305", 1, f_buy, 100, 2);
    m.insert_order("m2305", 2, f_buy, 100, 2);
    

    list<mc_result*> result= m.get_match_result();
    //打印结果
    for (auto it = result.begin(); it != result.end(); it++)
    {
        (*it)->dump(std::cout);
    }
    m.dump(std::cout);
    m.end_match();
}