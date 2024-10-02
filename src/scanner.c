/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/scanner.h"

// Macros
#define NOT_END_OF_ID (c = fgetc(file)) != EOF && c != '\n' && c != ' ' && c != '\t' && (isAlpha(c) || isNumber(c))

#define CHECK_FOLLOW_EQ(T_IF_EQ, T_IF_NEQ) \
    if (peek(file) == '=') { \
        getc(file); /*consume character*/ \
        token->type = T_IF_EQ; \
    } else { \
        token->type = T_IF_NEQ; \
    } \
        return token; \
    break

#define SINGLE_CHAR_TOKEN(T) \
    token->type = T; \
    return token

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
Token *get_token(FILE* file) {
    Token *token = (Token *)malloc(sizeof(Token));
    
    int state = S_START;
    int c;
    while ((c = fgetc(file)) != EOF) {
        switch (state)
        {
        case S_START:
            if (c == ' ' || c == '\t') {
                continue;
            }
            switch (c){
                case '(': SINGLE_CHAR_TOKEN(T_LPAREN);
                case ')': SINGLE_CHAR_TOKEN(T_RPAREN);
                case '{': SINGLE_CHAR_TOKEN(T_LBRACE);
                case '}': SINGLE_CHAR_TOKEN(T_RBRACE);
                case '[': SINGLE_CHAR_TOKEN(T_LBRACKET);
                case ']': SINGLE_CHAR_TOKEN(T_RBRACKET);
                case '|': SINGLE_CHAR_TOKEN(T_PIPE);
                case '?': SINGLE_CHAR_TOKEN(T_QMARK);
                case ':': SINGLE_CHAR_TOKEN(T_COLON);
                case ',': SINGLE_CHAR_TOKEN(T_COMMA);
                case '.': SINGLE_CHAR_TOKEN(T_DOT);
                case ';': SINGLE_CHAR_TOKEN(T_SEMICOL);
                
                case '=': CHECK_FOLLOW_EQ(T_EQ, T_ASGN);
                case '!': CHECK_FOLLOW_EQ(T_NEQ, T_BANG);
                case '<': CHECK_FOLLOW_EQ(T_LEQ, T_LESS);
                case '>': CHECK_FOLLOW_EQ(T_MEQ, T_MORE);
                case '+': CHECK_FOLLOW_EQ(T_PASGN, T_PLUS);
                case '-': CHECK_FOLLOW_EQ(T_MASGN, T_MINUS);
                case '*': CHECK_FOLLOW_EQ(T_MULASGN, T_MUL);
                
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
                    if (isAlpha(c) || c == '_') {
                        // put the character back, so it can be consumed in the next state
                        ungetc(c, file);
                        state = S_ID;
                    } else if (isNumber(c)) {
                        // put the character back, so it can be consumed in the next state
                        ungetc(c, file); 
                        state = S_INT;
                    } else {
                        error_exit(1,"Unknown character\n");
                    }
                    break;
            }
            break; // S_START

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

        case S_AT_IMPORT:
            dyn_str* at_import = dyn_str_init();
            while(NOT_END_OF_ID) {
                dyn_str_append(at_import, c);
            }
            int match = !strcmp(at_import->str, "@import");
            dyn_str_free(at_import); 
            if (match) {
                token->type = T_AT_IMPORT;
                return token;
            } else {
                error_exit(1,"Unknown keyword\n");
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

void error(Token *token, char *msg){
    if (token->value.string_value != NULL) {
        dyn_str_free(token->value.string_value);
    }
    free(token);

    error_exit(1, msg);
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
