#include <stdio.h>
#include <stdlib.h>

#include "unity/unity.h"

#include "memcmp_l.h"
#include "memncmp_l.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_memcmp_l(void)
{
    TEST_ASSERT(memcmp_l("", 0, "", 0) == 0);
    TEST_ASSERT(memcmp_l("", 0, "abc", 3) < 0);
    TEST_ASSERT(memcmp_l("abc", 3, "", 0) > 0);

    TEST_ASSERT(memcmp_l("ab", 2, "abc", 3) < 0);
    TEST_ASSERT(memcmp_l("ab", 2, "abc", 2) == 0);
}

void test_memncmp_l(void)
{
    TEST_ASSERT(memncmp_l("", 0, "", 0, 0) == 0);
    TEST_ASSERT(memncmp_l("", 0, "abc", 3, 3) < 0);
    TEST_ASSERT(memncmp_l("abc", 3, "", 0, 3) > 0);

    TEST_ASSERT(memncmp_l("ab", 2, "abc", 3, 3) < 0);
    TEST_ASSERT(memncmp_l("ab", 2, "abc", 3, 2) == 0);
}

char MessageBuffer[50];

static void runTest(UnityTestFunction test)
{
    if (TEST_PROTECT()) {
        setUp();
        test();
    }
    if (TEST_PROTECT() && !TEST_IS_IGNORED) {
        tearDown();
    }
}

void resetTest(void)
{
    tearDown();
    setUp();
}


int main(void)
{
    Unity.TestFile = __FILE__;
    UnityBegin();

    RUN_TEST(test_memcmp_l, 14);
    RUN_TEST(test_memncmp_l, 24);

    return (0 == UnityEnd() ? EXIT_SUCCESS : EXIT_FAILURE);
}
