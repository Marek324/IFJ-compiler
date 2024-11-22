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
void checkBinTypes(ASTNode* node);
// check variable types in a unary function
void checkUnTypes();
// check variable types in a ternary function
void checkTernTypes();
// check types in division (convert to whole number division if necessary)
void checkDiv(ASTNode* node);








#endif // SEM_ANAL_H