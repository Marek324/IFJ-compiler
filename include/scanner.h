/*
file: scanner.h
author: Marek Hric
Header file for the scanner.
*/

#ifndef SCANNER_H
#define SCANNER_H


#include "token.h"
#include "circ_buff.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


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
    S_INT,
    S_FLOAT, S_FLOAT_EXP, S_FLOAT_DEC_EXP, 
    S_FLOAT_DOT, S_FLOAT_EXP_SIGN,

    //LITERALS
    S_STR, S_STR_MLINE, S_STR_MLINE_NEWLINE,
    S_STR_ESC, S_STR_HEX,
};

// Function prototypes
Token *get_token(circ_buff_ptr buffer);
void free_token(Token* token);

#endif // SCANNER_H
