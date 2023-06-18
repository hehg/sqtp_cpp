#include "sq_struct_def.h"
#include "sq_quot_table.h"
#include "unit_test.h"
using namespace sq;

SQ_TEST(test_quot_table)
{

    sq_quot_table tb;
    
    sq_quot quot;
    memset(&quot,0,sizeof(quot));
    //char name[32];
    for (int i = 0; i < 10; i++)
    {
        sprintf(quot.contract.data(),"contract_%d",i);
        
        std::string contract_id=quot.contract.data();
        tb.add(contract_id,quot);
        tb.add(contract_id,quot);
    }
    //tb.dump(std::cout);

   // tb.del("contract_1");

    tb.dump(std::cout);
}