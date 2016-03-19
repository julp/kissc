#include <stdlib.h>
#include "utils.h"

int strcmp_l(
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
        while (min_len--/* > 0*/) {
            if (*str1 != *str2) {
                return (unsigned char) *str1 - (unsigned char) *str2;
            }
            ++str1, ++str2;
        }
    }

    return str1_len - str2_len;
}

int strncmp_l(
    const char *str1, size_t str1_len,
    const char *str2, size_t str2_len,
    size_t n
) {
    if (str1 != str2 && n/* > 0*/) {
        size_t min_len;

        min_len =  MIN(n, MIN(str1_len, str2_len));
        while (min_len-- > 0) {
            if (*str1 != *str2) {
                return *(const unsigned char *) str1 - *(const unsigned char *) str2;
            }
            ++str1, ++str2;
        }

        return MIN(n, str1_len) - MIN(n, str2_len);
    }

    return 0;
}
