char *stpcpy_s(char *to, const char *from, const char * const zero)
{
    const char * const end = zero - 1;

    if (NULL == to || to >= end) {
        return NULL;
    }
    if (NULL != from) {
        while (to < end && '\0' != (*to++ = *from++))
            ;
    }
    if (to == end) {
        *to = '\0';
        return NULL;
    } else {
        return to - 1;
    }
}
