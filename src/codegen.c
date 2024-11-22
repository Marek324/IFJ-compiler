#include "codegen_priv.h"



void codegen()
{
    PROLOG;

    /*
    Volanie funkcii

    -- parametre --
        PUSHS TF@a
        PUSHS int@2
        PUSHS int@2

        CALL &substring

    -- return --
        POPS TF@c
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

    printf("LABEL ");
    node = node->right; // ID
    printf("%s\n", node->token->value.string_value);
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    node = node->left->left; // params or RPAREN
    if (node->type == P_PARAM_LIST){

        param_list(node);

        node = node->left; // RPAREN
    }

    node = node->left->left; // block

    statement(node->right);

    printf("POPFRAME\n");
    printf("RETURN\n");

    function_def(node->left);


}

void param_list(ASTNode *node){
    if (node == NULL)
        return;

    // else is param

    printf("DEFVAR ");

    node = node->right; // ID
    ASTNode *idNode = node;
    printf("%s\n", node->token->value.string_value);

    node = node->left->left; // comma or null

    if (node != NULL){
        param_list(node->right);
    }

    printf("POPS TF@%s\n", idNode->token->value.string_value);

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
    printf("DEFVAR ");
    ASTNode *nextStatement = node->left;
    node = node->right->left; // ID

    printf("TF@%s\n", node->token->value.string_value);
    ASTNode *idNode = node;

    node = node->left; // P_TYPE_COMPLETE | ASSGN

    if(node->type == P_TYPE_COMPLETE)
        node = node->left; // ASSGN

    asgn_found(node->right, idNode->token->value.string_value);    
    
    statement(nextStatement);
}

void asgn_found(ASTNode *node, const char *var){
    expression(node->right->right);
    printf("POPS TF@%s\n", var);
}

void expression(ASTNode *node){
    if (node == NULL)
        return;
    // printf("node type %d\n", node->type);
    switch(node->type){
        case EQ:
            expression(node->left);
            expression(node->right);
            printf("EQS\n");
            break;
        case BANG:
            expression(node->left);
            expression(node->right);
            printf("NOTS\n");
            break;
        case NEQ:
            expression(node->left);
            expression(node->right);
            printf("EQS\n");
            printf("NOTS\n");
            break;
        case LESS:
            expression(node->left);
            expression(node->right);
            printf("LTS\n");
            break;
        case LEQ:
            expression(node->left);
            expression(node->right);
            printf("LTS\n");
            printf("NOTS\n");
            break;
        case MORE:
            expression(node->left);
            expression(node->right);
            printf("GTS\n");
            break;
        case MEQ:
            expression(node->left);
            expression(node->right);
            printf("GTS\n");
            printf("NOTS\n");
            break;
        case PLUS:
            expression(node->left);
            expression(node->right);
            printf("ADDS\n");
            break;
        case MINUS:
            expression(node->left);
            expression(node->right);
            printf("SUBS\n");
            break;
        case MUL:
            expression(node->left);
            expression(node->right);
            printf("MULS\n");
            break;
        case DIV:
            expression(node->left);
            expression(node->right);
            printf("DIVS\n");
            break;
        case T_AND:
            expression(node->left);
            expression(node->right);
            printf("ANDS\n");
            break;
        case T_OR:
            expression(node->left);
            expression(node->right);
            printf("ORS\n");
            break;
        case T_ORELSE:
            expression(node->left);
            expression(node->right);
            printf("orelse\n");
            break;
        case ID:
            if (node->right == NULL){
                printf("PUSHS TF@%s\n", node->token->value.string_value);
            } else {
                func_call(node);
            }
            break;
        case T_UNREACHABLE:
            printf("unreachable\n");
            break;
        case TYPE_INT:
            printf("PUSHS int@%lld\n", node->token->value.int_value);
            break;
        case T_FLOAT:
            printf("PUSHS float@%lf\n", node->token->value.float_value);
            break;
        case T_TRUE:
            printf("PUSHS bool@true\n");
            break;
        case T_FALSE:
            printf("PUSHS bool@false\n");
            break;
        default:
            printf("unknown\n");
            break;
    }

    
}

void func_call(ASTNode *node){
    if (node == NULL)
        return;
    

    expression_list(node);
    printf("CALL %s\n", node->token->value.string_value);


}

void expression_list(ASTNode *node){
    if (node == NULL)
        return;

    expression(node->right->right);


    if (node->left != NULL){
        expression_list(node->left->left);
    }

}

void id_statement(ASTNode *node){
    // printf("id_statement %s ", node->token->value.string_value);
    ASTNode *idNode = node;
    node = node->left; // ID_FOUND
    ASTNode *nextStatement = node->left;

    node = node->right; // P_ASGN_FOUND | ID | L_PAREN | P_WHILE_LOOP
    switch(node->type){
        case P_ASGN_FOUND:
            asgn_found(node, idNode->token->value.string_value);
            break;

        case ID:
            const char *builtin_func = node->token->value.string_value;

            if (!strcmp(builtin_func, "readstr")){
                printf("ifj.readstr\n");
            } else if (!strcmp(builtin_func, "readi32")){
                printf("ifj.readi32\n");
            } else if (!strcmp(builtin_func, "readf64")){
                printf("ifj.readf64\n");
            } else if (!strcmp(builtin_func, "write")){
                printf("WRITE ");
                print_out(node->left->left->right->right);
            } else if (!strcmp(builtin_func, "i2f")){
                printf("ifj.i2f\n");
            } else if (!strcmp(builtin_func, "f2i")){
                printf("ifj.f2i\n");
            } else if (!strcmp(builtin_func, "string")){
                printf("ifj.string\n");
            } else if (!strcmp(builtin_func, "length")){
                printf("ifj.length\n");
            } else if (!strcmp(builtin_func, "concat")){
                printf("ifj.concat\n");
            } else if (!strcmp(builtin_func, "substring")){
                printf("ifj.substring\n");
            } else if (!strcmp(builtin_func, "strcmp")){
                printf("ifj.strcmp\n");
            } else if (!strcmp(builtin_func, "ord")){
                printf("ifj.ord\n");
            } else if (!strcmp(builtin_func, "chr")){
                printf("ifj.chr\n");
            } else {
                printf ("unknown builtin function %s\n", builtin_func);
            }

            
            break;

        case LPAREN:
            func_call(idNode);
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


void print_out(ASTNode *node){
    switch(node->type){
        case TYPE_STR:
            dyn_str *str = dyn_str_init();
            convert_string(str, node->token->value.string_value);
            printf("string@%s\n", str->str);
            dyn_str_free(str);
            break;
        case TYPE_INT:
            printf("int@%lld\n", node->token->value.int_value);
            break;
        case TYPE_F64:
            printf("float@%a\n", node->token->value.float_value);
            break;
        case ID:
            printf("TF@%s\n", node->token->value.string_value);
            break;
        default:
            break;


    }
}

void convert_string(dyn_str *dyn_s, char *str){
    for (int i = 0; i < (int) strlen(str); i++){
        if (str[i] <= ' ' || str[i] == '#' || str[i] == '\\'){
            dyn_str_append_str(dyn_s, "\\0");
            dyn_str_append(dyn_s, '0' + (str[i] / 10));
            dyn_str_append(dyn_s, '0' + (str[i] % 10));
        } else {
            dyn_str_append(dyn_s, str[i]);
        }
    }
}
