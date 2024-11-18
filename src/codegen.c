#include "codegen.h"

stack_t* stack;

void codegen()
{
    stack = stackInit();
    ASTNode *node = ASTRoot->right->left; // root->right is prolog, root->right->left is first function

    ASTNode* main = NULL;

    PROLOG;

        

    EPILOG;

    stackClear(stack);
}

// void traverse_tree()