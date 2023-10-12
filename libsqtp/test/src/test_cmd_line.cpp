#include "sq_unit_test.h"
#include "log/sq_logger.h"
#include "cmd_line/sq_cmd_line_handler.h"
using namespace sq;

#if SUPPORT_READLINE 
SQ_TEST(test_cmd_line_case1)
{
    sq_cmd_line_handler h;
    h.run();
}
#endif