#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "attributes.h"

bool str_vconcat_s(char **, char *, const char * const, ...) SENTINEL;
