#ifndef DYN_STR_H
#define DYN_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* s;
    int length;
    int capacity;
}dyn_str;

//dynamic string initialization
dyn_str* dyn_str_init();

//String enlargement
dyn_str* dyn_str_grow(dyn_str *ds);

//add char at the end of the string  
dyn_str* dyn_str_append(dyn_str *ds, char c);

//free dynamic string
void dyn_str_free(dyn_str *ds);


#endif  // DYN_STR_H