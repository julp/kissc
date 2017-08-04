#include <stddef.h>

/**
 * X
 *
 * @param to
 * @param from
 * @param to_limit
 *
 * @return
 */
char *stpcpy_sp(char *to, const char *from, const char * const to_limit)
{
    const char * const zero = to_limit - 1;

    if (NULL == to || to >= zero) {
        return NULL;
    }
    if (NULL != from) {
        while (to < zero && '\0' != (*to++ = *from++))
            ;
    }
    if (to == zero) {
        *to = '\0';
        return NULL;
    } else {
        return to - 1;
    }
}

#if 0
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
#endif
