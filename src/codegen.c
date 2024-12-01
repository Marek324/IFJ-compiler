#include "codegen_priv.h"
#include "linked_list.h"

unsigned counter = 0;
char *curr_func = NULL;
LinkedList_ptr var_list = NULL;

int codegen()
{
    // IFJcode24 prolog
    printf("\
.IFJcode24\n\
DEFVAR GF@&discard\n\
CREATEFRAME\n\
CALL main\n\
EXIT int@0\n\
\n\
LABEL &strcmp\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@s1\n\
DEFVAR TF@s2\n\
POPS TF@s2\n\
POPS TF@s1\n\
DEFVAR TF@cnt\n\
DEFVAR TF@res\n\
LT TF@cnt TF@s1 TF@s2\n\
JUMPIFEQ &strcmp_smaller TF@cnt bool@true\n\
GT TF@cnt TF@s1 TF@s2\n\
JUMPIFEQ &strcmp_bigger TF@cnt bool@true\n\
MOVE TF@res int@0\n\
PUSHS TF@res\n\
POPFRAME\n\
RETURN\n\
LABEL &strcmp_smaller\n\
MOVE TF@res int@-1\n\
PUSHS TF@res\n\
POPFRAME\n\
RETURN\n\
LABEL &strcmp_bigger\n\
MOVE TF@res int@1\n\
PUSHS TF@res\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL &ord\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@res\n\
DEFVAR TF@s\n\
DEFVAR TF@i\n\
POPS TF@i\n\
POPS TF@s\n\
\
DEFVAR TF@len\n\
STRLEN TF@len TF@s\n\
\
PUSHS int@0\n\
PUSHS TF@len\n\
JUMPIFEQS &ord_err\n\
\
PUSHS TF@i\n\
PUSHS int@0\n\
LTS\n\
PUSHS bool@true\n\
JUMPIFEQS &ord_err\n\
\
PUSHS TF@i\n\
PUSHS TF@len\n\
LTS\n\
NOTS\n\
PUSHS bool@true\n\
JUMPIFEQS &ord_err\n\
\
PUSHS TF@s\n\
PUSHS TF@i\n\
STRI2INTS\n\
POPFRAME\n\
RETURN\n\
\
LABEL &ord_err\n\
MOVE TF@res int@0\n\
PUSHS TF@res\n\
POPFRAME\n\
RETURN\n\
\n\
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
");

    return prog(ASTRoot);


}

int prog(ASTNode *root){
    root = root->right->left; // skip prolog
    return function_def(root);
}


int function_def(ASTNode *node){
    if (node == NULL)
        return 0;

    // else is function def

    printf("\nLABEL ");
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

    // declare all variables in the function
    var_list = (LinkedList_ptr) malloc(sizeof(LinkedList_t));
    LLInit(var_list);
    int err = statement(node->right, true, false, NULL);
    if (err) return 99;
    LLDispose(var_list);
    free(var_list);

    // execute the function statements without declaring
    // no need to check for error as it can only be allocation error
    statement(node->right, false, true, NULL); // nechyba tu return ?

    printf("POPFRAME\n");
    printf("RETURN\n");

    free(curr_func);
    return function_def(node->left);
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

int statement(ASTNode *node, bool dec_var, bool var_asgn, const char *label){
    if (node == NULL)
        return 0;

    node = node->right;

    if(node == NULL) // NULL in empty block
        return 0;

    ASTNode *nextStatement = node->left;

    switch(node->type){
        case P_VAR_DECLARATION:{
            int err = var_dec(node, dec_var, var_asgn);
            if (err) return 99;            
            break;}

        case ID:{
            int err = id_statement(node, dec_var, var_asgn);
            if (err) return 99;
            nextStatement = nextStatement->left;
            break;}

        case P_IF_STATEMENT:
            if_statement(node, dec_var, var_asgn, label);
            break;

        case P_WHILE_LOOP: {
            char label_prefix[strlen(curr_func) + 12];

            int while_num = var_asgn ? counter++ : 0;
            sprintf(label_prefix, "&%s_while_%d", curr_func, while_num);

            int err = while_loop(node, label_prefix, dec_var, var_asgn);
            if (err) return 99;
            break;}

        case P_FOR_LOOP: {
            char label_prefix[strlen(curr_func) + 12];

            int for_num = var_asgn ? counter++ : 0;
            sprintf(label_prefix, "&%s_while_%d", curr_func, for_num);

            int err = for_loop(node, label_prefix, dec_var, var_asgn);
            if (err) return 99;
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

    return statement(nextStatement, dec_var, var_asgn, label);
}

int var_dec(ASTNode *node, bool dec_var, bool var_asgn){
    node = node->right->left; // ID
    if (dec_var){
        if(!LLFind(var_list, node->token->value.string_value)){
            int err = LLInsert(var_list, node->token->value.string_value);
            if (err){
                fprintf(stderr, "Error: Allocation error in code generation\n");  
                LLDispose(var_list);
                free(var_list);
                free(curr_func);
                return 99;
            }
                
            printf("DEFVAR TF@%s\n", node->token->value.string_value);
        } 
    }
    ASTNode *idNode = node;

    node = node->left; // P_TYPE_COMPLETE | ASSGN

    if(node->type == P_TYPE_COMPLETE)
        node = node->left; // ASSGN

    // node = node->right; // P_ASGN_FOUND
    if (var_asgn)
        asgn_found(node->right, idNode->token->value.string_value);    

    return 0;
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
        case T_ORELSE:{
            int id = counter++;
            expression(node->left);
            printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\nPOPS TF@tmp\nPUSHS TF@tmp\n");
            printf("PUSHS nil@nil\nJUMPIFNEQS &orelse_%d_not_null\n", id);
            expression(node->right);
            printf("JUMP &orelse_%d_end\n", id);
            printf("LABEL &orelse_%d_not_null\n", id);
            printf("PUSHS TF@tmp\n");
            printf("LABEL &orelse_%d_end\nPOPFRAME\n", id);
            break;}
        case ID:
            if (node->right == NULL && node->left == NULL){
                printf("PUSHS TF@%s\n", node->token->value.string_value);
            } else {
                if (node->right != NULL) { // builtin function

                    const char *builtin_func = node->right->token->value.string_value;
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
                        expression_list(node->left);
                    } else if (!strcmp(builtin_func, "length")){
                        expression_list(node->left);
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp\nPOPS TF@tmp\n");
                        printf("STRLEN TF@tmp TF@tmp\nPUSHS TF@tmp\nPOPFRAME\n");
                    } else if (!strcmp(builtin_func, "concat")){
                        expression_list(node->left); 
                        printf("PUSHFRAME\nCREATEFRAME\nDEFVAR TF@tmp1\nDEFVAR TF@tmp2\n");
                        printf("POPS TF@tmp2\nPOPS TF@tmp1\nCONCAT TF@tmp1 TF@tmp1 TF@tmp2\nPUSHS TF@tmp1\nPOPFRAME\n");
                    } else if (!strcmp(builtin_func, "substring")){
                        expression_list(node->left); 
                        printf("CALL &substring\n");
                    } else if (!strcmp(builtin_func, "strcmp")){
                        expression_list(node->left); 
                        printf("CALL &strcmp\n");
                    } else if (!strcmp(builtin_func, "ord")){
                        expression_list(node->left); 
                        printf("CALL &ord\n");
                    } else if (!strcmp(builtin_func, "chr")){
                        expression_list(node->left);
                        printf("INT2CHARS\n");
                    } else {
                        printf ("unknown builtin function %s\n", builtin_func);
                    }
                } else
                    func_call(node, true);
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
            printf("JUMPIFNEQS &if_%d_else\n", counter);
            node = node->left; 
            expression(node->right); // value if true
            printf("JUMP &if_%d_end\n", counter);
            printf("LABEL &if_%d_else\n", counter);
            node = node->left;
            expression(node->right); // value if false
            printf("LABEL &if_%d_end\n", counter++);
            break;}
        default:
            printf("expression unknown\n");
            break;
    }

    
}

/**
 * expression - true if this function is called from expression(), 
 * func_call in expressions = id --left-> P_EXPRESSION_LIST
 * func_call in statements = id --left-> P_ID_FOUND --right-> L_PAREN --left-> P_EXPRESSION_LIST
 */
void func_call(ASTNode *node, bool expression){
    if (node == NULL)
        return;
    
    ASTNode *idNode = node;

    node = node->left;
    if (!expression)
        node = node->right->left; 
    
    expression_list(node);
    printf("CALL %s\n", idNode->token->value.string_value);
}

void expression_list(ASTNode *node){
    if (node == NULL)
        return;

    node = node->right;
    if(node != NULL){
        expression(node->right);
        if (node->left != NULL){
            expression_list(node->left->left);
        }
    }


}

int id_statement(ASTNode *node, bool dec_var, bool var_asgn){
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
                func_call(idNode, false);
            break;

        case P_WHILE_LOOP:{ 
            char label_prefix[strlen(curr_func) + strlen(idNode->token->value.string_value) + 4];
            sprintf(label_prefix, "&%s_%s", curr_func, idNode->token->value.string_value);
            int err = while_loop(node, label_prefix, dec_var, var_asgn);
            if (err) return 99;
            break;}
        
        default:
            printf("id unknown\n");
            break;
    }

    return 0;

}   

void if_statement(ASTNode *node, bool dec_var, bool var_asgn, const char *label){ 
    int if_count;
    if (var_asgn)
        if_count = counter++;
    
    node = node->right; // P_EXPRESSION
    if (var_asgn)
        expression(node->right);
    node = node->left->right; // P_BLOCK | P_OPTIONAL_VALUE
    if (node->type == P_OPTIONAL_VALUE){
        if (dec_var){
            if(!LLFind(var_list, node->right->token->value.string_value)){
                int err = LLInsert(var_list, node->right->token->value.string_value);
                if (err){
                    fprintf(stderr, "Error: Allocation error in code generation\n");  
                    LLDispose(var_list);
                    free(var_list);
                    free(curr_func);
                    exit(99);
                }
                printf("DEFVAR TF@%s\n", node->right->token->value.string_value);
            }
        }

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

int while_loop(ASTNode *node, const char* label, bool dec_var, bool var_asgn){
    node = node->right; // P_EXPRESSION

    if (var_asgn) {
        printf("LABEL %s_loop\n", label);
        expression(node->right);
    }

    node = node->left; // P_BLOCK | P_OPTIONAL_STATEMENTS | P_OPTIONAL_VALUE
    
    if (node->type == P_OPTIONAL_VALUE) {
        if (dec_var){
            if(!LLFind(var_list, node->right->token->value.string_value)){
                LLInsert(var_list, node->right->token->value.string_value);
                printf("DEFVAR TF@%s\n", node->right->token->value.string_value);
            }
        }
        
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
    
    int err = statement(node->right, dec_var, var_asgn, label); // loop block
    if (err) return 99;

    if (var_asgn)
        printf("LABEL %s_continue\n", label);

    if (continue_statements != NULL){
        if (continue_statements->type == P_BLOCK)
            continue_statements = continue_statements->right; // P_STATEMENT
        
        int err = statement(continue_statements, dec_var, var_asgn, label);  // continue statement/s
        if (err) return 99;
    }

    if (var_asgn) {
        printf("JUMP %s_loop\n", label);
        printf("LABEL %s_break\n", label);
    }

    node = node->left; // P_ELSE_STATEMENT
    
    if (node != NULL) {
        node = node->right->right;
        int err = statement(node, dec_var, var_asgn, label); 
        if (err) return 99;
    }

    if (var_asgn)
        printf("LABEL %s_end\n", label);

    return 0;
    
}

int for_loop(ASTNode *node, const char* label, bool dec_var, bool var_asgn){
    printf("for\n");
    return 0;
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
