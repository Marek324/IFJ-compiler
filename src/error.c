/*
file: error.c
author: Marek Hric
Implementation of error messages.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../include/error.h"

void error_exit(int exit_code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(exit_code);
}
