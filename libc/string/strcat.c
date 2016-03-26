
char *strcat(char *to, const char *suffix) /* DEPRECATED */
{
    char *w;

    for (w = to; '\0' != *w; w++)
        ;
    while ('\0' != (*w++ = *suffix++))
        ;

    return to;
}
