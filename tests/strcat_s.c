#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity/unity.h"

#include "cpp/utils.h"
#include "strcat_s.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#define UT(content, suffix, extra_len) \
    do { \
        bool result; \
        char buffer[STR_SIZE(content) + extra_len]; \
 \
        memcpy(buffer, content, STR_SIZE(content)); \
        /*printf("%zu // %zu + %zu\n", ARRAY_SIZE(buffer), STR_SIZE(content), STR_LEN(suffix));*/ \
        result = strcat_sp(buffer, buffer + ARRAY_SIZE(buffer), suffix); \
        /*printf("strcat_s(\"%s\", \"%s\") = \"%s\" (%d)\n", content, suffix, buffer, result);*/ \
        TEST_ASSERT(result == (ARRAY_SIZE(buffer) >= STR_SIZE(content) + STR_LEN(suffix))); \
        TEST_ASSERT(0 == memcmp(content, buffer, STR_LEN(content))); \
        TEST_ASSERT(0 == strncmp(buffer + STR_LEN(content), suffix, ARRAY_SIZE(buffer) - STR_SIZE(content))); \
    } while (0);

void test_strcat_s(void)
{
    UT("", "foo", 0); // ""
    UT("", "foo", STR_LEN("foo")); // "foo"
    UT("", "foo", 1000); // "foo"

    UT("abc", "def", 0); // "abc"
    UT("abc", "def", 1); // "abcd"
    UT("abc", "def", 2); // "abcde"
    UT("abc", "def", 3); // "abcdef"
    UT("abc", "def", STR_SIZE("def")); // "abcdef"
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

    RUN_TEST(test_strcat_s, 3);

    return (0 == UnityEnd() ? EXIT_SUCCESS : EXIT_FAILURE);
}
