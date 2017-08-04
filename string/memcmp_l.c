#include <stddef.h>

/**
 * XXX
 *
 * @param str1
 * @param str1_len
 * @param str2
 * @param str2_len
 *
 * @return
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
