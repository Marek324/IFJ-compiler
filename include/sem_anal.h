#ifndef SEM_ANAL_H
#define SEM_ANAL_H

#include <stdbool.h>

#include "ast.h"
// main semantic analyzer function
void analyse(ASTNode* node);
// checks if the variable or function was already declared
void checkDec();
// check the arguments of a function
void checkArgs();
// checks the type of a function or variable
void checkType();
// check expression
void checkExpr();
// check variable types in a binary function
void checkBinTypes();
// check variable types in a unary function
void checkUnTypes();
// check variable types in a ternary function
void checkTernTypes();








#endif // SEM_ANAL_H