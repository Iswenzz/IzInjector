#include <greatest.h>

TEST test() 
{

    ASSERT_EQ(1, 1);
}

SUITE(Suite_IzInjector)
{
    RUN_TEST(test);
}
