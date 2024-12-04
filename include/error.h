/**
 * file: error.h
 * author: Marek Hric xhricma00
 * Header file for error message.
 */

#ifndef ERROR_H
#define ERROR_H

/**
 * error_exit
 * exit_code - exit code for the program
 * fmt - format string for the error message
 * prints error message and exits the program
 */
void error_exit(int exit_code, const char *fmt, ...);

#endif // ERROR_H
