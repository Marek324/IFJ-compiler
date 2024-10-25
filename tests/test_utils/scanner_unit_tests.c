#include "../../include/scanner.h"

void print_type(int type, FILE *output);
void print_kw(int kw, FILE *output);

int main(int argc, char **argv) {
    if (argc!=0){};
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }
    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        fprintf(stderr, "Error opening output file\n");
        return 1;
    }
    
    while(1) {
        Token *token = get_token(input);
        print_type(token->type, output);
        if(token->type == T_F64) {
            fprintf(output, "%f", token->value.float_value);
        } else if(token->type == T_INT) {
            fprintf(output, "%d", token->value.int_value);
        } else if(token->type == T_STR || token->type == T_ID) {
            fprintf(output, "%s", token->value.string_value);
        } else if(token->type == T_KW) {
            print_kw(token->value.keyword, output);
        } 
        fprintf(output, "\n");

        if(token->type != T_EOF) {
            free_token(token);
        }else{
            free_token(token);
            break;

        }
    }

    fclose(input);
    fclose(output);

    return 0;
}

void print_type(int type, FILE *output)
{
    switch(type){
        case T_KW: fprintf(output, "T_KW,"); break;
        case T_ID: fprintf(output, "T_ID,"); break;
        case T_AT_IMPORT: fprintf(output, "T_AT_IMPORT,"); break;
        case T_INT: fprintf(output, "T_INT,"); break;
        case T_F64: fprintf(output, "T_F64,"); break;
        case T_STR: fprintf(output, "T_STR,"); break;
        case T_ASGN: fprintf(output, "T_ASGN,"); break;
        case T_EQ: fprintf(output, "T_EQ,"); break;
        case T_BANG: fprintf(output, "T_BANG,"); break;
        case T_NEQ: fprintf(output, "T_NEQ,"); break;
        case T_LESS: fprintf(output, "T_LESS,"); break;
        case T_LEQ: fprintf(output, "T_LEQ,"); break;
        case T_MORE: fprintf(output, "T_MORE,"); break;
        case T_MEQ: fprintf(output, "T_MEQ,"); break;
        case T_PLUS: fprintf(output, "T_PLUS,"); break;
        case T_MINUS: fprintf(output, "T_MINUS,"); break;
        case T_MUL: fprintf(output, "T_MUL,"); break;
        case T_DIV: fprintf(output, "T_DIV,"); break;
        case T_LPAREN: fprintf(output, "T_LPAREN,"); break;
        case T_RPAREN: fprintf(output, "T_RPAREN,"); break;
        case T_LBRACE: fprintf(output, "T_LBRACE,"); break;
        case T_RBRACE: fprintf(output, "T_RBRACE,"); break;
        case T_LBRACKET: fprintf(output, "T_LBRACKET,"); break;
        case T_RBRACKET: fprintf(output, "T_RBRACKET,"); break;
        case T_COMMA: fprintf(output, "T_COMMA,"); break;
        case T_DOT: fprintf(output, "T_DOT,"); break;
        case T_SEMICOL: fprintf(output, "T_SEMICOL,"); break;
        case T_COLON: fprintf(output, "T_COLON,"); break;
        case T_PIPE: fprintf(output, "T_PIPE,"); break;
        case T_QMARK: fprintf(output, "T_QMARK,"); break;
        case T_EOF: fprintf(output, "T_EOF,"); break;
    }
}

void print_kw(int kw, FILE *output)
{
    switch(kw){
        case KW_CONST: fprintf(output, "KW_CONST,"); break;
        case KW_IF: fprintf(output, "KW_IF,"); break;
        case KW_ELSE: fprintf(output, "KW_ELSE,"); break;
        case KW_FN: fprintf(output, "KW_FN,"); break;
        case KW_I32: fprintf(output, "KW_I32,"); break;
        case KW_F64: fprintf(output, "KW_F64,"); break;
        case KW_NULL: fprintf(output, "KW_NULL,"); break;
        case KW_PUB: fprintf(output, "KW_PUB,"); break;
        case KW_RETURN: fprintf(output, "KW_RETURN,"); break;
        case KW_U8: fprintf(output, "KW_U8,"); break;
        case KW_VAR: fprintf(output, "KW_VAR,"); break;
        case KW_VOID: fprintf(output, "KW_VOID,"); break;
        case KW_WHILE: fprintf(output, "KW_WHILE,"); break;
        case NO_KW: fprintf(output, "NO_KW,"); break;
    }
}
