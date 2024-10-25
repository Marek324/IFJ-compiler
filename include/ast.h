#ifndef AST_H__
#define AST_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  
#include "token.h"  

// Forward declaration for ASTNode
struct ASTNode;
typedef struct ASTNode ASTNode;

// Definition of the AST Node structure
struct ASTNode {
    Token token;       // The token associated with this AST node
    ASTNode *left;     // Pointer to the left child node
    ASTNode *right;    // Pointer to the right child node
};

ASTNode *initASTNode(Token token);

// Function to add a word token to the AST
void addWordToAST(ASTNode *ptr, Token token);

// Function to add a function token to the AST
void addFunToAST(ASTNode *ptr, Token token);

// Function to add a line token to the AST
void addLineToAST(ASTNode *ptr, Token token);

// Function to get the start of the last added node
ASTNode *getBeginningOfLast(ASTNode *ptr);

// Function to get the last added node in the AST
ASTNode *getLast(ASTNode *ptr);

#endif // AST_H__
