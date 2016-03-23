#include "strcat.h"

#ifdef WITH_STRCAT
char *strcat(char *to, const char *suffix) /* DEPRECATED */
{
    char *w;

    for (w = to; '\0' != *w; w++)
        ;
    while ('\0' != (*w++ = *suffix++))
        ;

    return to;
}
#endif /* WITH_STRCAT */

/**
 * X
 *
 * @param to
 * @param to_limit
 * @param suffix
 *
 * @return X
 */
bool strcat_sp(char *to, const char * const to_limit, const char *suffix)
{
    const char * const zero = to_limit - 1;

    for (; '\0' != *to; to++)
        ;
    while (to < zero && '\0' != (*to++ = *suffix)) {
        ++suffix;
    }
    *to = '\0';

    return '\0' == *suffix;
}
