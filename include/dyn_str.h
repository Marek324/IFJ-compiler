/*
file: dyn_str.h
authors: Mikuláš Lešiga, Marek Hric, Adam Veselý
Header file for the dynamic string structures and its functions.
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

dyn_str* dyn_str_init();
dyn_str* dyn_str_append(dyn_str* ds, char c);
dyn_str* dyn_str_append_str(dyn_str* ds, char* str);
void dyn_str_free(dyn_str* ds);

#endif  // DYN_STR_H
