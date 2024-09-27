/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/dyn_str.h"
#include "../include/scanner.h"
#include "../include/token.h"

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNumber(char c){
    return c >= '0' && c <= '9';
}

int peek(file){
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
                case '=':
                
            }
            break;
        }
    }
    

    return token;
}
