#include "dyn_str.h"

dyn_str* dyn_str_init() {
    dyn_str *ds = malloc(sizeof(dyn_str));
    if (ds == NULL) {
        return NULL; 
    }
    
    ds->s = malloc(0);
    if (ds->s == NULL) {
        free(ds);  
        return NULL;
    }

    ds->length = 0;
    ds->capacity = 0;
    ds->s[0] = '\0';

    return ds;
}

dyn_str* dyn_str_grow(dyn_str *ds) {
    if (ds == NULL) {
        return NULL;
    }
    
    char* newStr = realloc(ds->s, ds->capacity == 0 ? 2 : 2*ds->capacity);
    if (newStr == NULL) {
        return NULL; 
    }

    ds->s = newStr;
    ds->capacity = ds->capacity == 0 ? 2 : 2*ds->capacity;  

    return ds;
}

dyn_str* dyn_str_append(dyn_str *ds, char c) {
    if (ds->length >= ds->capacity - 1) {
        ds = dyn_str_grow(ds);
        if (ds == NULL) {
            return NULL;
        }
    }

    ds->s[ds->length++] = c;
    ds->s[ds->length] = '\0';  

    return ds;
}

void dyn_str_free(dyn_str *ds) {
    if (ds != NULL) {
        free(ds->s);
        free(ds);
    }
}

