/*
file: scanner.h
author: Marek Hric
Header file for the scanner.
*/

#ifndef SCANNER_H
#define SCANNER_H


#include "token.h"
#include "error.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


enum State {
    S_START,
    S_END,
    S_ERROR,
    
    S_ID,

    S_UNDERSCORE,
    S_AT_IMPORT,

    //OPERATORS
    S_ASGN, 
    S_BANG,
    S_LESS,
    S_MORE,

    //NUMBERS
    S_ZERO,
    S_INT,
    S_F64, S_F64_DOT, S_F64_E, S_F64_EXP,

    //LITERALS
    S_STR, S_STR_MLINE,
    S_STR_ESC, S_STR_HEX,
};

// Function prototypes
Token *get_token(FILE* file);
void free_token(Token* token);

#endif // SCANNER_H
