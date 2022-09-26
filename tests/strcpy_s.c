#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity/unity.h"

#include "utils.h"
#include "strcpy_s.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#define UT_NUL(input) \
    do { \
        char buffer[1]; \
 \
        *buffer = 'A'; \
        TEST_ASSERT(ARRAY_SIZE(buffer) >= STR_SIZE(input) == strcpy_sp(buffer, buffer + ARRAY_SIZE(buffer), input)); \
        TEST_ASSERT('\0' == *buffer); \
    } while (0);

#define UT(input, input_len) \
    do { \
        char buffer[input_len]; \
 \
        *buffer = 'A'; \
        printf("%zu // %zu = %d\n", (size_t) input_len, STR_SIZE(input), strcpy_sp(buffer, buffer + ARRAY_SIZE(buffer), input)); \
        TEST_ASSERT((input_len >= STR_SIZE(input)) == strcpy_sp(buffer, buffer + ARRAY_SIZE(buffer), input)); \
        printf("strncmp(\"%s\", \"%s\", %d) = %d\n", input, buffer, (int) (input_len >= STR_SIZE(input) ? STR_LEN(input) : input_len - 1), strncmp(input, buffer, input_len >= STR_SIZE(input) ? STR_LEN(input) : input_len - 1)); \
        TEST_ASSERT(0 == strncmp(input, buffer, input_len >= STR_SIZE(input) ? STR_LEN(input) : input_len - 1)); \
    } while (0);

void test_strcpy_sp(void)
{
    UT_NUL("");
    UT_NUL("a");
    UT_NUL("ab");

    UT("", 1000);
    UT("bar", 1000);
    UT("bar", STR_SIZE("bar"));
    UT("bar", STR_LEN("bar"));
    UT("bar", 2);
    UT("bar", 1);
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

    RUN_TEST(test_strcpy_sp, 3);

    return (0 == UnityEnd() ? EXIT_SUCCESS : EXIT_FAILURE);
}
