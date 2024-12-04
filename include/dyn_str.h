/**
 * file: dyn_str.h
 * authors: Mikuláš Lešiga xlesigm00
 * Marek Hric xhricma00
 * Header file for the dynamic string structures and its functions.
*/

#ifndef DYN_STR_H
#define DYN_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dyn_str{
    char* str;
    int length;
    int capacity;
} dyn_str;

/**
 * dyn_str_init - initializes a new dynamic string
 */
dyn_str* dyn_str_init();

/**
 * dyn_str_append - appends a character to the dynamic string
 * ds - dynamic string
 * c - character to append
 * return pointer to the dynamic string
 */
dyn_str* dyn_str_append(dyn_str* ds, char c);

/**
 * dyn_str_append_str - appends a string to the dynamic string
 * ds - dynamic string
 * str - string to append
 * return pointer to the dynamic string
 */
dyn_str* dyn_str_append_str(dyn_str* ds, char* str);

/**
 * dyn_str_free - frees the dynamic string
 */
void dyn_str_free(dyn_str* ds);

#endif  // DYN_STR_H
