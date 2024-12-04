/*
file: codegen_priv.h
author: Marek Hric xhricma00
Header file for generator private functions.
*/

#include "codegen.h"
#include "linked_list.h"

// macro used for variable declaraton  
#define VARDEC(N) do{\
if(!LLFind(var_list, N)){\
            int err = LLInsert(var_list, N);\
            if (err){\
                fprintf(stderr, "Error: Allocation error in code generation\n"); \ 
                LLDispose(var_list);\
                free(var_list);\
                free(curr_func);\
                return 99;\
            }\
            printf("DEFVAR TF@%s\n", N);\
        }} while (0)


#ifndef CODEGEN_PRIV_H
#define CODEGEN_PRIV_H

/**
 * prog
 * root - root of the AST
 * skips unnecessary prolog nodes and calls function_def
 * returns 0 if successful, 99 if allocation error
 */
int prog(ASTNode *root);

/**
 * function_def
 * node - function definition node
 * prints function label, creates frame
 * calls param_list if there are any parameters
 * creates linked list for variables of the function
 * calls statement for variable declaration
 * calls statement for statement execution
 * returns 0 if successful, 99 if allocation error
 */
int function_def(ASTNode *node);

/**
 * param_list
 * node - parameter list node
 * prints DEFVAR and POPS for each parameter
 */
void param_list(ASTNode *node);

/**
 * statement
 * node - statement node
 * dec_var - true if variable declaration is allowed
 * var_asgn - true if variable assignment is allowed
 * label - label for loops
 * returns 0 if successful, 99 if allocation error
 */
int statement(ASTNode *node, bool dec_var, bool asgn_var, const char *label);

/**
 * var_dec
 * node - variable declaration node
 * dec_var - true if variable declaration is allowed
 * asgn_var - true if variable assignment is allowed
 * returns 0 if successful, 99 if allocation error
 */
int var_dec(ASTNode *node, bool dec_var, bool asgn_var);

/**
 * id_statement
 * node - id statement node
 * dec_var - true if variable declaration is allowed
 * asgn_var - true if variable assignment is allowed
 * returns 0 if successful, 99 if allocation error
 */
int id_statement(ASTNode *node, bool dec_var, bool asgn_var);

/**
 * if_statement
 * node - if statement node
 * dec_var - true if variable declaration is allowed
 * asgn_var - true if variable assignment is allowed
 * label - label for loops
 * returns 0 if successful, 99 if allocation error
 */
int if_statement(ASTNode *node, bool dec_var, bool asgn_var, const char *label);

/**
 * while_loop
 * node - while loop node
 * label - label for loops
 * dec_var - true if variable declaration is allowed
 * asgn_var - true if variable assignment is allowed
 * returns 0 if successful, 99 if allocation error
 */
int while_loop(ASTNode *node, const char* label_prefix, bool dec_var, bool asgn_var);

/**
 * for_loop
 * node - for loop node
 * label - label for loops
 * dec_var - true if variable declaration is allowed
 * asgn_var - true if variable assignment is allowed
 * returns 0 if successful, 99 if allocation error
 */
int for_loop(ASTNode *node, const char* label, bool dec_var, bool asgn_var);

/**
 * return_statement
 * node - return statement node
 * if any expression is present, calls expression
 * return expression is left on the data stack
 * prints POPFRAME and RETURN
 */
void return_statement(ASTNode *node);

/**
 * break_statement
 * node - break statement node
 * label - label for loops
 * prints JUMP to the end of the current loop if no right node is present
 * prints JUMP to the end of the loop with the label from the right node
 */
void break_statement(ASTNode *node, const char *label);

/**
 * continue_statement
 * node - continue statement node
 * label - label for loops
 * prints JUMP to the continue of the current loop if no right node is present
 * prints JUMP to the continue of the loop with the label from the right node
 */
void continue_statement(ASTNode *node, const char *label);

/**
 * expression
 * node - expression node
 * recursively calculates the expression on data stack
 * leaves the result on the data stack
 */
void expression(ASTNode *node);

/**
 * func_call
 * node - function call node
 * expression - true if the function call is in an expression (makes difference in tree traversal)
 * calls expression_list if there are any parameters
 * prints CALL to the function
 */
void func_call(ASTNode *node, bool expression);

/**
 * expression_list
 * node - expression list node
 * calls expression function for each expression in the list
 */
void expression_list(ASTNode *node);

/**
 * asgn_found
 * node - assignment node
 * var - variable name
 * pops top of the data stack and assigns it to the variable
 */
void asgn_found(ASTNode *node, const char *var);

/**
 * convert_string
 * dyn_s - dynamic string
 * str - string to convert
 * converts string to the format of IFJcode24 output
 */
void convert_string(dyn_str *dyn_s, char *str);

#endif // CODEGEN_PRIV_H
