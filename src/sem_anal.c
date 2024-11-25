#include "sem_anal.h"
#include "ast.h"
#include "error.h"

// INT + INT CONVERSION FOR SOME REASON
void analyse (ASTNode* node) {
    if(node == NULL) {
        return;
    }
    if(node->type == P_EXPRESSION) {
        checkExpr(node->right);
    }
    analyse(node->left);
    analyse(node->right);
}

void checkExpr(ASTNode* node) {
    if(node == NULL) {
        return;
    }
    switch(node->type) {
        case DIV:
            checkDiv(node);
            checkBinTypes(node);
            break;
        case EQ:
        case NEQ:
        case LESS:
        case LEQ:
        case MORE:
        case MEQ:
        case PLUS:
        case MINUS:
        case MUL:
            checkBinTypes(node);
            break;
        case BANG:
            checkUnTypes(node);
            break;
        default:
            break;
    }
    checkExpr(node->left);
    checkExpr(node->right);
}
void symtable_get_function_param_info(symtable_node_ptr tree, char *key, ASTNode *ParamList, int i, int capacity) {
    symtable_node_ptr node = symtable_search(tree, key);
    
    if (node->entry->param_nullable == NULL && node->entry->param_types == NULL) {
        node->entry->param_nullable = malloc(capacity*sizeof(bool));
        node->entry->param_types = malloc(capacity*sizeof(ret_type));
        if (node->entry->param_nullable == NULL || node->entry->param_types == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "ERROR: Unable to allocate memory\n");
        }
    }

    if (i == capacity - 1) {
        capacity = 2 * capacity;
        node->entry->param_nullable = realloc(node->entry->param_nullable, capacity * sizeof(bool));
        node->entry->param_types = realloc(node->entry->param_types, capacity * sizeof(ret_type));
        if (node->entry->param_nullable == NULL || node->entry->param_types == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "ERROR: Unable to allocate memory\n");
        }
    }
    // move to ID
    ParamList = ParamList->right;
    if (ParamList != NULL) {
        (node->entry->param_count)++;
    }
    // move to P_TYPE_COMPLETE
    ParamList = ParamList->left;

    if (ParamList->right->type == P_QUESTION_MARK) {
        node->entry->param_nullable[i] = true;
        node->entry->param_types[i] = get_ret_type(convertToASTType(T_KW, ParamList->right->left->right->token->value.keyword));
        i++;
    } else if (ParamList->right->type == P_TYPE) {
        node->entry->param_nullable[i] = false;
        node->entry->param_types[i] = get_ret_type(convertToASTType(T_KW, ParamList->right->right->token->value.keyword));
        i++;
    }
    if (ParamList->left != NULL) {
        if (ParamList->left->right != NULL) {
            symtable_get_function_param_info(tree, key, ParamList->left->right, i, capacity);
        }
    }
}

void symtable_get_function_type(symtable_node_ptr tree, char *key, ASTNode *FunctionType) {
    symtable_node_ptr node = symtable_search(tree, key);

    if (FunctionType->right->type == T_VOID) {
        node->entry->type = get_ret_type(T_VOID);
        node->entry->returnsValue = false;

    } else {
        // Move to P_TYPE_COMPLETE
        FunctionType = FunctionType->right;

        if (FunctionType->right->type == P_QUESTION_MARK) {
            node->entry->isNullable = true;
            node->entry->returnsValue = true;
            node->entry->type = get_ret_type(convertToASTType(T_KW, FunctionType->right->left->right->token->value.keyword));

        } else if (FunctionType->right->type == P_TYPE) {
            node->entry->isNullable = false;
            node->entry->returnsValue = true;
            node->entry->type = get_ret_type(convertToASTType(T_KW, FunctionType->right->right->token->value.keyword));
        }
    }
}

ret_type get_ret_type(ASTNodeType type) {
    switch(type) {
        case T__KW_I32: return T_INT_RET;
        case T_KW_F64: return T_FLOAT_RET;
        case T_KW_BOOL: return T_BOOL_RET;
        case T_U8: return T_STR_RET;
        case T_VOID: return T_NULL_RET;
        default: return 99;
    }
}

void checkDiv(ASTNode* node) {    
    if(node->left->type == TYPE_INT && node->right->type == TYPE_INT) {
        node->token->type = T_IDIV;
        node->type = IDIV;
    }
}

void checkBinTypes(ASTNode* node) {
    if(node->left == NULL || node->right == NULL) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(7, "ERROR: Missing operand in binary operation!\n");
        return;
    }
    if(node->left->type == TYPE_INT && node->right->type == TYPE_F64) {
        ASTNode* temp = node->left;
        Token* new_token = (Token*)malloc(sizeof(Token));
        if (new_token == NULL){ 
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "Memory allocation failed"); 
        }
        new_token->type = T_I2F;
        // create node for orelse
        ASTNode* new_node = nodeCreate(I2F, new_token);
        new_node->right = temp;
        node->left = new_node;
    }
    if(node->right->type == TYPE_INT && node->left->type == TYPE_F64) {
        ASTNode* temp = node->right;
        Token* new_token = (Token*)malloc(sizeof(Token));
        if (new_token == NULL){ 
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "Memory allocation failed"); 
        }
        new_token->type = T_I2F;
        // create node for orelse
        ASTNode* new_node = nodeCreate(I2F, new_token);
        new_node->right = temp;
        node->right = new_node;
    }
    if(node->left->type != node->right->type) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(7, "ERROR: Wrong types in binary operation!\n");
    }
}

void checkUnTypes(ASTNode* node) {
    switch(node->right->type) {
        case TYPE_INT:
        case TYPE_F64:
        case ID:
        case BANG:
            return;
        default:
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type in unary operation!\n");
    }
}