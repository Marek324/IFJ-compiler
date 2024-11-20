#include "codegen.h"

void codegen()
{
    ASTNode *node = ASTRoot->right->left; // root->right is prolog, root->right->left is first function

    printf("\
        .IFJcode21\n\
        CALL main\n\
        EXIT int@0\n\
        LABEL &strcmp\n\
        DEFVAR TF@&strcmp_cnt\n\
        LT TF@&strcmp_cnt TF@s1 TF@s2\n\
        JUMPIFEQ &strcmp_smaller TF@&strcmp_cnt bool@true\n\
        GT TF@&strcmp_cnt TF@s1 TF@s2\n\
        JUMPIFEQ &strcmp_bigger TF@&strcmp_cnt bool@true\n\
        MOVE TF@&strcmp_res int@0\n\
        RETURN\n\
        LABEL &strcmp_smaller\n\
        MOVE TF@&strcmp_res int@-1\n\
        RETURN\n\
        LABEL &strcmp_bigger\n\
        MOVE TF@&strcmp_res int@1\n\
        RETURN\n\
        LABEL &substring\n\
        MOVE TF@&substring_res string@\n\
        DEFVAR TF@&substring_err_cnt\n\
        LT TF@&substring_err_cnt TF@i int@0\n\
        JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
        LT TF@&substring_err_cnt TF@j int@0\n\
        JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
        LT TF@&substring_err_cnt TF@j TF@i\n\
        JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
        DEFVAR TF@&substring_strlen\n\
        STRLEN TF@&substring_strlen TF@s\n\
        GT TF@&substring_err_cnt TF@j TF@&substring_strlen\n\
        JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
        LT TF@&substring_err_cnt TF@i TF@&substring_strlen\n\
        NOT TF@&substring_err_cnt TF@&substring_err_cnt\n\
        JUMPIFEQ &substring_err TF@&substring_err_cnt bool@true\n\
        DEFVAR TF@&substring_char\n\
        LABEL &substring_loop\n\
        GETCHAR TF@&substring_char TF@s TF@i\n\
        CONCAT TF@&substring_res TF@&substring_res TF@&substring_char\n\
        ADD TF@i TF@i int@1\n\
        JUMPIFNEQ &substring_loop TF@i TF@j\n\
        RETURN\n\
        LABEL &substring_err\n\
        MOVE TF@&substring_res nil@nil\n\
        RETURN\n\
    ");

    /*
    Volanie ifj.strcmp

    toto sa da hned pastnut ig
        PUSHFRAME
        CREATEFRAME
        DEFVAR TF@&strcmp_res
        DEFVAR TF@s1
        DEFVAR TF@s2
        
    toto sa riesi pri prechadzani expression_list
        MOVE TF@s1 LF@a - a je argument
        MOVE TF@s2 LF@b - b je argument

        CALL &strcmp
        
    c je premenna kde sa to uklada
        MOVE LF@c TF@&strcmp_res
        POPFRAME
    */

    

        

    EPILOG;

}

// void traverse_tree()