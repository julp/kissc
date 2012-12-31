int strcmp_l(
    const char *str1, int32_t str1_len,
    const char *str2, int32_t str2_len
) {
    const char *end;

    if (str1 != str2) {
        if (str2_len > str1_len) {
            end = str2 + str2_len;
        } else {
            end = str1 + str1_len;
        }
        while (str1 <= end) {
            if (*str1 != *str2) {
                return (unsigned char) *str1 - (unsigned char) *str2;
            }
            ++str1, ++str2;
        }
    }

    return str1_len - str2_len;
}

int strncmp_l(
    const char *str1, int32_t str1_len,
    const char *str2, int32_t str2_len,
    int n
) {
    //
}
