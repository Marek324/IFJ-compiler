/**
 * file: token.h
 * author: Marek Hric xhricma00
 * Header file for tokens.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "dyn_str.h"

typedef enum {
    T_KW,
    T_ID,
    T_AT_IMPORT,
    T_AT_AS,

    T_AT,
    
    T_INT,
    T_FLOAT,
    T_STR,

    T_ASGN, T_EQ,
    T_BANG, T_NEQ,
    T_LESS, T_LEQ,
    T_MORE, T_MEQ,

    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_IDIV,

    T_LPAREN, T_RPAREN,
    T_LBRACE, T_RBRACE,
    T_LBRACKET, T_RBRACKET,

    T_COMMA,
    T_DOT,
    T_SEMICOL,
    T_COLON,
    T_PIPE,
    T_QMARK,
 
    T_EOF,
    T_I2F,
    T_F2I,

} TokenType;

typedef enum {
    KW_CONST,
    KW_IF,
    KW_ELSE,
    KW_FN,
    KW_I32,
    KW_F64,
    KW_NULL,
    KW_PUB,
    KW_RETURN,
    KW_U8,
    KW_VAR,
    KW_VOID,
    KW_FOR,
    KW_WHILE,
    KW_ORELSE,
    KW_UNREACHABLE,
    KW_TRUE,
    KW_FALSE,
    KW_BOOL,
    KW_AND,
    KW_OR,
    KW_BREAK,
    KW_CONTINUE,
    NO_KW
} KeyWordType;

typedef union {
    long long int int_value;
    double float_value;
    char *string_value;
    KeyWordType keyword;
} TokenValue;

typedef struct {
    TokenType type;
    TokenValue value;
} Token;

#endif // TOKEN_H
