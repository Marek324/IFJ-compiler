/**
 * @file scanner.c
 * 
 * @author Marek Hric
 * 
 * @brief Implementation of scanner.
 */

// Keywords: const, else, fn, if, i32, f64, null, pub, return, u8, var, void, while, import


#include <stdio.h>
#include <stdbool.h>

#include "../include/dyn_str.h"
#include "../include/scanner.h"
#include "../include/token.h"

/**
 * @brief Helper function for checking if the character is a letter.
 * 
 * @param c Character to check.
 * 
 * @return bool True if the character is a letter, false otherwise.
 */
bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief Helper function for checking if the character is a number.
 * 
 * @param c Character to check.
 * 
 * @return bool True if the character is a number, false otherwise.
 */
bool isNumber(char c){
    return c >= '0' && c <= '9';
}

/**
 * @brief Function for getting the next token from the file.
 * 
 * @param file File to get the token from.
 * file should be opened in the calling function.
 * @param line Pointer to the line number.
 * line should be initialized to 1 in the calling function.
 * 
 * @return Token* Pointer to the token.
 */
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
/*** End of file scanner.c */