int strcmp_l(
    const char *str1, size_t str1_len,
    const char *str2, size_t str2_len
) {
    size_t min_len;

    if (str1 != str2) {
        if (str2_len > str1_len) {
            min_len = str2_len;
        } else {
            min_len = str1_len;
        }
        while (min_len--) {
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
    int n
) {
    //
}
