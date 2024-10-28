/*
file: scanner_tests.h
authors: Marek Hric
Header file for scanner tests.
*/

#ifndef SCANNER_TESTS_H
#define SCANNER_TESTS_H

#include <criterion/criterion.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include "scanner.h"

#define TOKEN_N(T) {.type = (T), .value = {0}}
#define TOKEN_S(T, V) {.type = (T), .value = {.string_value = (V)}}
#define TOKEN(T, V) _Generic((V), \
    int: (Token){.type = (T), .value = {.int_value = (V)}}, \
    float: (Token){.type = (T), .value = {.float_value = (V)}}, \
    double: (Token){.type = (T), .value = {.float_value = (V)}}, \
    KeyWordType: (Token){.type = (T), .value = {.keyword = (V)}}, \
    default: (Token){.type = (T)} \
)

#define N  (int)(sizeof(expected_tokens) / sizeof(Token))

void assert_token_eq(Token *compared, Token *expected)
{
    cr_assert(compared->type == expected->type, "Expected token type %d, got %d", expected->type, compared->type);
    if (expected->type == T_INT)
        cr_assert(compared->value.int_value == expected->value.int_value, "Expected int value %d, got %d", expected->value.int_value, compared->value.int_value);
    else if (expected->type == T_FLOAT)
        cr_assert(compared->value.float_value == expected->value.float_value, "Expected float value %f, got %f", expected->value.float_value, compared->value.float_value);
    else if (expected->type == T_ID || expected->type == T_STR)
        cr_assert(strcmp(compared->value.string_value, expected->value.string_value) == 0, "Expected string value %s, got %s", expected->value.string_value, compared->value.string_value);
    else if (expected->type == T_KW)
        cr_assert(compared->value.keyword == expected->value.keyword, "Expected keyword value %d, got %d", expected->value.keyword, compared->value.keyword);
}

#endif // SCANNER_TESTS_H
