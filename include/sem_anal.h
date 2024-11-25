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
// gets ret_type
ret_type get_ret_type(ASTNodeType type);
// gets function param info during funcDef
void symtable_get_function_param_info(symtable_node_ptr tree, char *key, ASTNode *ParamList, int i, int capacity);
// gets function type
void symtable_get_function_type(symtable_node_ptr tree, char *key, ASTNode *FunctionType);








#endif // SEM_ANAL_H