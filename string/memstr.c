#include <string.h>

/**
 * Find the first occurrence of a string in an other one
 *
 * @param haystack the string to search in
 * @param needle the substring to look for
 * @param needle_len the length of *needle*
 * @param haystack_end a pointer after the last byte of haystack (eg: `haystack + strlen(haystack)` - if *haystack* is `'\0'`-terminated)
 *
 * @return a pointer to the first match found of *needle* in *haystack* else `NULL` if not found
 */
char *memstr(const char *haystack, const char *needle, size_t needle_len, const char * const haystack_end)
{
    if (needle_len <= (size_t) (haystack_end - haystack)) {
        if (0 == needle_len) {
            return (char *) haystack;
        } else if (1 == needle_len) {
            return (char *) memchr(haystack, (int) needle[0], haystack_end - haystack);
        } else {
            const char *p;
            const char * const l = haystack_end - needle_len; // last possible position to find a match

            for (p = haystack; p <= l; p++) {
                if (*p == needle[0] && 0 == memcmp(needle + 1, p + 1, needle_len - 1)) {
                    return (char *) p;
                }
            }
        }
    }

    return NULL;
}
