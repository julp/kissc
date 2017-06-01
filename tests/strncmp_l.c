#include <stdio.h>
#include <stdlib.h>

#include "unity/unity.h"

#include "strcmp_l.h"
#include "strncmp_l.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_strcmp_l(void)
{
    TEST_ASSERT(strcmp_l("", 0, "", 0) == 0);
    TEST_ASSERT(strcmp_l("", 0, "abc", 3) < 0);
    TEST_ASSERT(strcmp_l("abc", 3, "", 0) > 0);

    TEST_ASSERT(strcmp_l("ab", 2, "abc", 3) < 0);
    TEST_ASSERT(strcmp_l("ab", 2, "abc", 2) == 0);
}

void test_strncmp_l(void)
{
    TEST_ASSERT(strncmp_l("", 0, "", 0, 0) == 0);
    TEST_ASSERT(strncmp_l("", 0, "abc", 3, 3) < 0);
    TEST_ASSERT(strncmp_l("abc", 3, "", 0, 3) > 0);

    TEST_ASSERT(strncmp_l("ab", 2, "abc", 3, 3) < 0);
    TEST_ASSERT(strncmp_l("ab", 2, "abc", 3, 2) == 0);
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

void resetTest()
{
    tearDown();
    setUp();
}


int main(void)
{
    Unity.TestFile = __FILE__;
    UnityBegin();

    RUN_TEST(test_strcmp_l, 14);
    RUN_TEST(test_strncmp_l, 24);

    return (0 == UnityEnd() ? EXIT_SUCCESS : EXIT_FAILURE);
}
