/**
 * \file dyn_str.h
 * 
 * \author Mikuláš Lešiga
 * \author Marek Hric
 * \author Adam Veselý
 * 
 * \brief Header file for the dynamic string structures and its functions.
 */

#ifndef DYN_STR_H
#define DYN_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
 * \brief Dynamic string structure.
 * 
 * \param s String.
 * \param length Current length of the string.
 * \param capacity Maximum amount of characters that can be in the string. (including the null terminator)
 */
typedef struct dyn_str{
    char* s;
    int length;
    int capacity;
} dyn_str;

/**
 * \brief Dynamic string structure initialization.
 * 
 * Allocates memory for the dynamic string structure and returns it as a pointer.
 * Sets the length and capacity of the dynamic string structure to 0 and the string to NULL.
 * 
 * \return Pointer to the newly allocated dynamic string structure.
 */
dyn_str* dyn_str_init();

/**
 * \brief Dynamic string enlargement.
 * 
 * Reallocates memory for the string in the dynamic string structure to
 * 2 bytes if the initial value is 0 or to double the current capacity of the dynamic string structure.
 * Updates the capacity of the dynamic string structure to 2 or double the capacity respectively.
 * 
 * \param[in, out] ds Pointer to dynamic string that will be enlarged.
 * \return Pointer to the newly enlarged dynamic string.
 */
dyn_str* dyn_str_grow(dyn_str* ds);

/**
 * \brief Adds a character to the end of the string.
 * 
 * If the dynamic string length is bigger than the capacity - 2 this function calls the dyn_str_grow function to increase the capacity.
 * Puts the new character at the end of the string, replacing the null terminator.
 * The null terminator is added after the new character and the dynamic string length is incremented by 1.
 * 
 * \param[in, out] ds Pointer to dynamic string we want to add to.
 * \param[in] c Character that will be appended to the dynamic string.
 * \return Pointer to the expanded dynamic string.
 */
dyn_str* dyn_str_append(dyn_str* ds, char c);

/**
 * \brief Frees the dynamic string structure.
 * 
 * Also frees the string in the dynamic string structure.
 * 
 * \param[in] ds Pointer to dynamic string structure that will be freed.
 */
void dyn_str_free(dyn_str* ds);


#endif  // DYN_STR_H

/*** End of file dyn_str.h ***/