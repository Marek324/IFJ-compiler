/*
file: scanner.c
author: Marek Hric
Implementation of scanner.
*/

#include "scanner.h"

// Macros
#define INPUT stdin

#define RETURN_TOKEN(T)                                                         \
    token->type = T;                                                            \
    dyn_str_free(str);                                                          \
    return token

// Returns token with string value, type can be T_ID or T_STR
#define RETURN_STR_TOKEN(T)                                                     \
    token->value.string_value = (char *)malloc(str->length+1);                  \
    if (str->str == NULL) strcpy(token->value.string_value, "");                \
    else strcpy(token->value.string_value, str->str);                           \
    RETURN_TOKEN(T)

// Checks if next character is '=', returns T1 if true, T2 otherwise and puts character back to buffer
#define RETURN_TOKEN_FLW_EQ(T1, T2)                                             \
    c = getc(INPUT);                                                            \
    if (c == '=') {                                                             \
        RETURN_TOKEN(T1);                                                       \
    } else {                                                                    \
        circ_buff_enqueue(buffer, c);                                           \
        RETURN_TOKEN(T2);                                                       \
    } break

// Puts character back to buffer and changes state
#define PUT_C_BACK_CHANGE_STATE(S) {                                            \
    circ_buff_enqueue(buffer, c);                                               \
    state = S;                                                                  \
    break; }

// Reads until end of word or invalid id character
#define READ_ID {                                                               \
    circ_buff_enqueue(buffer, c);                                               \
    while((c = read_char(buffer)) != EOF && (isalnum(c) || c == '_'))           \
        dyn_str_append(str, c);                                                 \
    circ_buff_enqueue(buffer, c);}

// Reads until end of number
#define READ_NUM {                                                              \
    circ_buff_enqueue(buffer, c);                                               \
    while (isdigit(c = read_char(buffer)))                                      \
        dyn_str_append(str, c);}

#define APPEND_CHAR(C) {                                                        \
    dyn_str_append(str, C);                                                     \
    break; }

#define ERROR(E, MSG) \
    fprintf(stderr, "Error: %s\n",  MSG);                                  \
    token->value.int_value = (E);\
    RETURN_TOKEN(T_ERROR)

// Function prototypes
KeyWordType check_keyword(char *str);
int read_char(circ_buff_ptr buffer);

//char buffer
//get_token(&buffer)
Token *get_token(circ_buff_ptr buffer) 
{
    dyn_str *str = dyn_str_init();
    
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL){ 
        ERROR(99, "Memory allocation failed"); 
    }
    

    int state = S_START;
    int c; 
    while ((c = read_char(buffer)) != EOF) {
        switch (state) {
            case S_START:
                if (isspace(c)) 
                    continue;
                
                switch (c) {
                    // Single character tokens can be returned immediately
                    case '+': RETURN_TOKEN(T_PLUS);
                    case '-': RETURN_TOKEN(T_MINUS);
                    case '*': RETURN_TOKEN(T_MUL);
                    // case '@': RETURN_TOKEN(T_AT);
                    case '?': RETURN_TOKEN(T_QMARK);
                    case '.': RETURN_TOKEN(T_DOT);
                    case ',': RETURN_TOKEN(T_COMMA);
                    case ':': RETURN_TOKEN(T_COLON);
                    case ';': RETURN_TOKEN(T_SEMICOL);
                    case '(': RETURN_TOKEN(T_LPAREN);
                    case ')': RETURN_TOKEN(T_RPAREN);
                    case '|': RETURN_TOKEN(T_PIPE);
                    case '[': RETURN_TOKEN(T_LBRACKET);
                    case ']': RETURN_TOKEN(T_RBRACKET);
                    case '{': RETURN_TOKEN(T_LBRACE);
                    case '}': RETURN_TOKEN(T_RBRACE);

                    case '=': RETURN_TOKEN_FLW_EQ(T_EQ, T_ASGN);
                    case '!': RETURN_TOKEN_FLW_EQ(T_NEQ, T_BANG);
                    case '<': RETURN_TOKEN_FLW_EQ(T_LEQ, T_LESS);
                    case '>': RETURN_TOKEN_FLW_EQ(T_MEQ, T_MORE);

                    case '/':
                        c = getc(INPUT);
                        if (c == '/') {
                            while ((c = getc(INPUT)) != EOF && c != '\n')  // consume comment
                                continue;
                        } else {
                            // Store character back to buffer
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
                            ERROR(1, "Standalone backslash");
                        }
                        break;
                    
                    case '"': PUT_C_BACK_CHANGE_STATE(S_STR)
                    case '@': 
                        dyn_str_append(str, c); // add @ to dyn_string
                        state = S_AT;
                        break;
                    default:
                        if (isalpha(c) || c == '_')  PUT_C_BACK_CHANGE_STATE(S_ID)
                        else if (isdigit(c)) PUT_C_BACK_CHANGE_STATE(S_INT) 
                        else {
                            ERROR(1, "Invalid character"); // Invalid character
                        }
                        break;
                }
                break; // S_START

            case S_AT:
                {   
                    READ_ID;
                    int import = !strcmp(str->str, "@import");
                    int as = !strcmp(str->str, "@as");
                    if (import) {
                        RETURN_TOKEN(T_AT_IMPORT);
                    } else if (as) {
                        RETURN_TOKEN(T_AT_AS);
                    } else {
                        ERROR(1, "Invalid @ keyword"); // Invalid @ keyword
                    }
                }
                break; // S_AT_IMPORT

            case S_ID:
                READ_ID;
                KeyWordType kw = check_keyword(str->str);
                if (kw != NO_KW) {
                    token->value.keyword = kw;
                    RETURN_TOKEN(T_KW);
                } else {
                    RETURN_STR_TOKEN(T_ID);
                }
                break; // S_ID

            case S_INT:
                if (c == '0'){
                    c = read_char(buffer);
                    if (c == EOF) {
                        token->value.int_value = 0;
                        RETURN_TOKEN(T_INT);
                    }
                    else if (isdigit(c)) {
                        ERROR(1, "Leading zero"); // Leading zero
                    }
                    circ_buff_enqueue(buffer, '0');
                }

                READ_NUM;
                if (c == '.') {
                    state = S_FLOAT_DOT;
                    break;
                } else if (c == 'e' || c == 'E') {
                    state = S_FLOAT_EXP_SIGN;
                    break;
                } else {
                    circ_buff_enqueue(buffer, c);
                    token->value.int_value = atoi(str->str);
                    RETURN_TOKEN(T_INT);
                }
                break; // S_INT

            case S_FLOAT_DOT:
                if (isdigit(c)) {
                    dyn_str_append(str, '.');
                    circ_buff_enqueue(buffer, c);
                    state = S_FLOAT;
                    break;
                } else {
                    ERROR(1, "Invalid float, no decimal part after \'.\'"); // Invalid float - no decimal part
                }
                break; // S_FLOAT_DOT

            case S_FLOAT:
                READ_NUM;
                if (c == 'e' || c == 'E') {
                    state = S_FLOAT_EXP_SIGN;
                    break;
                } else {
                    circ_buff_enqueue(buffer, c);
                    token->value.float_value = strtod(str->str, NULL);
                    RETURN_TOKEN(T_FLOAT);
                }
                break; // S_FLOAT

            case S_FLOAT_EXP_SIGN:
                if (isdigit(c)) {
                    dyn_str_append(str, 'e');
                    circ_buff_enqueue(buffer, c);
                    state = S_FLOAT_EXP;
                    break;
                } else if (c == '+' || c == '-') {
                    dyn_str_append(str, 'e');
                    dyn_str_append(str, c);
                    state = S_FLOAT_EXP;
                    break;
                } else {
                    ERROR(1, "Invalid float, no exponent part after \'e\'"); // Invalid float - no exponent
                }
                break; // S_FLOAT_EXP_SIGN

            case S_FLOAT_EXP:
                READ_NUM;
                circ_buff_enqueue(buffer, c);
                token->value.float_value = strtod(str->str, NULL);
                RETURN_TOKEN(T_FLOAT);
                break; // S_FLOAT_EXP

            case S_STR:
                while((c = read_char(buffer)) != '"') {
                    if(c == '\\') {
                        state = S_STR_ESC;
                        break;
                    } else if (c == '\n' || c == EOF) {
                        ERROR(1, "No closing quote in string"); // No closing quote
                    } else {
                        dyn_str_append(str, c);
                    }
                }
                if (state == S_STR_ESC)
                    break;
                
                RETURN_STR_TOKEN(T_STR);
                break; // S_STR

            case S_STR_ESC:
                switch(c) {
                    case 'n':   APPEND_CHAR('\n');
                    case 't':   APPEND_CHAR('\t');
                    case 'r':   APPEND_CHAR('\r');
                    case '\\':  APPEND_CHAR('\\');
                    case '"':   APPEND_CHAR('"');
                    case 'x': 
                        state = S_STR_HEX;
                        break;
                    default:
                        ERROR(1, "Invalid escape sequence"); // Invalid escape sequence
                }
                if (state != S_STR_HEX) state = S_STR;
                circ_buff_enqueue(buffer, c);
                break; // S_STR_ESC

            case S_STR_HEX: {
                int hex = 0;
                for (int i = 0; i < 2; i++) {
                    c = read_char(buffer);
                    if (!isxdigit(c)){
                        fprintf(stderr, "Invalid hex escape number %c\n", c);
                        ERROR(1, "Invalid hex number"); // Invalid hex number
                    }
                    hex = hex * 16 + (isdigit(c) ? c - '0' : tolower(c) - 'a' + 10);
                }
                
                dyn_str_append(str, (char)hex);
                state = S_STR;           
                circ_buff_enqueue(buffer, c);     
                } break; // S_STR_HEX

            case S_STR_MLINE:
                circ_buff_enqueue(buffer, c);
                while((c = read_char(buffer)) != EOF && c != '\n') 
                    dyn_str_append(str, c);

                if (c == EOF) {
                    RETURN_STR_TOKEN(T_STR);
                } else if (c == '\n'){
                    state = S_STR_MLINE_NEWLINE;
                }    
                break; // S_STR_MLINE

            case S_STR_MLINE_NEWLINE:
                circ_buff_enqueue(buffer, c);
                while(isspace(c = read_char(buffer))) {} // skip whitespaces
                if (c == '\\') {
                    c = read_char(buffer);
                    if (c == '\\') {
                        dyn_str_append(str, '\n');
                        state = S_STR_MLINE;
                        break;
                    } else {
                        ERROR(1, "Standalone backslash"); // Standalone backslash
                    }
                } else {
                    circ_buff_enqueue(buffer, c);
                    RETURN_STR_TOKEN(T_STR);
                }
                break; // S_STR_MLINE_NEWLINE
        }
    } 

    RETURN_TOKEN(T_EOF);
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
    else if(!strcmp(str, "orelse")) return KW_ORELSE;
    else if(!strcmp(str, "unreachable")) return KW_UNREACHABLE;
    else if(!strcmp(str, "true")) return KW_TRUE;
    else if(!strcmp(str, "false")) return KW_FALSE;
    else if(!strcmp(str, "bool")) return KW_BOOL;
    else if(!strcmp(str, "and")) return KW_AND;
    else if(!strcmp(str, "or")) return KW_OR;
    else if(!strcmp(str, "for")) return KW_FOR;
    else if(!strcmp(str, "break")) return KW_BREAK;
    else if(!strcmp(str, "continue")) return KW_CONTINUE;
    else return NO_KW;
}

void free_token(Token* token)
{
    if (token == NULL) return;
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
