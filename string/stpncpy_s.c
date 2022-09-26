#include <stddef.h>
#include <assert.h>

/**
 * Safely copy at most *n* bytes and get a pointer to the trailing nul byte to
 * easily append other strings to it.
 *
 * \code
 *  #define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
 *  #define STR_SIZE(str) (ARRAY_SIZE(str))
 *
 *  char *w, buffer[MAXPATHLEN];
 *  const char * const buffer_end = buffer + STR_SIZE(buffer);
 *
 *  if (NULL == (w = stpncpy_sp(buffer, "abcdef", buffer_end, 3))) {
 *      // buffer overflow
 *  }
 *  if (NULL == (w = stpncpy_sp(w, "ghi", buffer_end, 6))) {
 *      // buffer overflow
 *  }
 *  printf(">%s<\n", buffer); // => ">abcghi<"
 * \endcode
 *
 * @param to the output buffer
 * @param from the string to partially copy
 * @param to_limit the limit of *to* buffer (this is *to* + its size,
 * eg: `to + STR_SIZE(to)` like above)
 * @param n the number of bytes to write from *from* to *to* except if a `'\0'` is
 * found before.
 *
 * @return `NULL` if there is no such room to write the at most *n* bytes of *from*
 * **and** the trailing `'\0'` else a pointer on this leading nul byte.
 */
char *stpncpy_sp(char *to, const char *from, const char * const to_limit, size_t n)
{
    const char * const zero = to_limit - 1;

    // assert(NULL != to);
    // assert(NULL != from);
    assert(NULL != to_limit);

    if (NULL == to || to >= zero) {
        return NULL;
    }
    if (NULL != from) {
        while (to < zero && 0 != n && '\0' != (*to = *from)) {
            --n;
            ++to;
            ++from;
        }
    }
    *to = '\0'; // the n characters may not include a '\0'
    if (to == zero) {
        return NULL;
    } else {
        return to;
    }
}
