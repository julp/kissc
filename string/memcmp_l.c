#include <stddef.h>

/**
 * Compare two substrings.
 * Since the length of each two is provided, you can compare
 * only part of the strings and/or strings which are not `'\0'`-terminated.
 *
 * @param str1 the first of the two substrings to compare
 * @param str1_len its length
 * @param str2 the second of the two substrings to compare
 * @param str2_len its length
 *
 * @return an integer as follows:
 *   - < 0 if *str1* is less than *str2*
 *   - 0 if *str1* equals *str2*
 *   - > 0 if *str1* is greater than *str2*
 */
int memcmp_l(
    const char *str1, size_t str1_len,
    const char *str2, size_t str2_len
) {
    if (str1 != str2) {
        size_t min_len;

        if (str2_len < str1_len) {
            min_len = str2_len;
        } else {
            min_len = str1_len;
        }
        while (0 != min_len--) {
            if (*str1 != *str2) {
                return (unsigned char) *str1 - (unsigned char) *str2;
            }
            ++str1, ++str2;
        }
    }

    return str1_len - str2_len;
}
