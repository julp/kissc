#include "strcpy_s.h"

#ifdef WITH_STRCPY
char *strcpy(char *to, const char *from) /* DEPRECATED */
{
    do {
        *to++ = *from++;
    } while ('\0' != *from);

    return to;
}
#endif /* WITH_STRCPY */

/**
 * Safely copy a string into a buffer
 *
 * @param to the output buffer
 * @param to_limit position right after the last usable byte of *to* (this is to + its size)
 * @param from the input string to copy
 *
 * @return false if the buffer was unsifficient
 *
 * For:
 * \code
 * char string[4];
 *
 * strcpy_sp(string, string + ARRAY_SIZE(string), "bar");
 * \endcode
 *
 * We got:
 * \code
 * +---+---+---+----+
 * | 0 | 1 | 2 |  3 |
 * +---+---+---+----+
 * | B | A | R | \0 |
 * +---+---+---+----+
 * ^ to             ^ to_limit
 * \endcode
 *
 * *to_limit* is right after the last usable byte and we reserve one byte
 * for a nul character.
 */
bool strcpy_sp(char *to, const char * const to_limit, const char *from)
{
    const char * const zero = to_limit - 1;

//     if (to == zero) {
//         *to = '\0';

//         return '\0' == *from;
//     } else {
        while (to </*=*/ zero && '\0' != (*to++ = *from/*++*/)) {
            /*++to,*/ ++from;
        }
        *to = '\0';

//         return '\0' == *(from - 1);
//     }
//     return (to == zero && '\0' == *from) || (to != zero && '\0' == *(from - 1));
    return '\0' == *from;
}

bool strcpy_sl(char *to, size_t to_size, const char *from)
{
    return strcpy_sp(to, to + to_size, from);
}
