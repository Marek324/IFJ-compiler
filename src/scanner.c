/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include "../include/scanner.h"

// Macros
#define NOT_END_OF_ID (c = fgetc(file)) != EOF && c != '\n' && c != ' ' && c != '\t' && (isAlpha(c) || isNumber(c))

#define RETURN_TOKEN(T) \
    token->type = T; \
    dyn_str_free(str); \
    return token

// Function prototypes
void consume_comment(FILE* file);
KeyWordType check_keyword(char *str);
void error(Token *token, char *msg);

// Helper functions
bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNumber(char c){
    return c >= '0' && c <= '9';
}

float str_to_float(char *str) {
    return strtof(str, NULL);
}

int str_to_int(char *str) {
    return strtol(str, NULL, 10);
}

// parameter file should be opened in the calling function.
Token *get_token(FILE* file) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) {
        error_exit(1, "Malloc failed\n");
    }
    dyn_str *str = dyn_str_init();
    
    int state = S_START;
    int c;
    while ((c = fgetc(file)) != EOF) {
        switch (state)
        {
        case S_START:
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                continue;
            }
            switch (c){
                case '(': RETURN_TOKEN(T_LPAREN);
                case ')': RETURN_TOKEN(T_RPAREN);
                case '{': RETURN_TOKEN(T_LBRACE);
                case '}': RETURN_TOKEN(T_RBRACE);
                case '[': RETURN_TOKEN(T_LBRACKET);
                case ']': RETURN_TOKEN(T_RBRACKET);
                case '|': RETURN_TOKEN(T_PIPE);
                case '?': RETURN_TOKEN(T_QMARK);
                case ':': RETURN_TOKEN(T_COLON);
                case ',': RETURN_TOKEN(T_COMMA);
                case '.': RETURN_TOKEN(T_DOT);
                case ';': RETURN_TOKEN(T_SEMICOL);
                case '+': RETURN_TOKEN(T_PLUS);
                case '-': RETURN_TOKEN(T_MINUS);
                case '*': RETURN_TOKEN(T_MUL);

                case '=': 
                    c = fgetc(file);
                    if (c == '=') {RETURN_TOKEN(T_EQ);}
                    else {
                        ungetc(c, file);
                        RETURN_TOKEN(T_ASGN);
                    }
                        
                    break;
                case '!': 
                    c = fgetc(file);
                    if (c == '=') {RETURN_TOKEN(T_NEQ);}
                    else {
                        ungetc(c, file);
                        RETURN_TOKEN(T_BANG);
                    }
                    break;
                case '<': 
                    c = fgetc(file);
                    if (c == '=') {RETURN_TOKEN(T_LEQ);}
                    else {
                        ungetc(c, file);
                        RETURN_TOKEN(T_LESS);
                    }
                    break;
                case '>': 
                    c = fgetc(file);
                    if (c == '=') {RETURN_TOKEN(T_MEQ);}
                    else {
                        ungetc(c, file);
                        RETURN_TOKEN(T_MORE);
                    }
                    break;

                case '/':
                    c = fgetc(file);
                    if (c == '/') {
                        consume_comment(file);
                        break;
                    } else {
                        ungetc(c, file);
                        RETURN_TOKEN(T_DIV);
                    }
                    break;

                case '\\':
                    
                
                
                case '"':
                    state = S_STR;
                    break;
                case '0':
                    state = S_ZERO;
                    break;
                case '@': 
                    break;
                default:
                    if (isAlpha(c) || c == '_') {
                    } else if (isNumber(c)) {
                    } else {
                        error_exit(1,"Unknown character\n");
                    }
                    break;
            }
            break; // S_START

        case S_AT_IMPORT:
            // token->value.string_value = dyn_str_init();
            // while(NOT_END_OF_ID) {
            //     dyn_str_append(token->value.string_value, c);
            // }
            // int match = !strcmp(token->value.string_value->str, "@import");
            // dyn_str_free(token->value.string_value); 
            // if (match) {
            //     token->type = T_AT_IMPORT;
            //     return token;
            // } else {
            //     error_exit(1,"Unknown keyword\n");
            // }
            break; // S_AT_IMPORT

        case S_ID:
            // token->value.string_value = dyn_str_init();
            // while(NOT_END_OF_ID) { 
            //     if (isAlpha(c) || isNumber(c) || c == '_') {
            //         dyn_str_append(token->value.string_value, c);
            //     } else {
            //         error(token, "Invalid character in ID\n");
            //     }
            // }
            // KeyWordType kw = check_keyword(token->value.string_value->str);
            // if (kw != NO_KW) {
            //     token->type = T_KW;
            //     dyn_str_free(token->value.string_value);
            //     token->value.keyword = kw;
            //     return token;
            // } else {
            //     token->type = T_ID;
            // }
            break; // S_ID

        case S_ZERO:
            // if (c == '.') {
            //     state = S_F64_DOT;
            //     CREATE_STR_FROM_S_ZERO;
            // } else if (c == 'e' || c == 'E') {
            //     state = S_F64_E;
            //     CREATE_STR_FROM_S_ZERO;
            // } else {
            //     token->value.int_value = 0;
            //     token->type = T_INT;
            //     return token;
            // }
            break; // S_ZERO

        case S_INT:
            // token->value.string_value = dyn_str_init();
            // dyn_str_append(token->value.string_value, c);
            // while (isNumber(c = fgetc(file))) {
            //     dyn_str_append(token->value.string_value, c);
            // }
            // if (c == '.') {
            //     dyn_str_append(token->value.string_value, c);
            //     state = S_F64_DOT;
            // } else if (c == 'e' || c == 'E') {
            //     dyn_str_append(token->value.string_value, c);
            //     state = S_F64_E;
            // } else {
            //     ungetc(c, file);
            //     token->type = T_INT;
            //     char *str = token->value.string_value->str;
            //     token->value.int_value = str_to_int(str);
            //     free(str);
            //     return token;
            // }
            break; // S_INT

        case S_F64_DOT:
                // if(c == 'e' || c == 'E'){
                //     dyn_str_append(token->value.string_value, c);
                //     state = S_F64_E;
                // } else if(isNumber(c)){
                //     ungetc(c, file);
                //     state = S_F64;
                // } else{
                //     error(token, "Invalid float, no number after \'.\'\n");
                // }
            break; // S_F64_DOT

        case S_F64:
            // if (isNumber(c)) {
            //     dyn_str_append(token->value.string_value, c);
            // } else if (c == 'e' || c == 'E') {
            //     dyn_str_append(token->value.string_value, c);
            //     state = S_F64_E;
            // } else {
            //     ungetc(c, file);
            //     token->type = T_F64;
            //     char *str = token->value.string_value->str;
            //     token->value.float_value = str_to_float(str);
            //     free(str);
            //     return token;
            // }
            break; // S_F64

        case S_F64_E:
            // if(c == '+' || c == '-' || isNumber(c)){
            //     dyn_str_append(token->value.string_value, c);
            //     state = S_F64_EXP;
            // } else{
            //     error(token, "Invalid float, exptected number, \'+\' or \'-\' after \'e\'\n");
            // }
            break; // S_F64_E

        case S_F64_EXP:
                // if(isNumber(peek(file))){
                //     dyn_str_append(token->value.string_value, c);
                // } else {
                //     char *str = token->value.string_value->str;
                //     token->type = T_F64;
                //     token->value.float_value = str_to_float(str);
                //     free(str);
                //     return token;
                // }
            break; // S_F64_EXP

        case S_STR:

            break; // S_STR

        case S_STR_ESC:

            break; // S_STR_ESC

        case S_STR_HEX:

            break; // S_STR_HEX

        case S_STR_MLINE:

            break; // S_STR_MLINE
        }
    }
    
    token->type = T_EOF;

    return token;
}

void consume_comment(FILE* file){
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        continue;
    }
}

void error(Token *token, char *msg){
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

void free_token(Token* token){
    free(token);
}
