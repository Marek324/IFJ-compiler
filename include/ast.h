/**
 * @file ast.h
 * 
 * @author Adam Veselý
 * 
 * @brief Header file for the AST.
 */

#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
    AST_NODE_NUMBER,
    AST_NODE_OPERATION,
    AST_NODE_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    // TODO
} ASTNode;

ASTNode* create_node(ASTNodeType type); // TODO

void destroy_ast(ASTNode* root);

#endif

/*** End of file parser.h ***/