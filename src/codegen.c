#include "codegen_priv.h"

unsigned if_counter = 0;
unsigned while_counter = 0;
unsigned for_counter = 0;
char *curr_func = NULL;

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
    curr_func = malloc(strlen(node->token->value.string_value) + 1);
    strcpy(curr_func, node->token->value.string_value);
    printf("%s\n", node->token->value.string_value);
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    node = node->left->left; // params or RPAREN
    if (node->type == P_PARAM_LIST){

        param_list(node);

        node = node->left; // RPAREN
    }
    
    node = node->left->left; // block

    statement(node->right, true, true, NULL);

    printf("POPFRAME\n");
    printf("RETURN\n");

    free(curr_func);
    function_def(node->left);
}

void param_list(ASTNode *node){
    if (node == NULL)
        return;

    // else is param

    printf("DEFVAR TF@");

    node = node->right; // ID
    ASTNode *idNode = node;
    printf("%s\n", node->token->value.string_value);

    node = node->left->left; // comma or null

    if (node != NULL){
        param_list(node->right);
    }

    printf("POPS TF@%s\n", idNode->token->value.string_value);

}

void statement(ASTNode *node, bool dec_var, bool var_asgn, const char *label){
    if (node == NULL)
        return;

    node = node->right;

    ASTNode *nextStatement = node->left;

    switch(node->type){
        case P_VAR_DECLARATION:
            var_dec(node, dec_var, var_asgn);
            break;

        case ID:
            id_statement(node, dec_var, var_asgn);
            nextStatement = nextStatement->left;
            break;

        case P_IF_STATEMENT:
            if_statement(node, dec_var, var_asgn, label);
            break;

        case P_WHILE_LOOP: {
            char label_prefix[strlen(curr_func) + 10];

            sprintf(label_prefix, "&%s_while_%d", curr_func, while_counter++);

            while_loop(node, label_prefix, dec_var, var_asgn);
            break;}

        case P_FOR_LOOP: {
            char label_prefix[strlen(curr_func) + 10];

            sprintf(label_prefix, "&%s_while_%d", curr_func, while_counter++);

            for_loop(node, label_prefix, dec_var, var_asgn);
            break;}
        
        case P_RETURN_STATEMENT:
            if (var_asgn)
                return_statement(node);
            nextStatement = NULL; // unreachable code
            break;

        case P_BREAK: 
            if (var_asgn)
                break_statement(node, label);
            nextStatement = NULL; // unreachable code
            break;

        case P_CONTINUE: 
            if (var_asgn)
                continue_statement(node, label);
            nextStatement = NULL; // unreachable code
            break;

        default:
            printf("statement unknown %d\n", node->type);
            break;

    }

    statement(nextStatement, dec_var, var_asgn, label);
}

void var_dec(ASTNode *node, bool dec_var, bool var_asgn){
    node = node->right->left; // ID
    if (dec_var){
        printf("DEFVAR TF@%s\n", node->token->value.string_value);
    }
    ASTNode *idNode = node;

    node = node->left; // P_TYPE_COMPLETE | ASSGN

    if(node->type == P_TYPE_COMPLETE)
        node = node->left; // ASSGN

    // node = node->right; // P_ASGN_FOUND
    if (var_asgn)
        asgn_found(node->right, idNode->token->value.string_value);    
}

void asgn_found(ASTNode *node, const char *var){
    expression(node->right->right);
    printf("POPS ");
    if(!strcmp(var, "_")) // discard
        printf("GF@&discard\n");
    else
        printf("TF@%s\n", var);
}

void expression(ASTNode *node){ // @as
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
            printf("GTS\n");
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
            printf("LTS\n");
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
        case IDIV:
            expression(node->left);
            expression(node->right);
            printf("IDIVS\n");
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
            printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\nPOPS TF@tmp\nPUSHS TF@tmp\n");
            printf("PUSHS nil@nil\nJUMPIFNEQS &orelse_not_null\n");
            expression(node->right);
            printf("JUMP &orelse_end\n");
            printf("LABEL &orelse_not_null\n");
            printf("PUSHS TF@tmp\n");
            printf("LABEL &orelse_end\nPOPFRAME\n");
            break;
        case ID:
            if (node->right == NULL && node->left == NULL){
                printf("PUSHS TF@%s\n", node->token->value.string_value);
            } else {
                if (node->left != NULL) { // builtin function

                    const char *builtin_func = node->left->token->value.string_value;
                    if (!strcmp(builtin_func, "readstr")){
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\n");
                        printf("READ TF@tmp string\nPUSHS TF@tmp\n");
                        printf("POPFRAME\n");
                    } else if (!strcmp(builtin_func, "readi32")){
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\n");
                        printf("READ TF@tmp int\nPUSHS TF@tmp\n");
                        printf("POPFRAME\n");
                    } else if (!strcmp(builtin_func, "readf64")){
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\n");
                        printf("READ TF@tmp float\nPUSHS TF@tmp\n");
                        printf("POPFRAME\n");
                    } else if (!strcmp(builtin_func, "i2f")){ 
                        expression(node->right->right);
                        printf("INT2FLOATS\n");
                    } else if (!strcmp(builtin_func, "f2i")){ 
                        expression(node->right->right);
                        printf("FLOAT2INTS\n");
                    } else if (!strcmp(builtin_func, "string")){
                        expression(node->right->right); // doesn't matter for IFJcode24
                    } else if (!strcmp(builtin_func, "length")){
                        expression(node->right->right);
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\nPOPS TF@tmp\n");
                        printf("STRLEN TF@tmp TF@tmp\nPUSHS TF@tmp\nPOPFRAME\n");
                    } else if (!strcmp(builtin_func, "concat")){
                        expression_list(node); 
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp1\nDEFVAR TF@tmp2\n");
                        printf("POPS TF@tmp2\nPOPS TF@tmp1\nCONCAT TF@tmp1 TF@tmp1 TF@tmp2\nPUSHS TF@tmp1\nPOPFRAME\n");
                    } else if (!strcmp(builtin_func, "substring")){
                        expression_list(node); 
                        printf("CALL &substring\n");
                    } else if (!strcmp(builtin_func, "strcmp")){
                        expression_list(node); 
                        printf("CALL &strcmp\n");
                    } else if (!strcmp(builtin_func, "ord")){
                        expression_list(node); 
                        printf("STRI2INTS\n");
                    } else if (!strcmp(builtin_func, "chr")){
                        expression(node->right->right);
                        printf("INT2CHARS\n");
                    } else {
                        printf ("unknown builtin function %s\n", builtin_func);
                    }
                } else
                    func_call(node);
            }
            break;
        case T_UNREACHABLE:
            printf("WRITE string@panic:\\032reached\\032reached\\032unreachable\\032code\\010\nEXIT int@57\n");
            break;
        case TYPE_INT:
            printf("PUSHS int@%lld\n", node->token->value.int_value);
            break;
        case TYPE_F64:
            printf("PUSHS float@%a\n", node->token->value.float_value);
            break;
        case T_TRUE:
            printf("PUSHS bool@true\n");
            break;
        case T_FALSE:
            printf("PUSHS bool@false\n");
            break;
        case TYPE_STR:{
            if(!strcmp(node->token->value.string_value, "")) {
                printf("PUSHS string@\n");
            } else {
                dyn_str *str = dyn_str_init();
                convert_string(str, node->token->value.string_value);
                printf("PUSHS string@%s\n", str->str);
                dyn_str_free(str);}
            break;}
        case T_NULL:
            printf("PUSHS nil@nil\n");
            break;
        case I2F:
            expression(node->right);
            printf("INT2FLOATS\n");
            break;
        case F2I:
            expression(node->right);
            printf("FLOAT2INTS\n");
            break;
        case T_IF:{
            node = node->left; // P_EXPRESSION
            expression(node->right); 
            printf("PUSHS bool@true\n");
            printf("JUMPIFNEQS &if_%d_else\n", if_counter);
            node = node->left; 
            expression(node->right); // value if true
            printf("JUMP &if_%d_end\n", if_counter);
            printf("LABEL &if_%d_else\n", if_counter);
            node = node->left;
            expression(node->right); // value if false
            printf("LABEL &if_%d_end\n", if_counter++);
            break;}
        default:
            printf("expression unknown\n");
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

    node = node->right;
    expression(node->right);

    if (node->left != NULL){
        expression_list(node->left->left);
    }

}

void id_statement(ASTNode *node, bool dec_var, bool var_asgn){
    // printf("id_statement %s ", node->token->value.string_value);
    ASTNode *idNode = node;
    node = node->left->right; // P_ASGN_FOUND | ID | L_PAREN | P_WHILE_LOOP
    switch(node->type){
        case P_ASGN_FOUND:
            if (var_asgn)
                asgn_found(node, idNode->token->value.string_value);
            break;

        case ID:
            if (var_asgn){
                expression(node->left->left->right->right);
                printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@out\nPOPS TF@out\nPUSHS TF@out\n");
                printf("WRITE TF@out\nPOPFRAME\n");
            }
            break;

        case LPAREN:
            if (var_asgn)
                func_call(idNode);
            break;

        case P_WHILE_LOOP:{ 
            char label_prefix[strlen(curr_func) + strlen(idNode->token->value.string_value) + 4];
            sprintf(label_prefix, "&%s_%s", curr_func, idNode->token->value.string_value);
            while_loop(node, label_prefix, dec_var, var_asgn);
            break;}
        
        default:
            printf("id unknown\n");
            break;
    }

}   

void if_statement(ASTNode *node, bool dec_var, bool var_asgn, const char *label){ 
    int if_count;
    if (var_asgn)
        if_count = if_counter++;
    
    node = node->right; // P_EXPRESSION
    if (var_asgn)
        expression(node->right);
    node = node->left->right; // P_BLOCK | P_OPTIONAL_VALUE
    if (node->type == P_OPTIONAL_VALUE){
        if (dec_var)
            printf("DEFVAR TF@%s\n", node->right->token->value.string_value);

        if (var_asgn){
            printf("POPS TF@%s\nPUSHS TF@%s\n", node->right->token->value.string_value, node->right->token->value.string_value);
            printf("PUSHS nil@nil\n");
            printf("JUMPIFEQS &if_%d_else\n", if_count);
        }
        node = node->left; // P_BLOCK
    } else {
        if (var_asgn){
            printf("PUSHS bool@true\n");
            printf("JUMPIFNEQS &if_%d_else\n", if_count);
        }
    }

    statement(node->right, dec_var, var_asgn, label);
    
    if (var_asgn){
        printf("JUMP &if_%d_end\n", if_count);
        printf("LABEL &if_%d_else\n", if_count);
    }
    if(node->left != NULL){
        node = node->left; // P_ELSE_STATEMENT
        if(node->right->type == P_BLOCK)
            node = node->right->right;
        statement(node, dec_var, var_asgn, label);
    }
    if (var_asgn)
        printf("LABEL &if_%d_end\n", if_count);
}

void while_loop(ASTNode *node, const char* label, bool dec_var, bool var_asgn){
    ASTNode *this_loop = node;
    node = node->right; // P_EXPRESSION

    bool first_loop = false;
    if (dec_var && var_asgn){
        first_loop = true;
        var_asgn = false;
    }

    if (var_asgn) {
        printf("LABEL %s_loop\n", label);
        expression(node->right);
    }

    node = node->left; // P_BLOCK | P_OPTIONAL_STATEMENTS | P_OPTIONAL_VALUE
    
    if (node->type == P_OPTIONAL_VALUE) {
        if (dec_var)
            printf("DEFVAR TF@%s\n", node->right->token->value.string_value);
        
        if (var_asgn) {
            printf("POPS TF@%s\nPUSHS TF@%s\n", node->right->token->value.string_value, node->right->token->value.string_value); // save value and leave it in data stack
            printf("PUSHS nil@nil\n");
            printf("JUMPIFEQS %s_break\n", label); // compare if value is null
        }
        node = node->left; // P_BLOCK | P_OPTIONAL_STATEMENTS
    } else {
        if (var_asgn) {
            printf("PUSHS bool@true\n");
            printf("JUMPIFNEQS %s_break\n", label); // check if condition is true 
        }
    }

    ASTNode *continue_statements = NULL;   
    if (node->type == P_OPTIONAL_STATEMENTS){
        continue_statements = node->right; // P_BLOCK | P_SINGLE_STATEMENT
        node = node->left; // P_BLOCK
    }
    
    statement(node->right, dec_var, var_asgn, label); // loop block

    if (var_asgn)
        printf("LABEL %s_continue\n", label);

    if (continue_statements != NULL){
        if (continue_statements->type == P_BLOCK)
            continue_statements = continue_statements->right; // P_STATEMENT
        statement(continue_statements, dec_var, var_asgn, label);  // continue statement/s
    }

    if (var_asgn) {
        printf("JUMP %s_loop\n", label);
        printf("LABEL %s_break\n", label);
    }

    node = node->left; // P_ELSE_STATEMENT
    
    if (node != NULL) {
        node = node->right->right;
        statement(node, dec_var, var_asgn, label); 
    }

    if (var_asgn)
        printf("LABEL %s_end\n", label);
    
    if (first_loop) {
        while_loop(this_loop, label, false, true);
        return;
        // statement(node->right, true, false); // loop block
    }
}

void for_loop(ASTNode *node, const char* label, bool dec_var, bool var_asgn){
    printf("for\n");
}

void return_statement(ASTNode *node){
    if (node->right != NULL) {
        expression(node->right->right);
    }

    printf ("POPFRAME\nRETURN\n");

}

void break_statement(ASTNode *node, const char *label){
    if (node->right != NULL)
        printf("JUMP &%s_%s_end\n", curr_func, node->right->token->value.string_value);
    else
        printf("JUMP %s_end\n", label);
}

void continue_statement(ASTNode *node, const char *label){
    if (node->right != NULL)
        printf("JUMP &%s_%s_continue\n", curr_func, node->right->token->value.string_value);
    else
        printf("JUMP %s_continue\n", label);

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
