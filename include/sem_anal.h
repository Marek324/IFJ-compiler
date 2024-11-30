#ifndef SEM_ANAL_H
#define SEM_ANAL_H

#include <stdbool.h>

#include "ast.h"
#include "symtable.h"
#include "stack.h"

typedef enum {
    FUNCTION_SCOPE,
    WHILE_SCOPE,
    FOR_SCOPE,
    NULL_SCOPE
} scopeLevel;
    

extern stack_t *SCOPEStack;
// main semantic analyzer function
void analyse(ASTNode* node);
// checks if the variable or function was already declared
void checkDec();
// checks the type of a function or variable
ret_type checkType();
// check expression, check for P_EXPRESSION and pass in its right child (the expression itself)
ret_type checkExpr(ASTNode* node, symtable_node_ptr local_table);
// check types in a binary arithmetic operation
ret_type checkAritTypes(ASTNode* node, symtable_node_ptr local_table);
// check types in a unary operation
ret_type checkUnType(ASTNode* node, symtable_node_ptr local_table);
// check types in a ternary operation
ret_type checkTernTypes(ASTNode* node, symtable_node_ptr local_table);
// check types in division (convert to whole number division if necessary)
ret_type checkDiv(ASTNode* node, symtable_node_ptr local_table);
// check types in relation operation
ret_type checkRel(ASTNode* node, symtable_node_ptr local_table);
// check types in an orelse operation
ret_type checkOrElse(ASTNode* node, symtable_node_ptr local_table);
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

void checkForReturn(ASTNode *root);

void symFuncDef(ASTNode* node);

void symParamList(ASTNode* node, symtable_tree_ptr local_table);

void symBlock(ASTNode* node, symtable_tree_ptr local_table, ASTNode* optionalValue, ret_type type, ASTNode* whileId);

void symStatement(ASTNode* node, symtable_tree_ptr local_table);

void symVarDec(ASTNode* node, symtable_tree_ptr local_table);

void symIdStatement(ASTNode* node, symtable_tree_ptr local_table);
// pass in ID node of function
void checkArguments(symtable_tree_ptr tree, ASTNode* node, symtable_node_ptr key);

void symIfStatement(ASTNode* node, symtable_tree_ptr local_table);

void insertBuiltInFun();

void symWhileLoop(ASTNode* node, symtable_tree_ptr local_table, ASTNode* id);
// check types for boolean operation
ret_type checkBool(ASTNode* node, symtable_node_ptr local_table);
// true if relation operation, false otherwise. (EQ, NEQ, MORE, LESS, MEQ, LEQ)
bool isRel(ASTNodeType type);

void symReturnStatement(ASTNode* node, symtable_tree_ptr local_table);

void symBreakStatement(ASTNode* node, symtable_tree_ptr local_table);

void symContinueStatement(ASTNode* node, symtable_tree_ptr local_table);

void symForLoop(ASTNode* node, symtable_tree_ptr local_table);

void funcDef(ASTNode* node);
// returns T_ERROR_RET as default, converts node types to return types
ret_type convertToRetType(ASTNodeType node_type);
// returns true if float can be converted to int (decimal part is 0)
bool floatIsInt(float value);







#endif // SEM_ANAL_H