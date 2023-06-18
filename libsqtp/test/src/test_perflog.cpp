#include "unit_test.h"
#include "log/perf_log.h"


SQ_TEST(test_perflog)
{
    using namespace sq;
    perf_log log;
    log.open("./perf.log");

    log.add("key1");
    log.add("key2");
    log.set("key2",perf_type_inc);
    
    for(int i=0;i<10;i++){
        log.put("key1",i);
        log.put("key2",i);

        log.output();
    }
    
}