#include <stddef.h>

/**
 * X
 *
 * @param to
 * @param from
 * @param zero
 *
 * @return
 */
char *stpcpy_sp(char *to, const char *from, const char * const zero)
{
    const char * const end = zero - 1;

    if (NULL == to || to >= end) {
        return NULL;
    }
    if (NULL != from) {
        while (to < end && '\0' != (*to++ = *from++))
            ;
    }
    if (to == end) {
        *to = '\0';
        return NULL;
    } else {
        return to - 1;
    }
}

/**
 * X
 *
 * @param to
 * @param from
 * @param string
 * @param string_len
 *
 * @return
 */
size_t stpcpy_sl(char *to, const char *from, const char * const *string, size_t string_len)
{
    //
}
