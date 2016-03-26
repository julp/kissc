
char *strcpy(char *to, const char *from) /* DEPRECATED */
{
    do {
        *to++ = *from++;
    } while ('\0' != *from);

    return to;
}
