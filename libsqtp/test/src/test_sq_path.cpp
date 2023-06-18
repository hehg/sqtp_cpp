#include "os/sq_path.h"
#include "unit_test.h"
using namespace sq;

SQ_TEST(test_sq_path)
{
    bool ret=sq::sq_path::rmdir("./mydir");
    SQ_EXPECT_TRUE(ret);
}