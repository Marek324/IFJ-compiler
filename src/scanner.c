/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include <stdio.h>
#include <stdbool.h>

#include "../include/dyn_str.h"
#include "../include/scanner.h"
#include "../include/token.h"

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNumber(char c){
    return c >= '0' && c <= '9';
}

// parameter file should be opened in the calling function.
// parameter line should be initialized to 1 in the calling function.
Token *get_token(FILE* file, int *line) {
    Token *token = (Token *)malloc(sizeof(Token));
    
    int state = S_START;
    int c;
    while ((c = fgetc(file)) != EOF && c != '@n') {
        switch (state)
        {
        case S_START:
            if (c == ' ' || c == '\t') {
                continue;
            } else if (c == '\n') {
                (*line)++;
                continue;
            } else if (isAlpha(c)) {
                state = S_ID;
                dyn_str_add_char(token->value.string_value, c);
            } else if (isNumber(c)) {
                state = S_INT;
                dyn_str_add_char(token->value.string_value, c);
            } else {
                switch (c)
                {
                case '=':
                    state = S_ASGN;
                    break;
                case '!':
                    state = S_BANG;
                    break;
                case '<':
                    state = S_LESS;
                    break;
                case '>':
                    state = S_MORE;
                    break;
                case '+':
                    state = S_PLUS;
                    break;
                case '-':
                    state = S_MINUS;
                    break;
                case '*':
                    state = S_MUL;
                    break;
                case '/':
                    state = S_SLASH;
                    break;
                case '.':
                    state = S_DOT;
                    break;
                case ',':
                    state = S_COMMA;
                    break;
                case ';':
                    state = S_SEMICOL;
                    break;
                case '(':
                    state = S_LPAREN;
                    break;
                case ')':
                    state = S_RPAREN;
                    break;
                case '{':
                    state = S_LBRACE;
                    break;
                case '}':
                    state = S_RBRACE;
                    break;
                case '[':
                    state = S_LBRACKET;
                    break;
                case ']':
                    state = S_RBRACKET;
                    break;
                case '"':
                    state = S_STR;
                    break;
                default:
                    state = S_ERROR;
                    break;
                }
            }
            break;
        
        default:
            break;
        }
    }
    

    return token;
}
