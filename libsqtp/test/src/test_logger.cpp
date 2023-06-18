#include "unit_test.h"
#include "log/sq_logger.h"
#include <stdint.h>
using namespace sq;


SQ_TEST(test_fmt_log)
{
    s_log.add_stdout_target();
   log_info("this is a book {},{}\n",1,"harly");
   log_debug("this is a book {},{}\n",2,"harly");
   log_error("this is a book {},{}\n",3,"harly");
}

SQ_TEST(test_log2)
{

   logger log;
   log.add_stdout_target();
   m_log_info(log,"this is log1 {}\n",1);

}