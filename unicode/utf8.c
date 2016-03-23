#include "utf8.h"

#define U8(c) \
    ((uint8_t) (c))

#define U32(c) \
    ((uint32_t) (c))

#undef S
#define S(state) \
    STATE_##state

enum {
    S(__),       // error/invalid, have to be 0
    S(OK),       // accept
    S(FB),       // last byte (always in range [0x80;0xBF] for a code point encoded on more than a single byte
    S(32),       // normal case for the 2nd byte of a 3 bytes code point ([0x80;0xBF])
    S(32E0),     // still 3 bytes encoded code point but when 2nd byte is 0xE0, its range is restricted to [0xA0;0xBF]
    S(32ED),     // still 3 bytes encoded code point but when 2nd byte is 0xED, its range is restricted to [0x80;0x9F]
    S(42),       // normal case for the 2nd byte of a 4 bytes code point ([0x80;0xBF])
    S(42F0),     // still 4 bytes encoded code point but when 2nd byte is 0xF0, its range is restricted to [0x90;0xBF]
    S(42F4),     // still 4 bytes encoded code point but when 2nd byte is 0xF4, its range is restricted to [0x80;0x8F]
    S(43),       // 3rd byte of a 4 bytes encoded code point
    _STATE_COUNT // number of states
};

static const uint8_t state_transition_table[_STATE_COUNT][256] = {
    // handle first byte
    [ S(OK) ]   = {
        [ 0 ... 0x7F ] = S(OK),
        [ 0xC2 ... 0xDF ] = S(FB),
        [ 0xE0 ] = S(32E0), [ 0xE1 ... 0xEC ] = S(32), [ 0xED ] = S(32ED), [ 0xEE ... 0xEF ] = S(32),
        [ 0xF0 ] = S(42F0), [ 0xF1 ... 0xF3 ] = S(42), [ 0xF4 ] = S(42F4)
    },
    // final regular byte
    [ S(FB) ]   = { [ 0x80 ... 0xBF ] = S(OK) },
    // 3 bytes encoded code point
    [ S(32) ]   = { [ 0x80 ... 0xBF ] = S(FB) }, // 2nd byte, normal case
    [ S(32E0) ] = { [ 0xA0 ... 0xBF ] = S(FB) }, // 2nd byte, special case for 0xE0
    [ S(32ED) ] = { [ 0x80 ... 0x9F ] = S(FB) }, // 2nd byte, special case for 0xED
    // 4 bytes encoded code point
    [ S(42) ]   = { [ 0x80 ... 0xBF ] = S(43) }, // 2nd byte, normal case
    [ S(42F0) ] = { [ 0x90 ... 0xBF ] = S(43) }, // 2nd byte, special case for 0xF0
    [ S(42F4) ] = { [ 0x80 ... 0x8F ] = S(43) }, // 2nd byte, special case for 0xF4
    [ S(43) ]   = { [ 0x80 ... 0xBF ] = S(FB) }, // 3rd byte
};

/**
 * Check if a string is a valid UTF-8 string
 *
 * @param string the string to check
 * @param string_len its length
 * @param errp, optionnal (NULL to ignore), to have a pointer on the first
 * invalid byte found in the string (*errp is set to NULL if none)
 *
 * @return true if the string is a valid
 */
bool utf8_check(const char *string, size_t string_len, const char **errp)
{
    int state;
    const uint8_t *s;
    const uint8_t * const end = (const uint8_t *) string + string_len;

    state = S(OK); // accept empty string
    for (s = (const uint8_t *) string; S(__) != state && s < end; s++) {
        state = state_transition_table[state][*s];
    }
    if (NULL != errp) {
        if (S(OK) == state) {
            *errp = NULL;
        } else {
            *errp = (const char *) s;
        }
    }
#if 0
    if (S(OK) != state) {
        if (S(__) != state && s == end) {
            return TRUNCATED;
        } else {
            return INVALID;
        }
    } else {
        return OK;
    }
#else
    return S(OK) == state;
#endif
}

size_t utf8_read_cp(const uint8_t *string, size_t string_len, codepoint *cp)
{
    const uint8_t *r;

    r = (uint8_t *) string;
    *cp = U8(*r++);
    if (*cp > 0x7F) {
        if (*cp < 0xE0) {
            // [0x80;0x7FF]
            // 110x xxxx 10xx xxxx
            // (string[0] & 0b00011111) << 6 ... | (string[1] & 0b00111111)
            *cp = (*cp & 0b00011111) << 6;
        } else {
            if (*cp < 0xF0) {
                // [0x800;0xFFFF]
                // 1110 xxxx 10xx xxxx 10xx xxxx
                // (string[0] & 0b00001111) << 12 ... | (string[1] & 0b00111111) << 6 | (string[2] & 0b00111111)
                // NOTE: the & 0b00001111, to remove the two higher bits, is "replaced" by a cast into uint16_t for the same effect
                *cp = (uint16_t) (*cp << 12);
            } else {
                // [0x10000;0x10FFFF]
                // 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
                // (string[0] & 0b00000111) << 18 | (string[1] & 0b00111111) << 12 ... | (string[2] & 0b00111111) << 6 | (string[3] & 0b00111111)
                *cp = (*cp & 0b00000111) << 18 | (*r++ & 0b00111111) << 12;
            }
            *cp |= (*r++ & 0b00111111) << 6;
        }
        *cp |= (*r++ & 0b00111111);
    }

    return r - string;
}

size_t utf8_write_cp(codepoint cp, const uint8_t *string, size_t string_size)
{
    uint8_t *w;

    w = (uint8_t *) string;
    if (U32(cp) <= 0x7F) {
        // [0;0x7F]
        // 0xxx xxxx
        *w++ = U8(cp);
    } else {
        if (U32(cp) <= 0x7FF) {
            // [0x80;0x7FF]
            // 110x xxxx (10xx xxxx)
            *w++ = U8((cp >> 6) | 0b11000000);
        } else {
            if (U32(cp) <= 0xFFFF) {
                // [0x800;0xFFFF]
                // 1110 xxxx (10xx xxxx 10xx xxxx)
                *w++ = U8((cp >> 12) | 0b11100000);
            } else {
                // [0x10000;0x10FFFF]
                // 1111 0xxx 10xx xxxx (10xx xxxx 10xx xxxx)
                *w++ = U8((cp >> 18) | 0b11110000);
                *w++ = U8(((cp >> 12) & 0b00111111) | 0b10000000);
            }
            *w++ = U8(((cp >> 6) & 0b00111111) | 0b10000000);
        }
        *w++ = U8((cp & 0b00111111) | 0b10000000);
    }

    return w - string;
}
