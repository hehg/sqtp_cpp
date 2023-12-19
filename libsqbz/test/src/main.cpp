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
    
    //UT->run("test_quot_file_write");
    UT->run("test_quot_file_read");
    }
    return 0;
}