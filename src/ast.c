#include "ast.h"

// Function to create and initialize a new ASTNode
ASTNode *initASTNode(Token token) {
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    if (newNode == NULL) {
        return NULL;
    }
    newNode->token = token;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Function to traverse to the beginning of the last node in the chain
ASTNode *getBeginningOfLast(ASTNode *ptr) {
    while (ptr->right != NULL) {
        ptr = ptr->right;
    }
    return ptr;
}

// Function to traverse to the last node in the chain
ASTNode *getLast(ASTNode *ptr) {
    while (ptr->left != NULL) {
        ptr = ptr->left;
    }
    return ptr;
}

// Function to add a word node to the AST
void addWordToAST(ASTNode *ptr, Token token) {
    ptr = getBeginningOfLast(ptr);
    if (ptr->left != NULL) {
        ASTNode *newNode = initASTNode(token);
        if (newNode == NULL) return;
        ptr = ptr->left;
        ptr = getBeginningOfLast(ptr);
        ptr = getLast(ptr);
        ptr->left = newNode;
    }
    else {
        addLineToAST(ptr, token);
    }
}

// Function to add a line node to the AST
void addLineToAST(ASTNode *ptr, Token token) {
    
    ASTNode *newNode = initASTNode(token);
    if (newNode == NULL) return;
    ptr = getBeginningOfLast(ptr);
    if (ptr->left != NULL) {
        ptr = ptr->left;
        ptr = getBeginningOfLast(ptr->left);
        ptr->right = newNode;
    } else {
        ptr->left = newNode;
    }
}

void addFunToAST(ASTNode *ptr, Token token) {
    ASTNode *newNode = initASTNode(token);
    if (newNode == NULL) return;
    ptr = getBeginningOfLast(ptr);
    ptr->right = newNode;
}