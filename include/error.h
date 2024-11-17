/*
file: error.h
author: Marek Hric
Header file for error message.
*/

#ifndef ERROR_H
#define ERROR_H

extern char *lex_error_messages[];

void error_exit(int exit_code, const char *fmt, ...);

#endif // ERROR_H
