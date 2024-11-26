#ifndef SEM_ANAL_H
#define SEM_ANAL_H

#include <stdbool.h>

#include "ast.h"
#include "symtable.h"

// main semantic analyzer function
void analyse(ASTNode* node);
// checks if the variable or function was already declared
void checkDec();
// check the arguments of a function
void checkArgs();
// checks the type of a function or variable
ret_type checkType();
// check expression, check for P_EXPRESSION and pass in its right child (the expression itself)
ret_type checkExpr(ASTNode* node);
// check types in a binary arithmetic operation
ret_type checkAritTypes(ASTNode* node);
// check types in a unary operation
ret_type checkUnType(ASTNode* node);
// check types in a ternary operation
ret_type checkTernTypes(ASTNode* node);
// check types in division (convert to whole number division if necessary)
ret_type checkDiv(ASTNode* node);
// check types in relation operation
ret_type checkRel(ASTNode* node);
// gets ret_type
ret_type getRetType(ASTNodeType type);
// gets function
void getFunction(symtable_node_ptr tree, char *key);
// gets function param info during funcDef
void getFunctionParamInfo(symtable_node_ptr tree, char *key, ASTNode *ParamList, int i, int capacity);
// gets function type
void getFunctionType(symtable_node_ptr tree, char *key, ASTNode *FunctionType);
// check for main
void checkForMain();
// check for function
void checkIfIdExits(symtable_node_ptr tree, char *key);

void symFuncDef(ASTNode* node);

void symParamList(ASTNode* node, symtable_tree_ptr tree);

void symStatement(ASTNode* node, symtable_tree_ptr tree);

void symVarDec(ASTNode* node, symtable_tree_ptr tree);

void symIdStatement(ASTNode* node, symtable_tree_ptr tree);

void symIfStatement(ASTNode* node, symtable_tree_ptr tree);

void symWhileLoop(ASTNode* node, symtable_tree_ptr tree);
// check types for boolean operation
ret_type checkBool(ASTNode* node);
// true if relation operation, false otherwise. (EQ, NEQ, MORE, LESS, MEQ, LEQ)
bool isRel(ASTNodeType type);

void symReturnStatement(ASTNode* node, symtable_tree_ptr tree);

void symBreakStatement(ASTNode* node, symtable_tree_ptr tree);

void symContinueStatement(ASTNode* node, symtable_tree_ptr tree);

void symForLoop(ASTNode* node, symtable_tree_ptr tree);
void funcDef(ASTNode* node);
// returns T_NULL_RET as default
ret_type convertToRetType(ASTNodeType node_type);







#endif // SEM_ANAL_H