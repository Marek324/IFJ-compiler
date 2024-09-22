#ifndef DYN_STR_H
#define DYN_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \brief Dynamic string
 * 
 * \param s String
 * \param length Current length of dynamic string
 * \param capacity Capacity of dynamic string         *PLEASE SPECIFY*
 */
typedef struct {
    char* s;
    int length;
    int capacity;
} dyn_str;

/**
 * \brief Dynamic string initialization
 * 
 * \return Pointer to the newly allocated dynamic string
 */
dyn_str* dyn_str_init();

/**
 * \brief String enlargement
 * 
 * \param[in, out] ds Pointer to dynamic string that will be enlarged
 * \return Pointer to the newly enlarged dynamic string
 */
dyn_str* dyn_str_grow(dyn_str *ds);

/**
 * \brief Adds a character to the end of the string
 * 
 * \param[in, out] ds Pointer to dynamic string we want to add to
 * \param[in] c Character that will be appended to the dynamic string
 * \return Pointer to the expanded dynamic string
 */
dyn_str* dyn_str_append(dyn_str *ds, char c);

/**
 * \brief Frees dynamic string
 * 
 * \param[in] ds Pointer to dynamic string that will be freed
 */
void dyn_str_free(dyn_str *ds);


#endif  // DYN_STR_H