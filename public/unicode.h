#pragma once

#include <stdint.h> /* uint32_t */

#define UNICODE_HIGHEST_CODE_POINT 0x10FFFF
#define UNICODE_REPLACEMENT_CHARACTER 0xFFFD

typedef uint32_t codepoint;

typedef enum {
    UNI_ERR_OK,
    UNI_ERR_INVALID,
    UNI_ERR_TRUNCATED,
    UNI_ERR_BUFFER_OVERFLOW,
} UnicodeError;
