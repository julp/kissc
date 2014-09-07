#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "parsenum.h"

// TODO: temporary
#ifndef FALSE
# define FALSE 0
#endif /* !FALSE */
#ifndef TRUE
# define TRUE 1
#endif /* !TRUE */

/**
 * - PARSE_NUM_NO_ERR: all characters of the string were consumed to convert it into an integer ; *endptr, if given, is NULL since there wasn't any error
 *
 * - PARSE_NUM_ERR_INVALID_BASE: base parameter is invalide ; *endptr, if given, is NULL since the string wasn't parsed at all
 *
 * - PARSE_NUM_ERR_NO_DIGIT_FOUND: the given string contains no digit at all, meaning we reach the end of the string without finding a first digit for
 *   the conversion into integer. Empty strings ("") as empty prefixed binary (0b) and hexadecimal (0x) numbers ("0x" as "0b") will throw this error.
 *   There is an exception: "0" with base auto-detection (base = 0) will be recognized as a (octal) zero without throwing this error.
 *   *endptr, if given, is NULL since there is nothing to point out (the fact is that something is missing)
 *
 * - PARSE_NUM_ERR_NON_DIGIT_FOUND: conversion failed on a character which can't be a digit (according to the actual base). *endptr will be set to point
 *   on it.
 *
 * - PARSE_NUM_ERR_TOO_SMALL:
 * - PARSE_NUM_ERR_TOO_LARGE:
 *   the number is valid but is out of physical limits
 *   *endptr, if given, is NULL since string parsing was stopped because current number gets out of physical limit of output type
 *
 * - PARSE_NUM_ERR_LESS_THAN_MIN:
 * - PARSE_NUM_ERR_GREATER_THAN_MAX:
 *   the number is valid but is out of user limits
 *   *endptr, if given, is NULL since parsing was successful
 **/

#define parse_signed(type, unsigned_type, value_type_min, value_type_max) \
    ParseNumError strto## type(const char *nptr, char **endptr, int base, type *min, type *max, type *ret) { \
        return strnto## type(nptr, nptr + strlen(nptr), endptr, base, min, max, ret); \
    } \
 \
    ParseNumError strnto## type(const char *nptr, const char * const end, char **endptr, int base, type *min, type *max, type *ret) { \
        char c; \
        int negative; \
        const char *s; \
        int any, cutlim; \
        ParseNumError err; \
        unsigned_type cutoff, acc; \
 \
        s = nptr; \
        acc = any = 0; \
        *endptr = NULL; \
        negative = FALSE; \
        err = PARSE_NUM_NO_ERR; \
        if (s < end) { \
            if ('-' == *s) { \
                ++s; \
                negative = TRUE; \
            } else { \
                negative = FALSE; \
                if ('+' == *s) { \
                    ++s; \
                } \
            } \
            if ((0 == base || 2 == base) && '0' == *s && (end - s) > 1 && ('b' == s[1] || 'B' == s[1])) { \
                s += 2; \
                base = 2; \
            } \
            if ((0 == base || 16 == base) && '0' == *s && (end - s) > 1 && ('x' == s[1] || 'X' == s[1])) { \
                s += 2; \
                base = 16; \
            } \
            if (0 == base) { \
                base = '0' == *s ? 8 : 10; \
            } \
            if (base < 2 || base > 36) { \
                return PARSE_NUM_ERR_INVALID_BASE; \
            } \
            cutoff = negative ? (unsigned_type) - (value_type_min + value_type_max) + value_type_max : value_type_max; \
            cutlim = cutoff % base; \
            cutoff /= base; \
            while (s < end) { \
                if (*s >= '0' && *s <= '9') { \
                    c = *s - '0'; \
                } else if (base > 10 && *s >= 'A' && *s <= 'Z') { \
                    c = *s - 'A' - 10; \
                } else if (base > 10 && *s >= 'a' && *s <= 'z') { \
                    c = *s - 'a' - 10; \
                } else { \
                    *endptr = (char *) s; \
                    if (NULL != endptr) { \
                        err = PARSE_NUM_ERR_NON_DIGIT_FOUND; \
                    } \
                    break; \
                } \
                if (c >= base) { \
                    *endptr = (char *) s; \
                    if (NULL != endptr) { \
                        err = PARSE_NUM_ERR_NON_DIGIT_FOUND; \
                    } \
                    break; \
                } \
                if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) { \
                    any = -1; \
                } else { \
                    any = 1; \
                    acc *= base; \
                    acc += c; \
                } \
                ++s; \
            } \
        } \
        if (any < 0) { \
            if (negative) { \
                *ret = value_type_min; \
                return PARSE_NUM_ERR_TOO_SMALL; \
            } else { \
                *ret = value_type_max; \
                return PARSE_NUM_ERR_TOO_LARGE; \
            } \
        } else if (!any && PARSE_NUM_NO_ERR == err) { \
            err = PARSE_NUM_ERR_NO_DIGIT_FOUND; \
        } else if (negative) { \
            *ret = -acc; \
        } else { \
            *ret = acc; \
        } \
        if (PARSE_NUM_NO_ERR == err) { \
            if (NULL != min && *ret < *min) { \
                err = PARSE_NUM_ERR_LESS_THAN_MIN; \
            } \
            if (NULL != max && *ret > *max) { \
                err = PARSE_NUM_ERR_GREATER_THAN_MAX; \
            } \
        } \
 \
        return err; \
    }

parse_signed(int8_t, uint8_t, INT8_MIN, INT8_MAX);
parse_signed(int16_t, uint16_t, INT16_MIN, INT16_MAX);
parse_signed(int32_t, uint32_t, INT32_MIN, INT32_MAX);
parse_signed(int64_t, uint64_t, INT64_MIN, INT64_MAX);

#undef parse_signed

#define parse_unsigned(type, value_type_max) \
    ParseNumError strto## type(const char *nptr, char **endptr, int base, type *min, type *max, type *ret) { \
        return strnto## type(nptr, nptr + strlen(nptr), endptr, base, min, max, ret); \
    } \
 \
    ParseNumError strnto## type(const char *nptr, const char * const end, char **endptr, int base, type *min, type *max, type *ret) { \
        char c; \
        int negative; \
        const char *s; \
        int any, cutlim; \
        type cutoff, acc; \
        ParseNumError err; \
 \
        s = nptr; \
        acc = any = 0; \
        *endptr = NULL; \
        negative = FALSE; \
        err = PARSE_NUM_NO_ERR; \
        if (s < end) { \
            if ('-' == *s) { \
                ++s; \
                negative = TRUE; \
            } else { \
                negative = FALSE; \
                if ('+' == *s) { \
                    ++s; \
                } \
            } \
            if ((0 == base || 2 == base) && '0' == *s && (end - s) > 1 && ('b' == s[1] || 'B' == s[1])) { \
                s += 2; \
                base = 2; \
            } \
            if ((0 == base || 16 == base) && '0' == *s && (end - s) > 1 && ('x' == s[1] || 'X' == s[1])) { \
                s += 2; \
                base = 16; \
            } \
            if (0 == base) { \
                base = '0' == *s ? 8 : 10; \
            } \
            if (base < 2 || base > 36) { \
                return PARSE_NUM_ERR_INVALID_BASE; \
            } \
            cutoff = value_type_max / base; \
            cutlim = value_type_max % base; \
            while (s < end) { \
                if (*s >= '0' && *s <= '9') { \
                    c = *s - '0'; \
                } else if (base > 10 && *s >= 'A' && *s <= 'Z') { \
                    c = *s - 'A' - 10; \
                } else if (base > 10 && *s >= 'a' && *s <= 'z') { \
                    c = *s - 'a' - 10; \
                } else { \
                    *endptr = (char *) s; \
                    if (NULL != endptr) { \
                        err = PARSE_NUM_ERR_NON_DIGIT_FOUND; \
                    } \
                    break; \
                } \
                if (c >= base) { \
                    *endptr = (char *) s; \
                    if (NULL != endptr) { \
                        err = PARSE_NUM_ERR_NON_DIGIT_FOUND; \
                    } \
                    break; \
                } \
                if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) { \
                    any = -1; \
                } else { \
                    any = 1; \
                    acc *= base; \
                    acc += c; \
                } \
                ++s; \
            } \
        } \
        if (any < 0) { \
            *ret = value_type_max; \
            return PARSE_NUM_ERR_TOO_LARGE; \
        } else if (!any && PARSE_NUM_NO_ERR == err) { \
            err = PARSE_NUM_ERR_NO_DIGIT_FOUND; \
        } else if (negative) { \
            *ret = -acc; \
        } else { \
            *ret = acc; \
        } \
        if (PARSE_NUM_NO_ERR == err) { \
            if (NULL != min && *ret < *min) { \
                err = PARSE_NUM_ERR_LESS_THAN_MIN; \
            } \
            if (NULL != max && *ret > *max) { \
                err = PARSE_NUM_ERR_GREATER_THAN_MAX; \
            } \
        } \
 \
        return err; \
    }

parse_unsigned(uint8_t, UINT8_MAX);
parse_unsigned(uint16_t, UINT16_MAX);
parse_unsigned(uint32_t, UINT32_MAX);
parse_unsigned(uint64_t, UINT64_MAX);

#undef parse_unsigned
