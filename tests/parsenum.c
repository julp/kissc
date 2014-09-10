#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "parsenum.h"
#include "cpp/utils.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#define UT(name, type, strval, expected_value, expected_return, base, _min, _max, stopoffset) \
    void test_strtonX_##name##_##type(void) { \
        ParseNumError x; \
        char *endptr; \
        char stopchar; \
        type min, max, *pmin, *pmax, v = 0; \
        char string[STR_SIZE(strval)]; \
        /*char *string;*/ \
        /*size_t size = STR_SIZE(strval);*/ \
         \
        /*string = malloc(size);*/ \
        memcpy(string, strval, STR_LEN(strval)); \
        string[STR_LEN(string)] = '#'; \
        pmin = pmax = NULL; \
        if (0 != _min) { \
            min = _min; \
            pmin = &min; \
        } \
        if (0 != _max) { \
            max = _max; \
            pmax = &max; \
        } \
        x = strnto ## type(string, string + STR_LEN(strval), &endptr, base, pmin, pmax, &v); \
        if (expected_return != x) { \
            printf("parse_%s(%s): %d returned (%d expected)\n", #type, string, x, expected_return); \
        } \
        TEST_ASSERT(x == expected_return); \
        if (v != expected_value) { \
            /* TODO: PRIi8 vs PRIu8 modifier */ \
            printf("parse_%s(%s): %d parsed (%d expected)\n", #type, string, v, expected_value); \
        } \
        TEST_ASSERT(v == expected_value); \
        stopchar = string[stopoffset]; \
        if (*endptr != stopchar) { \
            printf("parse_%s(%s): stopped on 0x%02X/%c (0x%02X/%c expected)\n", #type, string, *endptr, *endptr, stopchar, stopchar); \
        } \
        TEST_ASSERT(endptr - string == stopoffset); \
    }
#include "parsenum-ut.h"
#undef UT

#define UT(name, type, string, expected_value, expected_return, base, _min, _max, stopoffset) \
    void test_strtoX_##name##_##type(void) { \
        ParseNumError x; \
        char *endptr; \
        char stopchar; \
        type min, max, *pmin, *pmax, v = 0; \
         \
        pmin = pmax = NULL; \
        if (0 != _min) { \
            min = _min; \
            pmin = &min; \
        } \
        if (0 != _max) { \
            max = _max; \
            pmax = &max; \
        } \
        x = strto ## type(string, &endptr, base, pmin, pmax, &v); \
        if (expected_return != x) { \
            printf("parse_%s(%s): %d returned (%d expected)\n", #type, string, x, expected_return); \
        } \
        TEST_ASSERT(x == expected_return); \
        if (v != expected_value) { \
            /* TODO: PRIi8 vs PRIu8 modifier */ \
            printf("parse_%s(%s): %d parsed (%d expected)\n", #type, string, v, expected_value); \
        } \
        TEST_ASSERT(v == expected_value); \
        stopchar = string[stopoffset]; \
        if (*endptr != stopchar) { \
            printf("parse_%s(%s): stopped on 0x%02X/%c (0x%02X/%c expected)\n", #type, string, *endptr, *endptr, stopchar, stopchar); \
        } \
        TEST_ASSERT(endptr - string == stopoffset); \
    }
#include "parsenum-ut.h"
#undef UT

void good_strntoint8_t(void)
{
    int8_t ret;
    char *endptr, string[] = "23ABC";

    TEST_ASSERT(PARSE_NUM_NO_ERR == strntoint8_t(string, string + STR_LEN("23"), &endptr, 10, NULL, NULL, &ret));
    TEST_ASSERT(23 == ret);
    TEST_ASSERT('A' == *endptr);
}

void good_strtoint8_t(void)
{
    int8_t ret;
    char *endptr, string[] = "23";

    TEST_ASSERT(PARSE_NUM_NO_ERR == strtoint8_t(string, &endptr, 10, NULL, NULL, &ret));
    TEST_ASSERT(23 == ret);
    TEST_ASSERT('\0' == *endptr);
}

void strntoint8_t_without_endptr(void)
{
    int8_t ret;
    char *endptr, string[] = "23ABC";

    TEST_ASSERT(PARSE_NUM_ERR_NON_DIGIT_FOUND == strntoint8_t(string, string + STR_LEN(string), NULL, 10, NULL, NULL, &ret));
    TEST_ASSERT(23 == ret);
}

void strtoint8_t_without_endptr(void)
{
    int8_t ret;
    char *endptr, string[] = "23ABC";

    TEST_ASSERT(PARSE_NUM_ERR_NON_DIGIT_FOUND == strtoint8_t(string, NULL, 10, NULL, NULL, &ret));
    TEST_ASSERT(23 == ret);
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

    RUN_TEST(good_strntoint8_t, 105);
    RUN_TEST(good_strtoint8_t, 115);
    RUN_TEST(strntoint8_t_without_endptr, 125);
    RUN_TEST(strtoint8_t_without_endptr, 134);
    fputs("==========\n", stdout);
#define UT(name, type, string, expected_value, expected_return, base, _min, _max, stopchar) \
    RUN_TEST(test_strtonX_##name##_##type, 0);
#include "parsenum-ut.h"
#undef UT
    fputs("==========\n", stdout);
#define UT(name, type, string, expected_value, expected_return, base, _min, _max, stopchar) \
    RUN_TEST(test_strtoX_##name##_##type, 0);
#include "parsenum-ut.h"
#undef UT

    return (0 == UnityEnd() ? EXIT_SUCCESS : EXIT_FAILURE);
}
