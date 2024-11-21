#include "codegen_priv.h"



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

void prog(ASTNode *root){
    root = root->right->left; // skip prolog
    function_def(root);
}


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

    node = node->right;

    switch(node->type){
        case P_VAR_DECLARATION:
            var_dec(node);
            break;

        case ID:
            id_statement(node);
            break;

        case P_IF_STATEMENT:
            if_statement(node);
            break;

        case P_WHILE_LOOP:
            while_loop(node);
            break;
        
        case P_RETURN_STATEMENT:
            return_statement(node);
            break;

        case P_BREAK:
            break_statement(node);
            break;

        case P_CONTINUE:
            continue_statement(node);
            break;

        case P_FOR_LOOP:
            for_loop(node);
            break;

        default:
            printf("unknown\n");
            break;

    }
    


}


void var_dec(ASTNode *node){
    printf("var_dec ");
    ASTNode *nextStatement = node->left;
    node = node->right->left; // ID

    printf("%s\n", node->token->value.string_value);

    printf("expression \n");
    expression(node->left->left->right->right->right); 
    
    
    statement(nextStatement);
}

void expression(ASTNode *node){
    if (node == NULL)
        return;
    // printf("node type %d\n", node->type);
    switch(node->type){
        case EQ:
            expression(node->left);
            expression(node->right);
            printf("eq\n");
            break;
        case BANG:
            expression(node->left);
            expression(node->right);
            printf("bang\n");
            break;
        case NEQ:
            expression(node->left);
            expression(node->right);
            printf("neq\n");
            break;
        case LESS:
            expression(node->left);
            expression(node->right);
            printf("less\n");
            break;
        case LEQ:
            expression(node->left);
            expression(node->right);
            printf("leq\n");
            break;
        case MORE:
            expression(node->left);
            expression(node->right);
            printf("more\n");
            break;
        case MEQ:
            expression(node->left);
            expression(node->right);
            printf("meq\n");
            break;
        case PLUS:
            expression(node->left);
            expression(node->right);
            printf("plus\n");
            break;
        case MINUS:
            expression(node->left);
            expression(node->right);
            printf("minus\n");
            break;
        case MUL:
            expression(node->left);
            expression(node->right);
            printf("mul\n");
            break;
        case DIV:
            expression(node->left);
            expression(node->right);
            printf("div\n");
            break;
        case T_AND:
            expression(node->left);
            expression(node->right);
            printf("and\n");
            break;
        case T_OR:
            expression(node->left);
            expression(node->right);
            printf("or\n");
            break;
        case T_ORELSE:
            expression(node->left);
            expression(node->right);
            printf("orelse\n");
            break;
        case ID:
            if (node->right == NULL){
                printf("id %s\n", node->token->value.string_value);
            } else {
                printf("func_call %s\n", node->token->value.string_value);
                func_call(node->right);
                printf("end_func_call %s\n", node->token->value.string_value);
            }
            break;
        case T_UNREACHABLE:
            printf("unreachable\n");
            break;
        case TYPE_INT:
            printf("int %lld\n", node->token->value.int_value);
            break;
        case T_FLOAT:
            printf("float %lf\n", node->token->value.float_value);
            break;
        case T_TRUE:
            printf("true\n");
            break;
        case T_FALSE:
            printf("false\n");
            break;
        default:
            printf("unknown\n");
            break;
    }

    
}

void func_call(ASTNode *node){

}

void id_statement(ASTNode *node){
    printf("id_statement %s ", node->token->value.string_value);
    node = node->left; // ID_FOUND
    ASTNode *nextStatement = node->left;

    node = node->right; // P_ASGN_FOUND | ID | L_PAREN | P_WHILE_LOOP
    switch(node->type){
        case P_ASGN_FOUND:
            printf("asgn\n");
            printf("expression\n");
            expression(node->right->right);
            break;

        case ID:
            printf("%s\n", node->token->value.string_value);
            break;

        case LPAREN:
            printf("l_paren\n");
            break;

        case P_WHILE_LOOP:
            printf("while\n");
            break;
        
        default:
            printf("unknown\n");
            break;
    }


    statement(nextStatement);
}   

void if_statement(ASTNode *node){

}

void while_loop(ASTNode *node){
    printf("while ");

}

void for_loop(ASTNode *node){

}

void return_statement(ASTNode *node){

}

void break_statement(ASTNode *node){
    
}

void continue_statement(ASTNode *node){

}
