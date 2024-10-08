#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "error.h"

void _error_set(char **error, const char *format, ...)
{
    if (NULL != error) {
        int len;
        va_list ap, aq;

        if (NULL != *error) {
            fprintf(stderr, "Warning: overwrite attempt of a previous error: %s\n", *error);
            error_free(error);
        }
        va_start(ap, format);
        va_copy(aq, ap);
        len = vsnprintf(NULL, 0, format, aq);
        va_end(aq);
        if (len >= 0) {
            int chk, size;

            size = len + 1;
            *error = malloc(size * sizeof(**error));
            assert(NULL != *error);
            chk = vsnprintf(*error, size, format, ap);
            assert(chk >= 0 && chk == len);
            (void) chk; // quiet warning variable 'chk' set but not used when assert is turned off
        }
        va_end(ap);
    }
}

void error_free(char **error)
{
    assert(NULL != error);

    free(*error);
    *error = NULL;
}
