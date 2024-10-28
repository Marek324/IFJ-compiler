/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include "../include/scanner.h"

// Macros
#define INPUT stdin

#define RETURN_TOKEN(T) \
    token->type = T; \
    dyn_str_free(str); \
    return token

// Function prototypes
void consume_comment();
KeyWordType check_keyword(char *str);
void error(Token *token, char *msg);
int read_char(circ_buff_ptr buffer);

void load_id_to_dyn_str(dyn_str *str, circ_buff_ptr buffer);

//char buffer
//get_token(&buffer)
Token *get_token(circ_buff_ptr buffer) 
{
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL) 
        error_exit(1, "Malloc failed\n");
    
    dyn_str *str = dyn_str_init();
    
    int state = S_START;
    int c; 
    while ((c = read_char(buffer)) != EOF) {
        
        switch (state) {
            case S_START:
                if (isspace(c)) 
                    continue;
                
                switch (c) {
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
                        c = getc(INPUT);
                        if (c == '=') {
                            RETURN_TOKEN(T_EQ);
                        } else {
                            circ_buff_enqueue(buffer, c);
                            RETURN_TOKEN(T_ASGN);
                        }
                            
                        break;
                    case '!': 
                        c = getc(INPUT);
                        if (c == '=') {
                            RETURN_TOKEN(T_NEQ);
                        } else {
                            circ_buff_enqueue(buffer, c);
                            RETURN_TOKEN(T_BANG);
                        }
                        break;
                    case '<': 
                        c = getc(INPUT);
                        if (c == '=') {
                            RETURN_TOKEN(T_LEQ);
                        } else {
                            circ_buff_enqueue(buffer, c);
                            RETURN_TOKEN(T_LESS);
                        }
                        break;
                    case '>': 
                        c = getc(INPUT);
                        if (c == '=') {
                            RETURN_TOKEN(T_MEQ);
                        } else {
                            circ_buff_enqueue(buffer, c);
                            RETURN_TOKEN(T_MORE);
                        }
                        break;

                    case '/':
                        c = getc(INPUT);
                        if (c == '/') {
                            consume_comment(INPUT);
                            break;
                        } else {
                            circ_buff_enqueue(buffer, c);
                            RETURN_TOKEN(T_DIV);
                        }
                        break;

                    case '\\':
                        c = getc(INPUT);
                        if (c == '\\') {
                            state = S_STR_MLINE;
                            break;
                        } else {
                            error(token, "Invalid character after backslash\n");
                        }
                        break;
                    
                    case '"':
                        circ_buff_enqueue(buffer, c); // '"' needs to be returned because it will be read in main while and S_STR will read nexy char 
                        state = S_STR;
                        break;
                    case '@': 
                        dyn_str_append(str, c); // add @ to dyn_string
                        state = S_AT_IMPORT;
                        break;
                    default:
                        if (isalpha(c) || c == '_') {
                            circ_buff_enqueue(buffer, c);
                            state = S_ID;
                        } else if (isdigit(c)) {
                            if (c == '0') {
                                circ_buff_enqueue(buffer, c);
                                state = S_ZERO;
                            } else {
                                state = S_INT;
                            }
                            dyn_str_append(str, c);
                        } else {
                            free_token(token);
                            dyn_str_free(str);
                            error_exit(1,"Unknown character\n");
                        }
                        break;
                }
                break; // S_START

            case S_AT_IMPORT:
                {   
                    circ_buff_enqueue(buffer, c); // put character back to buffer
                    load_id_to_dyn_str(str, buffer);
                    int match = !strcmp(str->str, "@import");
                    if (match) {
                        RETURN_TOKEN(T_AT_IMPORT);
                    } else error_exit(1,"Unknown keyword starting with @\n");
                }
                break; // S_AT_IMPORT

            case S_ID:
                circ_buff_enqueue(buffer, c);
                load_id_to_dyn_str(str, buffer);

                KeyWordType kw = check_keyword(str->str);
                if (kw != NO_KW) {
                    token->value.keyword = kw;
                    RETURN_TOKEN(T_KW);
                } else {
                    token->value.string_value = (char *)malloc(str->length+1);
                    strcpy(token->value.string_value, str->str);
                    RETURN_TOKEN(T_ID);
                }
                break; // S_ID

            case S_ZERO:
                
                break; // S_ZERO

            case S_INT:
                
                break; // S_INT

            case S_F64_DOT:
                
                break; // S_F64_DOT

            case S_F64:
                
                break; // S_F64

            case S_F64_E:
                if (!isdigit(c)) {
                    
                }
                break; // S_F64_E

            case S_F64_EXP:
                
                break; // S_F64_EXP

            case S_STR:
                while((c = read_char(buffer)) != '"') {
                    if(c == '\\') {
                        state = S_STR_ESC;
                        break;
                    } else if (c == '\n' || c == EOF) {
                        error(token, "Missing terminating \" character\n");
                    } else {
                        dyn_str_append(str, c);
                    }
                }
                if (state == S_STR_ESC)
                    break;
                

                token->value.string_value = (char *)malloc(str->length+1);
                strcpy(token->value.string_value, str->str);
                RETURN_TOKEN(T_STR);
                break; // S_STR

            case S_STR_ESC:
                switch(c) {
                    case 'n': 
                        dyn_str_append(str, '\n');
                        break;
                    case 't': 
                        dyn_str_append(str, '\t');
                        break;
                    case 'r': 
                        dyn_str_append(str, '\r');
                        break;
                    case '\\': 
                        dyn_str_append(str, '\\');
                        break;
                    case '"': 
                        dyn_str_append(str, '"');
                        break;
                    case 'x': 
                        state = S_STR_HEX;
                        break;
                    default:
                        error(token, "Invalid escape sequence\n");
                        break;
                }
                state = S_STR;
                circ_buff_enqueue(buffer, c);
                break; // S_STR_ESC

            case S_STR_HEX:
                
                break; // S_STR_HEX

            case S_STR_MLINE:

                break; // S_STR_MLINE
            }
    } 

    RETURN_TOKEN(T_EOF);
}

void consume_comment()
{
    int c;
    while ((c = getc(INPUT)) != EOF && c != '\n') 
        continue;
}

void error(Token *token, char *msg)
{
    free_token(token);

    error_exit(1, msg);
}

KeyWordType check_keyword(char *str)
{
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

void free_token(Token* token)
{
    if(token->type == T_ID || token->type == T_STR)
        free(token->value.string_value);
    free(token);
}

int read_char(circ_buff_ptr buffer)
{
    if (!circ_buff_is_empty(buffer)) 
        return circ_buff_dequeue(buffer);
    else 
        return getc(INPUT);
    
}

void load_id_to_dyn_str(dyn_str *str, circ_buff_ptr buffer)
{
    int c;
    while((c = read_char(buffer)) != EOF && (isalnum(c) || c == '_')) 
        dyn_str_append(str, c);

    circ_buff_enqueue(buffer, c);
}
