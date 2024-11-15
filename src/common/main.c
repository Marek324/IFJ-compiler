#include <stdio.h>

#include "token.h"
#include "ast.h"
#include "parser.h"
#include "exp_parser.h"

// Function to print a token's value
void printToken(Token *token) {
    if (token == NULL) return;

    // Print based on the token type
    switch (token->type) {
        case T_ID: 
            printf("ID(%s) ", token->value.string_value); 
            break;

        case T_KW: 
            switch (token->value.keyword) {
                case KW_CONST: printf("Keyword(const) "); break;
                case KW_IF: printf("Keyword(if) "); break;
                case KW_ELSE: printf("Keyword(else) "); break;
                case KW_FN: printf("Keyword(fn) "); break;
                case KW_I32: printf("Keyword(i32) "); break;
                case KW_F64: printf("Keyword(f64) "); break;
                case KW_BOOL: printf("Keyword(bool) "); break;
                case KW_NULL: printf("Keyword(null) "); break;
                case KW_PUB: printf("Keyword(pub) "); break;
                case KW_RETURN: printf("Keyword(return) "); break;
                case KW_U8: printf("Keyword(u8) "); break;
                case KW_VAR: printf("Keyword(var) "); break;
                case KW_VOID: printf("Keyword(void) "); break;
                case KW_FOR: printf("Keyword(for) "); break;
                case KW_WHILE: printf("Keyword(while) "); break;
                default: printf("Unknown Keyword "); break;
            }
            break;

        case T_AT_IMPORT: 
            printf("@import "); 
            break;

        case T_INT: 
            printf("Int(%d) ", token->value.int_value); 
            break;

        case T_FLOAT: 
            printf("Float(%f) ", token->value.float_value); 
            break;

        case T_STR: 
            printf("String(\"%s\") ", token->value.string_value); 
            break;

        case T_ASGN: 
            printf("= "); 
            break;

        case T_EQ: 
            printf("== "); 
            break;

        case T_BANG: 
            printf("! "); 
            break;

        case T_NEQ: 
            printf("!= "); 
            break;

        case T_LESS: 
            printf("< "); 
            break;

        case T_LEQ: 
            printf("<= "); 
            break;

        case T_MORE: 
            printf("> "); 
            break;

        case T_MEQ: 
            printf(">= "); 
            break;

        case T_PLUS: 
            printf("+ "); 
            break;

        case T_MINUS: 
            printf("- "); 
            break;

        case T_MUL: 
            printf("* "); 
            break;

        case T_DIV: 
            printf("/ "); 
            break;

        case T_LPAREN: 
            printf("( "); 
            break;

        case T_RPAREN: 
            printf(") "); 
            break;

        case T_LBRACE: 
            printf("{ "); 
            break;

        case T_RBRACE: 
            printf("} "); 
            break;

        case T_LBRACKET: 
            printf("[ "); 
            break;

        case T_RBRACKET: 
            printf("] "); 
            break;

        case T_COMMA: 
            printf(", "); 
            break;

        case T_DOT: 
            printf(". "); 
            break;

        case T_SEMICOL: 
            printf("; "); 
            break;

        case T_COLON: 
            printf(": "); 
            break;

        case T_PIPE: 
            printf("| "); 
            break;

        case T_QMARK: 
            printf("? "); 
            break;

        case T_EOF: 
            printf("EOF "); 
            break;

        case T_ERROR: 
            printf("Error "); 
            break;

        default: 
            printf("Unknown "); 
            break;
    }
}

// for expressions
void printTreeLeftToRight(ASTNode* node) {
    if (node == NULL) { return; }
    printTreeLeftToRight(node->left);
    // printToken(node->token);
    printTreeLeftToRight(node->right);
}

// Recursive function to print the AST tokens from right to left
void printTreeRightToLeft(ASTNode *node) {
    if (node == NULL) { return; }
    // Recur to the right child first
    printTreeRightToLeft(node->right);

    // Print the token associated with this node
    printToken(node->token);

    if(node->type == P_EXPRESSION) {
        printTreeLeftToRight(node->right);
    }

    // Recur to the left child last
    printTreeRightToLeft(node->left);
}

// Example usage
int main() {
    circ_buff_ptr buffer = circ_buff_init();
    Parse(buffer);
    printf("AST Tokens (Right to Left):\n");
    printTreeRightToLeft(ASTRoot);
    circ_buff_free(buffer);
    freeAST(ASTRoot);
    return 0;
}