#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "unit_test.h"
int main(int argc,char** argv)
{
    if(argc==2){
        unit_test::instance()->run(argv[1]);
    }
    else{
    unit_test::instance()->run();
    //unit_test::instance()->run("test_ring_buf_var2");
    //unit_test::instance()->run("test_vint");
    //unit_test::instance()->run("serialize");
    //unit_test::instance()->run("test_perflog");
    //unit_test::instance()->run("test_http_client");
    //unit_test::instance()->run("test_sq_path");
    //UT->run("test_fmt_log");
    //UT->run("test_websocket_client");
    //UT->run("test_sim_trader");
    //UT->run("test_ws_client");
    UT->run("test_http_parser_rsp");
    }
    return 0;
}
