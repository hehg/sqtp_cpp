#include "sq_struct_def.h"
#include "sq_quot_file.h"
#include "sq_tid_define.h"
#include "sq_unit_test.h"
using namespace sq;

SQ_TEST(test_quot_file_write)
{
    sq_quot_file_writer w;
    w.open("./quot.csv","w+");
    for(int i=0;i<100;i++){
        sq_quot quot;
        memset(&quot,0,sizeof(sq_quot));
        quot.pkg_no=i;
        quot.contract="m";
        w.write(tid_market_data,(char*)&quot,sizeof(sq_quot));
    }
    w.close();
}
SQ_TEST(test_quot_file_read)
{
    sq_quot_file_reader w;
    w.open("./quot.csv",true);
    sq_quot quot;
    while (w.read_line(quot))
    {
        std::stringstream ss;
        quot.to_string(ss);
        printf("%s\n",ss.str().c_str());
    }
    
    w.close();
}