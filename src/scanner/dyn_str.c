/*
file: dyn_str.c
author: Mikuláš Lešiga
Implementation of dynamic string functions from dyn_str.h.
*/

#include "scanner/dyn_str.h"

dyn_str* dyn_str_init() {
    dyn_str* ds = malloc(sizeof(dyn_str));
    if (ds == NULL) {
        return NULL; 
    }
    
    ds->length = 0;
    ds->capacity = 0;
    ds->str = NULL;

    return ds;
}

dyn_str* dyn_str_grow(dyn_str* ds) {
    if (ds == NULL) {
        return NULL;
    }
    
    char* newStr = realloc(ds->str, ds->capacity == 0 ? 2 : 2*ds->capacity);
    if (newStr == NULL) {
        return NULL; 
    }

    ds->str = newStr;
    ds->capacity = ds->capacity == 0 ? 2 : 2*ds->capacity;

    return ds;
}

dyn_str* dyn_str_append(dyn_str* ds, char c) {
    // to ensure that the null terminator can fit
    if (ds->length >= ds->capacity - 1) {
        ds = dyn_str_grow(ds);
        if (ds == NULL) {
            return NULL;
        }
    }

    ds->str[ds->length++] = c;
    ds->str[ds->length] = '\0';

    return ds;
}

void dyn_str_free(dyn_str* ds) {
    if (ds != NULL) {
        free(ds->str);
        free(ds);
    }
}
