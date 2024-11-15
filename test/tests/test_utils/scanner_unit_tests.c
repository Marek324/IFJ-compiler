<<<<<<< HEAD
#include "../../include/scanner.h"

void print_type(int type);
void print_kw(int kw);

int main() {
    circ_buff_ptr buffer = circ_buff_init();
    while(1) {
        Token *token = get_token(buffer);
        print_type(token->type);
        if(token->type == T_FLOAT) {
            printf(" %f", token->value.float_value);
        } else if(token->type == T_INT) {
            printf(" %d", token->value.int_value);
        } else if(token->type == T_STR || token->type == T_ID) {
            printf(" %s", token->value.string_value);
        } else if(token->type == T_KW) {
            print_kw(token->value.keyword);
        }
        printf("\n");

        if(token->type != T_EOF) {
            free_token(token);
        }else{
            free_token(token);
            break;
        }
    }
    circ_buff_free(buffer);

    return 0;
}

void print_type(int type)
{
    switch(type){
        case T_KW: printf("T_KW,"); break;
        case T_ID: printf("T_ID,"); break;
        case T_AT_IMPORT: printf("T_AT_IMPORT,"); break;
        case T_INT: printf("T_INT,"); break;
        case T_FLOAT: printf("T_FLOAT,"); break;
        case T_STR: printf("T_STR,"); break;
        case T_ASGN: printf("T_ASGN,"); break;
        case T_EQ: printf("T_EQ,"); break;
        case T_BANG: printf("T_BANG,"); break;
        case T_NEQ: printf("T_NEQ,"); break;
        case T_LESS: printf("T_LESS,"); break;
        case T_LEQ: printf("T_LEQ,"); break;
        case T_MORE: printf("T_MORE,"); break;
        case T_MEQ: printf("T_MEQ,"); break;
        case T_PLUS: printf("T_PLUS,"); break;
        case T_MINUS: printf("T_MINUS,"); break;
        case T_MUL: printf("T_MUL,"); break;
        case T_DIV: printf("T_DIV,"); break;
        case T_LPAREN: printf("T_LPAREN,"); break;
        case T_RPAREN: printf("T_RPAREN,"); break;
        case T_LBRACE: printf("T_LBRACE,"); break;
        case T_RBRACE: printf("T_RBRACE,"); break;
        case T_LBRACKET: printf("T_LBRACKET,"); break;
        case T_RBRACKET: printf("T_RBRACKET,"); break;
        case T_COMMA: printf("T_COMMA,"); break;
        case T_DOT: printf("T_DOT,"); break;
        case T_SEMICOL: printf("T_SEMICOL,"); break;
        case T_COLON: printf("T_COLON,"); break;
        case T_PIPE: printf("T_PIPE,"); break;
        case T_QMARK: printf("T_QMARK,"); break;
        case T_EOF: printf("T_EOF,"); break;
    }
}

void print_kw(int kw)
{
    switch(kw){
        case KW_CONST: printf(" KW_CONST"); break;
        case KW_IF: printf(" KW_IF"); break;
        case KW_ELSE: printf(" KW_ELSE"); break;
        case KW_FN: printf(" KW_FN"); break;
        case KW_I32: printf(" KW_I32"); break;
        case KW_F64: printf(" KW_F64"); break;
        case KW_NULL: printf(" KW_NULL"); break;
        case KW_PUB: printf(" KW_PUB"); break;
        case KW_RETURN: printf(" KW_RETURN"); break;
        case KW_U8: printf(" KW_U8"); break;
        case KW_VAR: printf(" KW_VAR"); break;
        case KW_VOID: printf(" KW_VOID"); break;
        case KW_WHILE: printf(" KW_WHILE"); break;
        case NO_KW: printf(" NO_KW"); break;
    }
}
=======
#include "../../include/scanner.h"

void print_type(int type);
void print_kw(int kw);

int main() {
    circ_buff_ptr buffer = circ_buff_init();
    while(1) {
        Token *token = get_token(buffer);
        print_type(token->type);
        if(token->type == T_FLOAT) {
            printf(" %f", token->value.float_value);
        } else if(token->type == T_INT) {
            printf(" %d", token->value.int_value);
        } else if(token->type == T_STR || token->type == T_ID) {
            printf(" %s", token->value.string_value);
        } else if(token->type == T_KW) {
            print_kw(token->value.keyword);
        }
        printf("\n");

        if(token->type != T_EOF) {
            free_token(token);
        }else{
            free_token(token);
            break;
        }
    }
    circ_buff_free(buffer);

    return 0;
}

void print_type(int type)
{
    switch(type){
        case T_KW: printf("T_KW,"); break;
        case T_ID: printf("T_ID,"); break;
        case T_AT_IMPORT: printf("T_AT_IMPORT,"); break;
        case T_INT: printf("T_INT,"); break;
        case T_FLOAT: printf("T_FLOAT,"); break;
        case T_STR: printf("T_STR,"); break;
        case T_ASGN: printf("T_ASGN,"); break;
        case T_EQ: printf("T_EQ,"); break;
        case T_BANG: printf("T_BANG,"); break;
        case T_NEQ: printf("T_NEQ,"); break;
        case T_LESS: printf("T_LESS,"); break;
        case T_LEQ: printf("T_LEQ,"); break;
        case T_MORE: printf("T_MORE,"); break;
        case T_MEQ: printf("T_MEQ,"); break;
        case T_PLUS: printf("T_PLUS,"); break;
        case T_MINUS: printf("T_MINUS,"); break;
        case T_MUL: printf("T_MUL,"); break;
        case T_DIV: printf("T_DIV,"); break;
        case T_LPAREN: printf("T_LPAREN,"); break;
        case T_RPAREN: printf("T_RPAREN,"); break;
        case T_LBRACE: printf("T_LBRACE,"); break;
        case T_RBRACE: printf("T_RBRACE,"); break;
        case T_LBRACKET: printf("T_LBRACKET,"); break;
        case T_RBRACKET: printf("T_RBRACKET,"); break;
        case T_COMMA: printf("T_COMMA,"); break;
        case T_DOT: printf("T_DOT,"); break;
        case T_SEMICOL: printf("T_SEMICOL,"); break;
        case T_COLON: printf("T_COLON,"); break;
        case T_PIPE: printf("T_PIPE,"); break;
        case T_QMARK: printf("T_QMARK,"); break;
        case T_EOF: printf("T_EOF,"); break;
    }
}

void print_kw(int kw)
{
    switch(kw){
        case KW_CONST: printf(" KW_CONST"); break;
        case KW_IF: printf(" KW_IF"); break;
        case KW_ELSE: printf(" KW_ELSE"); break;
        case KW_FN: printf(" KW_FN"); break;
        case KW_I32: printf(" KW_I32"); break;
        case KW_F64: printf(" KW_F64"); break;
        case KW_NULL: printf(" KW_NULL"); break;
        case KW_PUB: printf(" KW_PUB"); break;
        case KW_RETURN: printf(" KW_RETURN"); break;
        case KW_U8: printf(" KW_U8"); break;
        case KW_VAR: printf(" KW_VAR"); break;
        case KW_VOID: printf(" KW_VOID"); break;
        case KW_WHILE: printf(" KW_WHILE"); break;
        case NO_KW: printf(" NO_KW"); break;
    }
}
>>>>>>> master
