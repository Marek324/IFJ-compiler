#include "codegen.h"

void codegen()
{
    ASTNode *node = ASTRoot->right->left; // root->right is prolog, root->right->left is first function

    ASTNode* main = NULL;

    PROLOG;

        

    EPILOG;

}

// void traverse_tree()