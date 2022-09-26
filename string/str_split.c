#include <stddef.h>
#include <stdlib.h> /* malloc */
#include <assert.h>

#include "str_split.h"

size_t str_split(char separator, char *string, char ***parts, bool no_empty)
{
    char *p;
    size_t count, i; // count = number of separator occurrences

    assert(NULL != string);
    assert(NULL != parts);

    i = count = 0;
    if (no_empty) {
        while (*string == separator) {
            ++string;
        }
    }
    p = string;
    for (p = string; '\0' != *p; p++) {
        if (*p == separator) {
            if (no_empty) {
                while (*++p == separator)
                    ;
            }
            ++count;
        }
    }
    *parts = malloc(sizeof(**parts) * (count + 2)); // + 1 for the difference between number of separator and parts ; + 1 for the sentinel for convenience
    // TODO: check NULL != *parts
    (*parts)[i++] = string;
    while ('\0' != *string) {
        if (*string == separator) {
            *string++ = '\0';
            if (no_empty) {
                while (*string == separator) {
                    ++string;
                }
            }
            (*parts)[i++] = string/* + 1*/;
        } else {
            ++string;
        }
    }
    (*parts)[i] = NULL;

    return i;
}
