#include "codegen.h"

void prog(ASTNode *root);
void function_def(ASTNode *node);
void param_list(ASTNode *node);
void statement(ASTNode *node);
void var_dec(ASTNode *node);
void id_statement(ASTNode *node);
void if_statement(ASTNode *node);
void while_loop(ASTNode *node);
void for_loop(ASTNode *node);
void return_statement(ASTNode *node);
void break_statement(ASTNode *node);
void continue_statement(ASTNode *node);
void expression(ASTNode *node);
void func_call(ASTNode *node);
void expression_list(ASTNode *node);
void asgn_found(ASTNode *node);
