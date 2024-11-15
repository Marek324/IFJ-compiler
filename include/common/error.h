/*
file: error.h
author: Marek Hric
Header file for error message.
*/

#ifndef ERROR_H
#define ERROR_H

char *lex_error_messages = {
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

void error_exit(int exit_code, const char *fmt, ...);

#endif // ERROR_H
