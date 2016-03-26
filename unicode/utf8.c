#include "utf8.h"
#include "cpp/utils.h"

#define U8(c) \
    ((uint8_t) (c))

#define U32(c) \
    ((uint32_t) (c))

#undef I
#undef R

#define I -1 /* Illegal */
#define R -2 /* Out of range */

static const int8_t utf8_count_bytes[256] = {
    /*      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F */
    /* 0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 1 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 2 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 4 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 6 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 8 */ I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,
    /* 9 */ I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,
    /* A */ I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,
    /* B */ I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,
    /* C */ I, I, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    /* D */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    /* E */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    /* F */ 4, 4, 4, 4, R, R, R, R, R, R, R, R, R, R, R, R
};

#undef I
#undef R

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

// #define PRECOMPUTED

#ifdef PRECOMPUTED

# ifndef UTF8_UNSAFE
#  define BAD_CP_MASK 0x200000
#  define E(x) (x | BAD_CP_MASK)
# else
#  define E(x) x
# endif /* !UTF8_UNSAFE */

enum {
    UTF8_FIRST_BYTE,
    UTF8_LAST_BYTE,
    UTF8_BEFORE_LAST_BYTE,
    UTF8_4_2,
//     UTF8_1_0 = UTF8_FIRST_BYTE,
//     UTF8_2_0 = UTF8_FIRST_BYTE,
//     UTF8_3_0 = UTF8_FIRST_BYTE,
//     UTF8_4_0 = UTF8_FIRST_BYTE,
//     UTF8_2_2 = UTF8_LAST_BYTE,
//     UTF8_3_2 = UTF8_BEFORE_LAST_BYTE,
//     UTF8_3_3 = UTF8_LAST_BYTE,
//     UTF8_4_3 = UTF8_BEFORE_LAST_BYTE,
//     UTF8_4_4 = UTF8_LAST_BYTE,
};

static const uint32_t cumask[][256] = {
//     [ S(__) ] = { [0 ... 0xFF ] = E(0) },
    // first byte
    [ UTF8_FIRST_BYTE ] = {
#if 0
php -r 'foreach (range(0xC2, 0xDF) as $c) { if ($c == ($c & 0xF0)) echo PHP_EOL; printf("0x%X, ", ($c & 0b00011111) << 6); }'
php -r 'foreach (range(0xE0, 0xEF) as $c) { if ($c == ($c & 0xF0)) echo PHP_EOL; printf("0x%X, ", ($c & 0b00001111) << 12); }'
php -r 'foreach (range(0xF0, 0xF4) as $c) { if ($c == ($c & 0xF0)) echo PHP_EOL; printf("0x%X, ", ($c & 0b00000111) << 18); }'
#endif
        /* 0 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        /* 1 */ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        /* 2 */ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        /* 3 */ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        /* 4 */ 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        /* 5 */ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        /* 6 */ 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        /* 7 */ 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        /* 8 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 9 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* A */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* B */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* C */ E(0), E(0), 0x80, 0xC0, 0x100, 0x140, 0x180, 0x1C0, 0x200, 0x240, 0x280, 0x2C0, 0x300, 0x340, 0x380, 0x3C0,
        /* D */ 0x400, 0x440, 0x480, 0x4C0, 0x500, 0x540, 0x580, 0x5C0, 0x600, 0x640, 0x680, 0x6C0, 0x700, 0x740, 0x780, 0x7C0,
        /* E */ 0x0, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000, 0xF000,
        /* F */ 0x0, 0x40000, 0x80000, 0xC0000, 0x100000, E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
    },
    // last byte when length > 1
    [ UTF8_LAST_BYTE ] = {
#if 0
php -r 'foreach (range(0x80, 0xBF) as $c) { if ($c == ($c & 0xF0)) echo PHP_EOL; printf("0x%X, ", $c & 0b00111111); }'
#endif
        /* 0 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 1 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 2 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 3 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 4 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 5 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 6 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 7 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 8 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        /* 9 */ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        /* A */ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        /* B */ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        /* C */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* D */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* E */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* F */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
    },
    // before last byte when length > 2
    [ UTF8_BEFORE_LAST_BYTE ] = {
#if 0
php -r 'foreach (range(0x80, 0xBF) as $c) { if ($c == ($c & 0xF0)) echo PHP_EOL; printf("0x%X, ", ($c & 0b00111111) << 6); }'
#endif
        /* 0 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 1 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 2 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 3 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 4 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 5 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 6 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 7 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 8 */ 0x0, 0x40, 0x80, 0xC0, 0x100, 0x140, 0x180, 0x1C0, 0x200, 0x240, 0x280, 0x2C0, 0x300, 0x340, 0x380, 0x3C0,
        /* 9 */ 0x400, 0x440, 0x480, 0x4C0, 0x500, 0x540, 0x580, 0x5C0, 0x600, 0x640, 0x680, 0x6C0, 0x700, 0x740, 0x780, 0x7C0,
        /* A */ 0x800, 0x840, 0x880, 0x8C0, 0x900, 0x940, 0x980, 0x9C0, 0xA00, 0xA40, 0xA80, 0xAC0, 0xB00, 0xB40, 0xB80, 0xBC0,
        /* B */ 0xC00, 0xC40, 0xC80, 0xCC0, 0xD00, 0xD40, 0xD80, 0xDC0, 0xE00, 0xE40, 0xE80, 0xEC0, 0xF00, 0xF40, 0xF80, 0xFC0,
        /* C */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* D */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* E */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* F */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
    },
    // 2nd byte when length = 4
    [ UTF8_4_2 ] = {
#if 0
php -r 'foreach (range(0x80, 0xBF) as $c) { if ($c == ($c & 0xF0)) echo PHP_EOL; printf("0x%X, ", ($c & 0b00111111) << 12); }'
#endif
        /* 0 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 1 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 2 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 3 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 4 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 5 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 6 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 7 */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* 8 */ 0x0, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000, 0xF000,
        /* 9 */ 0x10000, 0x11000, 0x12000, 0x13000, 0x14000, 0x15000, 0x16000, 0x17000, 0x18000, 0x19000, 0x1A000, 0x1B000, 0x1C000, 0x1D000, 0x1E000, 0x1F000,
        /* A */ 0x20000, 0x21000, 0x22000, 0x23000, 0x24000, 0x25000, 0x26000, 0x27000, 0x28000, 0x29000, 0x2A000, 0x2B000, 0x2C000, 0x2D000, 0x2E000, 0x2F000,
        /* B */ 0x30000, 0x31000, 0x32000, 0x33000, 0x34000, 0x35000, 0x36000, 0x37000, 0x38000, 0x39000, 0x3A000, 0x3B000, 0x3C000, 0x3D000, 0x3E000, 0x3F000,
        /* C */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* D */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* E */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
        /* F */ E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0), E(0),
    },
};
#endif /* PRECOMPUTED */

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

// TODO: check for truncated code point (if so, set *cp to U+FFFD)
size_t utf8_read_cp(const uint8_t *string, size_t string_len, codepoint *cp)
{
    const uint8_t *r;

    r = (uint8_t *) string;
#ifdef PRECOMPUTED
    *cp = cumask[UTF8_FIRST_BYTE][*r++];
    if (*cp > 0x7F) {
        if (*cp < 0xE0) {
            *cp |= cumask[UTF8_LAST_BYTE][*r++];
        } else if (*cp < 0xF0) {
            *cp |= cumask[UTF8_BEFORE_LAST_BYTE][*r++] | cumask[UTF8_LAST_BYTE][*r++];
        } else {
            *cp |= cumask[UTF8_4_2][*r++] | cumask[UTF8_BEFORE_LAST_BYTE][*r++] | cumask[UTF8_LAST_BYTE][*r++];
        }
    }
# ifndef UTF8_UNSAFE
    if (HAS_FLAG(*cp, BAD_CP_MASK)) {
        *cp = UNICODE_REPLACEMENT_CHARACTER;
    }
# endif /* !UTF8_UNSAFE */
#else
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
#endif

    return r - string;
}

// TODO: buffer overflow
size_t utf8_write_cp(codepoint cp, uint8_t *string, size_t string_size)
{
    uint8_t *w;

    w = string;
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
