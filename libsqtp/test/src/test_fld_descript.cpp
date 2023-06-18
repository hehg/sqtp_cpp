#include "unit_test.h"
#if 0
#include "pack/msg_describe.h"
#include "protocol_quot.h"
#include "protocol_quot_des.h"
#include <iostream>
using namespace sq;
struct my_struct{
    int m1;
    float m2;
};

#define REG_STRUCT_DES_BEGIN(s) class s##_des:public sq::table_describe{\
public:\
 s##_des(){m_name="my_struct";

#define REG_STRUCT_DES_END(name) }};static name##_des name##_descript;

// REG_STRUCT_DES_BEGIN(my_struct)
// ADD_FIELD_INT32(my_struct,m1)
// ADD_FIELD_FLOAT32(my_struct,m2)
// REG_STRUCT_DES_END(my_struct)

SQ_TEST(test_descript)
{
    
   sq_quot quot;
   memset(&quot,0,sizeof(sq_quot));
   quot.tid=111;
   quot.pkg_no=0;
   quot.type=trade_type_t::f_future;
   quot.date="20220201";
   quot.time="09:23:32.000";
   quot.price=123.4;
   for(int i=0;i<5;i++){
    quot.mbl_buy_price[i]=i;
    quot.mbl_buy_qty[i]=i;
    quot.mbl_sell_price[i]=i+10;
    quot.mbl_sell_qty[i]=i+10;
   }

    sq_quot_desc des;
    des.to_string(&quot);
}
#endif