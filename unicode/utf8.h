#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef int32_t codepoint;

bool utf8_check(const char *, size_t, const char **);
size_t utf8_read_cp(const uint8_t *, size_t, codepoint *);
size_t utf8_write_cp(codepoint, const uint8_t *, size_t);
