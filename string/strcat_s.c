#include <stddef.h>
#include <assert.h>

#include "strcat_s.h"

/**
 * Safely append a string to an other one
 *
 * @param to the modified string where to append *suffix*
 * @param to_limit position right after the last usable byte of *to* (this is *to* + its size)
 * @param suffix the string to concatenate to *to*
 *
 * @return `false` if the buffer is too small
 */
bool strcat_sp(char *to, const char * const to_limit, const char *suffix)
{
    const char * const zero = to_limit - 1;

    assert(NULL != to);
    assert(NULL != to_limit);
    assert(NULL != suffix);

    for (; '\0' != *to; to++)
        ;
    while (to < zero && '\0' != (*to++ = *suffix)) {
        ++suffix;
    }
    *to = '\0';

    return '\0' == *suffix;
}
