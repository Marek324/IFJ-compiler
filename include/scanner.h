/*
file: scanner.h
author: Marek Hric xhricma00
Header file for the scanner.
*/

#ifndef SCANNER_H
#define SCANNER_H

#include "circ_buff.h"
#include "token.h"
#include "error.h"
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
    S_AT,

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

/**
 * get_token
 * buffer - used for saving characters for later use
 * reads stdin and returns a token
 */
Token *get_token(circ_buff_ptr buffer);

/**
 * frees a token
 * used other modules
 */
void free_token(Token* token);

/**
 * read_char
 * buffer - used for saving characters for later use
 * reads character from buffer if not empty, stdin otherwise
 * returns a character
 */
int read_char(circ_buff_ptr buffer);

/**
 * unget_char
 * used for checking if read id is not a reserved keyword
 */
KeyWordType check_keyword(char *str);

#endif // SCANNER_H
