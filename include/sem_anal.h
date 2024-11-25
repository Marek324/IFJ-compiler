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
void checkType();
// check expression
void checkExpr(ASTNode* node);
// check variable types in a binary function
void checkBinTypes(ASTNode* node);
// check variable types in a unary function
void checkUnTypes(ASTNode* node);
// check variable types in a ternary function
void checkTernTypes();
// check types in division (convert to whole number division if necessary)
void checkDiv(ASTNode* node);
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

void symReturnStatement(ASTNode* node, symtable_tree_ptr tree);

void symBreakStatement(ASTNode* node, symtable_tree_ptr tree);

void symContinueStatement(ASTNode* node, symtable_tree_ptr tree);

void symForLoop(ASTNode* node, symtable_tree_ptr tree);


#endif // SEM_ANAL_H