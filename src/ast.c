#include "../include/ast.h"

ASTNode *ASTRoot = NULL;


// Function to initialize the global ASTRoot
void initializeAST() {
    ASTRoot = (ASTNode*)malloc(sizeof(ASTNode));
    if (ASTRoot == NULL) {
        error_exit(99, "Malloc failed\n");
    }
    ASTRoot->type = P_PROG;  // Initial type
    ASTRoot->left = NULL;  // No children initially
    ASTRoot->right = NULL;
    ASTRoot->token = NULL; 
}

// Function to create and initialize a new ASTNode
ASTNode * nodeCreate(ASTNodeType type, Token *token) {
    if (token->type == T_ERROR) {
        freeAST(ASTRoot);
        error_exit(token->value.int_value, "Malloc failed\n");
    }
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
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
        free(ptr->token);
    }
    
    // Free the current node
    free(ptr);
}