#include "codegen.h"
#include "ast.h"
#include "dyn_str.h"

int codegen(ASTNode* root)
{
    dyn_str *functions = ds_create();

    PROLOG;

    

    EPILOG;

    return 0;
}