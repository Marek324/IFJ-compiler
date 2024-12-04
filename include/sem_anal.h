/**
 * file: sem_anal.h
 * author: Adam Veselý xvesela00
 * author: Mikuláš Lešiga xlesigm00
 * Header file for the semantic analyzer.
 */
#ifndef SEM_ANAL_H
#define SEM_ANAL_H

#include <stdbool.h>

#include "ast.h"
#include "symtable.h"
#include "stack.h"

// Global variable for stack
extern stack_t *SCOPEStack;
// main semantic analyzer function
void analyse(ASTNode* node);
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
// check type in @as
ret_type checkAtAs(ASTNode* node, symtable_node_ptr local_table);
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
// check for function or var
void checkIfIdExits(symtable_node_ptr tree, char *key);
// check if var/const has been used / changed
void checkVarsAndConsts(symtable_node_ptr local_table);
// finds return in main function block
void findReturn(ASTNode *root);
// check return 
void checkForReturn(symtable_node_ptr function);
// starting semantic control for function
void symFuncDef(ASTNode* node);
// semantic control for Params
void symParamList(ASTNode* node, symtable_tree_ptr local_table);
// creates a block and its sub-blocks
void symBlock(ASTNode* node, symtable_tree_ptr local_table, ASTNode* optionalValue, ret_type type, ASTNode* whileId, symtable_node_ptr function);
// semantic control for statements
void symStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function);
// semantic control for varDec
void symVarDec(ASTNode* node, symtable_tree_ptr local_table);
// semantic control for every possible ID operation
void symIdStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function);
// pass in ID node of function (2nd ID of built-in) and P_EXPRESSION_LIST node
void checkArguments(symtable_tree_ptr tree, ASTNode* node, symtable_node_ptr key);
// semantic control for every possible if 
void symIfStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function);
// inserts built-in functions
void insertBuiltInFun();
// semantic control for every possible while
void symWhileLoop(ASTNode* node, symtable_tree_ptr local_table, ASTNode* id, symtable_node_ptr function);
// check types for boolean operation
ret_type checkBool(ASTNode* node, symtable_node_ptr local_table);
// true if relation operation, false otherwise. (EQ, NEQ, MORE, LESS, MEQ, LEQ)
bool isRel(ASTNodeType type);
// semantic control for return
void symReturnStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function);
// semantic control for break
void symBreakStatement(ASTNode* node, symtable_tree_ptr local_table);
// semantic control for continue
void symContinueStatement(ASTNode* node, symtable_tree_ptr local_table);
// semantic control for for_loop
void symForLoop(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function);
// returns T_ERROR_RET as default, converts node types to return types
ret_type convertToRetType(ASTNodeType node_type);
// returns true if float can be converted to int (decimal part is 0)
bool floatIsInt(float value);


#endif // SEM_ANAL_H