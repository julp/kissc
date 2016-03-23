#pragma once

#include <stddef.h>
#include <stdbool.h>

#ifdef WITH_STRCPY
char *strcpy(char *, const char *) DEPRECATED;
#endif /* WITH_STRCPY */

bool strcpy_sp(char *, const char * const, const char *);
