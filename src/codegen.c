#include "codegen.h"


void prog(ASTNode *root);

void codegen()
{
//     printf("\
// .IFJcode21\n\
// CALL main\n\
// EXIT int@0\n\
// LABEL &strcmp\n\
// DEFVAR TF@&strcmp_cnt\n\
// LT TF@&strcmp_cnt TF@s1 TF@s2\n\
// JUMPIFEQ &strcmp_smaller TF@&strcmp_cnt bool@true\n\
// GT TF@&strcmp_cnt TF@s1 TF@s2\n\
// JUMPIFEQ &strcmp_bigger TF@&strcmp_cnt bool@true\n\
// MOVE TF@&strcmp_res int@0\n\
// RETURN\n\
// LABEL &strcmp_smaller\n\
// MOVE TF@&strcmp_res int@-1\n\
// RETURN\n\
// LABEL &strcmp_bigger\n\
// MOVE TF@&strcmp_res int@1\n\
// RETURN\n\
// LABEL &substring\n\
// MOVE TF@&substring_res string@\n\
// DEFVAR TF@&substring_err_cnt\n\
// LT TF@&substring_err_cnt TF@i int@0\n\
// JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
// LT TF@&substring_err_cnt TF@j int@0\n\
// JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
// LT TF@&substring_err_cnt TF@j TF@i\n\
// JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
// DEFVAR TF@&substring_strlen\n\
// STRLEN TF@&substring_strlen TF@s\n\
// GT TF@&substring_err_cnt TF@j TF@&substring_strlen\n\
// JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
// LT TF@&substring_err_cnt TF@i TF@&substring_strlen\n\
// NOT TF@&substring_err_cnt TF@&substring_err_cnt\n\
// JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
// DEFVAR TF@&substring_char\n\
// LABEL &substring_loop\n\
// GETCHAR TF@&substring_char TF@s TF@i\n\
// CONCAT TF@&substring_res TF@&substring_res TF@&substring_char\n\
// ADD TF@i TF@i int@1\n\
// JUMPIFNEQ &substring_loop TF@i TF@j\n\
// RETURN\n\
// LABEL &substring_err\n\
// MOVE TF@&substring_res nil@nil\n\
// RETURN\n\
// ");

    /*
    Volanie funkcii

        PUSHFRAME
        CREATEFRAME
        DEFVAR TF@&strcmp_res -- vysledok
        -- argumenty --
        DEFVAR TF@arg1
        MOVE TF@arg1 LF@a - a je argument
        
        DEFVAR TF@arg2 
        MOVE TF@arg2 LF@b - b je argument

        CALL &strcmp
        
    c je premenna kde sa to uklada
        MOVE LF@c TF@&strcmp_res
        POPFRAME
    */

    prog(ASTRoot);

        

}

void function_def(ASTNode *node);

void prog(ASTNode *root){
    root = root->right->left; // skip prolog
    function_def(root);
}

void param_list(ASTNode *node);
void statement(ASTNode *node);

void function_def(ASTNode *node){
    if (node == NULL)
        return;

    // else is function def

    printf("function ");
    node = node->right; // ID
    printf("%s\n", node->token->value.string_value);

    node = node->left->left; // params or RPAREN
    if (node->type == P_PARAM_LIST){

        param_list(node);

        node = node->left; // RPAREN
    }

    node = node->left->left; // block

    statement(node->right);

    function_def(node->left);


}

void param_list(ASTNode *node){
    if (node == NULL)
        return;

    // else is param

    printf("param ");

    node = node->right; // ID
    printf("%s\n", node->token->value.string_value);

    node = node->left->left; // comma or null

    if (node != NULL){
        param_list(node->right);
    }

}

void statement(ASTNode *node){
    if (node == NULL)
        return;

    printf("statement ");
    node = node->right;

    switch(node->type){
        case P_VAR_DECLARATION:
            printf("var_decl\n");
            break;

        case ID:
            node = node->left;
            printf("id\n");
            break;

        case P_IF_STATEMENT:
            printf("if\n");
            break;

        case P_WHILE_LOOP:
            printf("while\n");
            break;
        
        case P_RETURN_STATEMENT:
            printf("return\n");
            break;

        case P_BREAK:
            printf("break\n");
            break;

        case P_CONTINUE:
            printf("continue\n");
            break;

        case P_FOR_LOOP:
            printf("for\n");
            break;

    }
    
    statement(node->left);


}
