#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "sq_unit_test.h"
int main(int argc,char** argv)
{
    if(argc==2){
        sq_unit_test::instance()->run(argv[1]);
    }
    else{
    //sq_unit_test::instance()->run();
    //sq_unit_test::instance()->run("test_ring_buf_var2");
    //sq_unit_test::instance()->run("test_vint");
    //sq_unit_test::instance()->run("serialize");
    //sq_unit_test::instance()->run("test_perflog");
    //sq_unit_test::instance()->run("test_http_client");
    //sq_unit_test::instance()->run("test_sq_path");
    UT->run("test_serialize_case1");
    //UT->run("test_websocket_client");
    //UT->run("test_sim_trader");
    //UT->run("test_ws_client");
   // UT->run("test_http_parser_rsp");
   // UT->run("test_sq_sqlite_adapter");
    }
    return 0;
}
