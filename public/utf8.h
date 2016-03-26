#pragma once

#include <stddef.h> /* size_t */
#include <stdbool.h>

#include "unicode.h"

#define UTF8_MAX_CU_LENGTH 8

bool utf8_check(const char *, size_t, const char **);
size_t utf8_read_cp(const uint8_t *, size_t, codepoint *);
size_t utf8_write_cp(codepoint, uint8_t *, size_t);
