/*
UT(
    test name,
    type,
    string to parse,
    expected integer value after parsing,
    expected returned value (see PARSE_NUM_ERR_* constants),
    base,
    min, // unused for now
    max, // unused for now
    expected stop offset
)
*/

UT(empty_string, int8_t, "", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN(""));
UT(empty_string, uint8_t, "", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN(""));

UT(just_unary_plus, int8_t, "+", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("+"));
UT(just_unary_plus, uint8_t, "+", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("+"));
UT(just_unary_minus, int8_t, "-", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("-"));
UT(just_unary_minus, uint8_t, "-", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("-"));

UT(unary_plus_number, int8_t, "+123", 123, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("+123"));
UT(unary_plus_number, uint8_t, "+123", 123, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("+123"));
UT(unary_minus_number, int8_t, "-123", -123, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("-123"));
UT(unary_minus_number, uint8_t, "-123", (uint8_t) -123, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("-123"));

UT(just_0b_base_auto, int8_t, "0b", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("0b"));
UT(just_0b_base_auto, uint8_t, "0b", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("0b"));
UT(just_0b_base2, int8_t, "0b", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 2, 0, 0, STR_LEN("0b"));
UT(just_0b_base2, uint8_t, "0b", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 2, 0, 0, STR_LEN("0b"));
UT(just_0b_base10, int8_t, "0b", 0, PARSE_NUM_ERR_NON_DIGIT_FOUND, 10, 0, 0, STR_LEN("0b") - 1);
UT(just_0b_base10, uint8_t, "0b", 0, PARSE_NUM_ERR_NON_DIGIT_FOUND, 10, 0, 0, STR_LEN("0b") - 1);

UT(just_0x_base_auto, int8_t, "0x", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("0x"));
UT(just_0x_base_auto, uint8_t, "0x", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 0, 0, 0, STR_LEN("0x"));
UT(just_0x_base16, int8_t, "0x", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 16, 0, 0, STR_LEN("0x"));
UT(just_0x_base16, uint8_t, "0x", 0, PARSE_NUM_ERR_NO_DIGIT_FOUND, 16, 0, 0, STR_LEN("0x"));
UT(just_0x_base10, int8_t, "0x", 0, PARSE_NUM_ERR_NON_DIGIT_FOUND, 10, 0, 0, STR_LEN("0x") - 1);
UT(just_0x_base10, uint8_t, "0x", 0, PARSE_NUM_ERR_NON_DIGIT_FOUND, 10, 0, 0, STR_LEN("0x") - 1);

UT(fine_5, int8_t, "5", 5, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("5"));
UT(fine_5, uint8_t, "5", 5, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("5"));
UT(fine_45, int8_t, "45", 45, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("45"));
UT(fine_45, uint8_t, "45", 45, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("45"));

UT(physical_too_large, int8_t, "129", INT8_MAX, PARSE_NUM_ERR_TOO_LARGE, 0, 0, 0, STR_LEN("129"));
UT(physical_too_large, uint8_t, "256", UINT8_MAX, PARSE_NUM_ERR_TOO_LARGE, 0, 0, 0, STR_LEN("256"));

UT(user_too_small, int8_t, "2", 2, PARSE_NUM_ERR_LESS_THAN_MIN, 0, 5, 0, STR_LEN("2"));
UT(user_too_small, uint8_t, "2", 2, PARSE_NUM_ERR_LESS_THAN_MIN, 0, 5, 0, STR_LEN("2"));

UT(invalid_number_on_first_char, int8_t, "j10", 0, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("j") - 1);
UT(invalid_number_on_first_char, uint8_t, "j10", 0, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("j") - 1);
UT(invalid_number_on_second_char, int8_t, "1j0", 1, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("1j") - 1);
UT(invalid_number_on_second_char, uint8_t, "1j0", 1, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("1j") - 1);

UT(truncation_on_non_digit, int8_t, "28:19", 28, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("28:") - 1);
UT(truncation_on_non_digit, uint8_t, "28:19", 28, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("28:") - 1);

UT(valid_auto_number, int8_t, "5", 5, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("5"));
UT(valid_auto_number, uint8_t, "5", 5, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("5"));
UT(valid_auto_binary, int8_t, "0b1001", 9, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("0b1001"));
UT(valid_auto_binary, uint8_t, "0b1001", 9, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("0b1001"));
UT(invalid_auto_binary, int8_t, "0b1201", 1, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("0b12") - 1);
UT(invalid_auto_binary, uint8_t, "0b1201", 1, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("0b12") - 1);

UT(zero_in_front_as_decimal, int32_t, "0123456789", 123456789, PARSE_NUM_NO_ERR, 10, 0, 0, STR_LEN("0123456789"));
UT(zero_in_front_as_decimal, uint32_t, "0123456789", 123456789, PARSE_NUM_NO_ERR, 10, 0, 0, STR_LEN("0123456789"));
UT(zero_in_front_as_auto, int32_t, "0123456789", 01234567, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("012345678") - 1);
UT(zero_in_front_as_auto, uint32_t, "0123456789", 01234567, PARSE_NUM_ERR_NON_DIGIT_FOUND, 0, 0, 0, STR_LEN("012345678") - 1);



UT(unary_minus_unsigned, uint8_t, "-1", UINT8_MAX, PARSE_NUM_NO_ERR, 0, 0, 0, STR_LEN("-1"));
