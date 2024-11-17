/*
file: error.c
author: Marek Hric
Implementation of error messages.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "common/error.h"

char *lex_error_messages[] = {
    "Memory allocation error",
    "Standalone backslash",
    "Unknown character",
    "Invalid @ keyword",
    "Leading zero",
    "Invalid float - no decimal part",
    "Invalid float - no exponent part",
    "No closing quote",
    "Invalid escape sequence",
    "Invalid hex number"
};

void error_exit(int exit_code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(exit_code);
}
