#pragma once

#include <stdbool.h>

#ifdef WITH_STRCAT
char *strcat(char *, const char *) DEPRECATED;
#endif /* WITH_STRCAT */

bool strcat_sp(char *, const char * const, const char *);
