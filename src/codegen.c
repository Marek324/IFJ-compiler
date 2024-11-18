#include "codegen.h"


int codegen(ASTNode* root)
{
    dyn_str *functions = dyn_str_init();

    if (functions == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 99;
    }

    if (root == NULL) {
        fprintf(stderr, "Error: Root is NULL\n");
        return 99;
    }

    root = root->right->left; // root->right is prolog, root->right->left is first function
    
    PROLOG;

    

    EPILOG;

    return 0;
}