/*
file: scanner.h
author: Marek Hric
Header file for the scanner.
*/

#ifndef SCANNER_H
#define SCANNER_H


#include "token.h"
#include "error.h"


enum State {
    S_START,
    S_END,
    S_ERROR,
    
    S_ID,
    S_KW,

    S_UNDERSCORE,
    S_AT_IMPORT,

    //OPERATORS
    S_ASGN, S_EQ, 
    S_BANG, S_NEQ,
    S_LESS, S_LEQ,
    S_MORE, S_MEQ,

    S_PLUS, S_PASGN,
    S_MINUS, S_MASGN,
    S_MUL, S_MULASGN,
    S_SLASH, S_DASGN,

    //NUMBERS
    S_ZERO,
    S_INT,
    S_F64, S_F64_DOT, S_F64_E, S_F64_E1, S_F64_E2,

    //DELIMITERS
    S_DOT,
    S_COMMA,
    S_SEMICOL,
    S_LPAREN, S_RPAREN,
    S_LBRACE, S_RBRACE,
    S_LBRACKET, S_RBRACKET,

    //LITERALS
    S_STR, S_STR_ESC,

    //COMMENTS
    S_COMM   
};

// Function prototypes
Token *get_token(FILE* file, int *line);
void free_token(Token* token);

#endif // SCANNER_H
