/**
 * file: ast.c
 * author: Mikuláš Lešiga xlesigm00
 * Implementation of the AST.
 */
#include "ast.h"

ASTNode *ASTRoot = NULL;

// Function to create and initialize a new ASTNode
ASTNode * nodeCreate(ASTNodeType type, Token *token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        free_token(token);
        freeAST(ASTRoot);
        error_exit(99, "Malloc failed\n");
    }
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->token = token;
    return node;
}

// Functions to link children to their parent <|:)
void insertLeft(ASTNode* parent, ASTNode* leftChild) {
    parent->left = leftChild;
}

void insertRight(ASTNode* parent, ASTNode* rightChild) {
    parent->right = rightChild;
}

// Function to free the memory allocated for the AST
void freeAST(ASTNode *ptr) {
    if (ptr == NULL) return;  //if the node is NULL, do nothing
    // Recursively free the left and right children
    freeAST(ptr->left);
    freeAST(ptr->right);
    // Free the token
    if (ptr->token != NULL) {
        free_token(ptr->token);
    }
    // Free the current node
    free(ptr);
}

ASTNodeType convertToASTType(TokenType type, KeyWordType keyword) {
    if (type == T_KW && keyword != NO_KW) {
        // Map each keyword to its specific ASTNodeType
        switch (keyword) {
            case KW_CONST: return T_CONST;
            case KW_IF: return T_IF;
            case KW_ELSE: return T_ELSE;
            case KW_FN: return T_FN;
            case KW_I32: return T__KW_I32;
            case KW_F64: return T_KW_F64;
            case KW_BOOL: return T_KW_BOOL;
            case KW_NULL: return T_NULL;
            case KW_PUB: return T_PUB;
            case KW_RETURN: return T_RETURN;
            case KW_U8: return T_U8;
            case KW_VAR: return T_VAR;
            case KW_VOID: return T_VOID;
            case KW_WHILE: return T_WHILE;
            case KW_UNREACHABLE: return T_UNREACHABLE;
            case KW_AND: return T_AND;
            case KW_OR: return T_OR;
            case KW_ORELSE: return T_ORELSE;
            case KW_TRUE: return T_TRUE;
            case KW_FALSE: return T_FALSE;

            default: return 99;
        }
    }

    // Map non-keyword tokens to their ASTNodeType equivalents
    switch (type) {
        case T_ID: return ID;
        case T_AT_IMPORT: return AT_IMPORT;
        case T_INT: return TYPE_INT;
        case T_FLOAT: return TYPE_F64;
        case T_STR: return TYPE_STR;
        case T_ASGN: return ASSGN;
        case T_EQ: return EQ;
        case T_BANG: return BANG;
        case T_NEQ: return NEQ;
        case T_LESS: return LESS;
        case T_LEQ: return LEQ;
        case T_MORE: return MORE;
        case T_MEQ: return MEQ;
        case T_PLUS: return PLUS;
        case T_MINUS: return MINUS;
        case T_MUL: return MUL;
        case T_DIV: return DIV;
        case T_LPAREN: return LPAREN;
        case T_RPAREN: return RPAREN;
        case T_LBRACE: return LBRACE;
        case T_RBRACE: return RBRACE;
        case T_LBRACKET: return LBRACKET;
        case T_RBRACKET: return RBRACKET;
        case T_COMMA: return COMMA;
        case T_DOT: return DOT;
        case T_SEMICOL: return SEMICOLON;
        case T_COLON: return COLON;
        case T_PIPE: return PIPE;
        case T_QMARK: return QMARK;
        case T_AT_AS: return AT_AS;
        case T_EOF: return END_OF_FILE;
        default: return 99;
    }
    return 99;
}
