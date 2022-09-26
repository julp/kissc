#include <stddef.h>
#include <assert.h>
#include <string.h> /* strncmp */

#include "str_starts_with.h"

/**
 * Test if a string begins with an other
 *
 * @param string the string to test
 * @param prefix the string to search for
 *
 * @return `true` if *string* begins with *prefix*
 */
bool str_starts_with(const char *string, const char *prefix)
{
    size_t prefix_len;

    assert(NULL != string);
    assert(NULL != prefix);

    prefix_len = strlen(prefix);

    return prefix_len <= strlen(string) && 0 == strncmp(string, prefix, prefix_len);
}
