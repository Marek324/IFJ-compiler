#include "codegen.h"

#define PROLOG do {printf("\
.IFJcode24\n\
CREATEFRAME\n\
CALL main\n\
EXIT int@0\n\
LABEL &strcmp\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@s1\n\
DEFVAR TF@s2\n\
POPS TF@s2\n\
POPS TF@s1\n\
DEFVAR TF@cnt\n\
LT TF@cnt TF@s1 TF@s2\n\
JUMPIFEQ &strcmp_smaller TF@cnt bool@true\n\
GT TF@cnt TF@s1 TF@s\n\
JUMPIFEQ &strcmp_bigger TF@cnt bool@true\n\
MOVE TF@res int@0\n\
RETURN\n\
LABEL &strcmp_smaller\n\
MOVE TF@res int@-1\n\
RETURN\n\
LABEL &strcmp_bigger\n\
MOVE TF@res int@1\n\
PUSHS TF@res\n\
POPFRAME\n\
RETURN\n\
LABEL &substring\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@s\n\
DEFVAR TF@i\n\
DEFVAR TF@j\n\
POPS TF@j\n\
POPS TF@i\n\
POPS TF@s\n\
DEFVAR TF@res\n\
MOVE TF@res string@\n\
DEFVAR TF@cnt_err\n\
LT TF@cnt_err TF@i int@0\n\
JUMPIFEQ &substring_err TF@cnt_err bool@true\n\
LT TF@cnt_err TF@j int@0\n\
JUMPIFEQ &substring_err TF@cnt_err bool@true\n\
LT TF@cnt_err TF@j TF@i\n\
JUMPIFEQ &substring_err TF@cnt_err bool@true\n\
DEFVAR TF@len\n\
STRLEN TF@len TF@s\n\
GT TF@cnt_err TF@j TF@len\n\
JUMPIFEQ &substring_err TF@cnt_err bool@true\n\
LT TF@cnt_err TF@i TF@len\n\
NOT TF@cnt_err TF@cnt_err\n\
JUMPIFEQ &substring_err TF@cnt_err bool@true\n\
DEFVAR TF@char\n\
LABEL &substring_loop\n\
JUMPIFEQ &substring_loop_end TF@i TF@j\n\
GETCHAR TF@char TF@s TF@i\n\
CONCAT TF@res TF@res TF@char\n\
ADD TF@i TF@i int@1\n\
JUMP &substring_loop\n\
LABEL &substring_loop_end\n\
PUSHS TF@res\n\
POPFRAME\n\
RETURN\n\
LABEL &substring_err\n\
PUSHS nil@nil\n\
POPFRAME\n\
RETURN\n\
\n");} while(0)

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
void asgn_found(ASTNode *node, const char *var);
void ter_if(ASTNode *node, const char *var);
void print_out(ASTNode *node);
void convert_string(dyn_str *dyn_s, char *str);
