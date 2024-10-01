/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/scanner.h"
#include "../include/error.h"

#define NOT_END_OF_ID (c = fgetc(file)) != EOF && c != '\n' && c != ' ' && c != '\t' && (isAlpha(c) || isNumber(c))

// Helper functions
bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNumber(char c){
    return c >= '0' && c <= '9';
}

int peek(FILE *file){
    int c = fgetc(file);
    if (c == EOF) return EOF;
    ungetc(c, file);
    return c;
}

int str_to_float(char *str, float *f) {
    char *endptr;
    *f = strtof(str, &endptr);
    if (*endptr != '\0') {
        return 1;
    }
    return 0;
}

int str_to_int(char *str, int *i) {
    char *endptr;
    *i = strtol(str, &endptr, 10);
    if (*endptr != '\0') {
        return 1;
    }
    return 0;
}

// Function prototypes
void consume_comment(FILE* file);
KeyWordType check_keyword(char *str);

// parameter file should be opened in the calling function.
// parameter line should be initialized to 1 in the calling function.
Token *get_token(FILE* file, int *line) {
    Token *token = (Token *)malloc(sizeof(Token));
    
    int state = S_START;
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        switch (state)
        {
        case S_START:
            if (c == ' ' || c == '\t') {
                continue;
            }
            switch (c){
                case '(':
                    token->type = T_LPAREN;
                    return token;
                case ')':
                    token->type = T_RPAREN;
                    return token;
                case '{':
                    token->type = T_LBRACE;
                    return token;
                case '}':
                    token->type = T_RBRACE;
                    return token;
                case '[':
                    token->type = T_LBRACKET;
                    return token;
                case ']':
                    token->type = T_RBRACKET;
                    return token;
                case '|':
                    token->type = T_PIPE;
                    break;
                case '?':
                    token->type = T_QMARK;
                    return token;
                case ':':
                    token->type = T_COLON;
                    return token;
                case ',':
                    token->type = T_COMMA;
                    return token;
                case '.':
                    token->type = T_DOT;
                    return token;
                case ';':
                    token->type = T_SEMICOL;
                    return token;
                case '_':
                    state = S_UNDERSCORE;
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
                case '"':
                    state = S_STR;
                    break;
                case '0':
                    state = S_ZERO;
                    break;
                case '@':
                    //put the character back, so it can be consumed in the next state
                    ungetc(c, file);
                    state = S_AT_IMPORT;
                    break;
                default:
                    if (isAlpha(c)) {
                        // put the character back, so it can be consumed in the next state
                        ungetc(c, file);
                        state = S_ID;
                    } else if (isNumber(c)) {
                        // put the character back, so it can be consumed in the next state
                        ungetc(c, file); 
                        state = S_INT;
                    } else if (c == EOF) {
                        token->type = T_EOF;
                        return token;
                    } else {
                        error_exit("Unknown character\n");
                    }
                    break;
            }
            break; // S_START

        case S_ASGN:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_EQ;
                return token;
            } else {
                token->type = T_ASGN;
                return token;
            }
            break; // S_ASGN

        case S_BANG:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_NEQ;
                return token;
            } else {
                token->type = T_BANG;
                return token;
            }
            break; // S_BANG

        case S_LESS:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_LEQ;
                return token;
            } else {
                token->type = T_LESS;
                return token;
            }
            break; // S_LESS

        case S_MORE:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_MEQ;
                return token;
            } else {
                token->type = T_MORE;
                return token;
            }
            break; // S_MORE

        case S_PLUS:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_PASGN;
                return token;
            } else {
                token->type = T_PLUS;
                return token;
            }
            break; // S_PLUS

        case S_MINUS:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_MASGN;
                return token;
            } else {
                token->type = T_MINUS;
                return token;
            }
            break; // S_MINUS

        case S_MUL:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_MULASGN;
                return token;
            } else {
                token->type = T_MUL;
                return token;
            }
            break; // S_MUL

        case S_SLASH:
            if (peek(file) == '=') {
                getc(file); // consume the character
                token->type = T_DASGN;
                return token;
            } else if (peek(file) == '/') {
                consume_comment(file);
            } else {
                token->type = T_DIV;
                return token;
            }
            break; // S_SLASH
        
        case S_UNDERSCORE:
            if (isAlpha(c) || isNumber(c)) {
                // put the character back, so it can be consumed in the next state
                ungetc(c, file);
                token->value.string_value = dyn_str_init();
                dyn_str_append(token->value.string_value, '_');
                state = S_ID;
            } else {
                error_exit("Invalid identifier\n");
            }
            break; // S_UNDERSCORE

        case S_AT_IMPORT:
            dyn_str* at_import = dyn_str_init();
            while(NOT_END_OF_ID) {
                dyn_str_append(at_import, c);
            }
            if (!strcmp(at_import->str, "@import")) {
                token->type = T_AT_IMPORT;
                return token;
            } else {
                error_exit("Unknown keyword\n");
            }
            break; // S_AT_IMPORT

        case S_ID:
            token->value.string_value = dyn_str_init();
            while(NOT_END_OF_ID) {
                dyn_str_append(token->value.string_value, c);
            }
            KeyWordType kw = check_keyword(token->value.string_value->str);
            if (kw != NO_KW) {
                token->type = T_KW;
                dyn_str_free(token->value.string_value);
                token->value.keyword = kw;
                return token;
            } else {
                token->type = T_ID;
            }
        }
    }
    

    return token;
}

void consume_comment(FILE* file){
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        continue;
    }
}

KeyWordType check_keyword(char *str){
    if(!strcmp(str, "const")) return KW_CONST;
    else if(!strcmp(str, "if")) return KW_IF;
    else if(!strcmp(str, "else")) return KW_ELSE;
    else if(!strcmp(str, "fn")) return KW_FN;
    else if(!strcmp(str, "i32")) return KW_I32;
    else if(!strcmp(str, "f64")) return KW_F64;
    else if(!strcmp(str, "null")) return KW_NULL;
    else if(!strcmp(str, "pub")) return KW_PUB;
    else if(!strcmp(str, "return")) return KW_RETURN;
    else if(!strcmp(str, "u8")) return KW_U8;
    else if(!strcmp(str, "var")) return KW_VAR;
    else if(!strcmp(str, "void")) return KW_VOID;
    else if(!strcmp(str, "while")) return KW_WHILE;
    else return NO_KW;
}
