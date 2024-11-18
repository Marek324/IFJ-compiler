#include "codegen.h"
#include "ast.h"
#include "dyn_str.h"

int codegen(ASTNode* root)
{
    dyn_str *functions = dyn_str_init();

    PROLOG;

    

    EPILOG;

    return 0;
}