#include <stddef.h> /* size_t */

#include "utils.h" /* MIN */

/**
 * XXX
 *
 * @param str1
 * @param str1_len
 * @param str2
 * @param str2_len
 * @param n
 *
 * @return
 */
int memncmp_l(
    const char *str1, size_t str1_len,
    const char *str2, size_t str2_len,
    size_t n
) {
    if (str1 != str2 && 0 != n) {
        size_t min_len;

        min_len =  MIN(n, MIN(str1_len, str2_len));
        while (0 != min_len--) {
            if (*str1 != *str2) {
                return *(const unsigned char *) str1 - *(const unsigned char *) str2;
            }
            ++str1, ++str2;
        }

        return MIN(n, str1_len) - MIN(n, str2_len);
    }

    return 0;
}
