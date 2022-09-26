#include <stdio.h>
#include <stdlib.h>

#include "unity/unity.h"

#include "utils.h"
#include "iterator.h"

void setUp(void)
{
}

void tearDown(void)
{
}

static int numbers[] = {1, 2, 3};

void test_int_array_to_iterator(void)
{
    int *v;
    size_t i;
    uint64_t k;
    Iterator it;

    v = NULL;
    array_to_iterator(&it, numbers, sizeof(numbers[0]), ARRAY_SIZE(numbers));
    iterator_first(&it);
    for (i = 0; i < ARRAY_SIZE(numbers); i++) {
        TEST_ASSERT_TRUE(iterator_is_valid(&it, &k, &v));
        TEST_ASSERT_EQUAL_UINT64(i, k);
        TEST_ASSERT_EQUAL_INT(numbers[i], *v);
        iterator_next(&it);
    }
    TEST_ASSERT_FALSE(iterator_is_valid(&it, &k, &v));
    iterator_close(&it);
}

static const char *strings[] = {"un", "deux", "trois"};

void test_string_array_to_iterator(void)
{
    size_t i;
    uint64_t k;
    Iterator it;
    const char **v;

    v = NULL;
    array_to_iterator(&it, strings, sizeof(strings[0]), ARRAY_SIZE(strings));
    iterator_first(&it);
    for (i = 0; i < ARRAY_SIZE(strings); i++) {
        TEST_ASSERT_TRUE(iterator_is_valid(&it, &k, &v));
        TEST_ASSERT_EQUAL_UINT64(i, k);
        TEST_ASSERT_EQUAL_STRING(strings[i], *v);
        iterator_next(&it);
    }
    TEST_ASSERT_FALSE(iterator_is_valid(&it, &k, &v));
    iterator_close(&it);
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

    RUN_TEST(test_int_array_to_iterator, 19);
    RUN_TEST(test_string_array_to_iterator, 41);

    return (0 == UnityEnd() ? EXIT_SUCCESS : EXIT_FAILURE);
}
