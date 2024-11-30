#include "codegen.h"
#include "linked_list.h"

#ifndef CODEGEN_PRIV_H
#define CODEGEN_PRIV_H

int prog(ASTNode *root);
int function_def(ASTNode *node);
void param_list(ASTNode *node);
int statement(ASTNode *node, bool dec_var, bool asgn_var, const char *label);
int var_dec(ASTNode *node, bool dec_var, bool asgn_var);
int id_statement(ASTNode *node, bool dec_var, bool asgn_var);
void if_statement(ASTNode *node, bool dec_var, bool asgn_var, const char *label);
int while_loop(ASTNode *node, const char* label_prefix, bool dec_var, bool asgn_var);
int for_loop(ASTNode *node, const char* label, bool dec_var, bool asgn_var);
void return_statement(ASTNode *node);
void break_statement(ASTNode *node, const char *label);
void continue_statement(ASTNode *node, const char *label);
void expression(ASTNode *node);
void func_call(ASTNode *node);
void expression_list(ASTNode *node);
void asgn_found(ASTNode *node, const char *var);
void convert_string(dyn_str *dyn_s, char *str);

#endif // CODEGEN_PRIV_H
