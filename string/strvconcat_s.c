#include <stddef.h>
#include <assert.h>

#include "strvconcat_s.h"
#include "error.h"
#include "stpcpy_s.h"

/**
 * Concatenates several strings together in a buffer
 *
 * \code
 *  #define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
 *  #define STR_SIZE(str) (ARRAY_SIZE(str))
 *
 *  char *error, buffer[MAXPATHLEN];
 *  const char * const buffer_end = buffer + STR_SIZE(buffer);
 *
 *  error = NULL;
 *  if (!strvconcat_s(&error, buffer, buffer_end, "abc", "def", "ghi", NULL)) {
 *      // buffer overflow
 *      fprintf(stderr, "error: %s", error);
 *      error_free(&error);
 *  } else {
 *      printf(">%s<\n", buffer); // => ">abcdefghi<"
 *  }
 * \endcode
 *
 * @param error TODO
 * @param buffer the buffer which receives the result of the concatenation
 * @param buffer_end the end of *buffer* (this is *buffer* + its size,
 * eg: `buffer + STR_SIZE(buffer)` like above)
 * @param ... a `NULL`-terminated list of strings to concatenate
 *
 * @return `false` if the capacity of *buffer* was insufficient
 */
bool str_vconcat_s(char **error, char *buffer, const char * const buffer_end, ...) /* SENTINEL */
{
    char *w;
    bool ret;
    va_list ap;
    const char *s;

    assert(NULL != buffer);
    assert(NULL != buffer_end);

    ret = true;
    w = buffer;
    va_start(ap, buffer_end);
    while (ret && NULL != (s = va_arg(ap, const char *))) {
        char *oldw;

        oldw = w;
        ret = NULL != (w = stpcpy_sp(w, s, buffer_end));
        if (!ret) {
            set_buffer_overflow_error(error, s, w, buffer_end - oldw);
        }
    }
    va_end(ap);

    return ret;
}
