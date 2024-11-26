#include "sem_anal.h"
#include "ast.h"
#include "error.h"
#include "exp_parser.h"

// TODO: implicit conversion and IDIV/DIV
void analyse (ASTNode* root) {
    checkForMain();
    root = root->right->left; // skip prolog
    symFuncDef(root);
}

void symFuncDef(ASTNode* node){
    if (node == NULL || node->type == END_OF_FILE) return;
    // function Sym node 
    node = node->right; // ID
    symtable_node_ptr tree = symtable_search(SymFunctionTree, node->token->value.string_value);
    tree = *tree->entry->local_symtable;

    node = node->left->left; // params or RPAREN
    if (node->type == P_PARAM_LIST){

        symParamList(node, &tree);
        
        node = node->left; // RPAREN
    }

    node = node->left->left; // block

    symStatement(node->right, &tree); 

    symFuncDef(node->left);
}
void symStatement(ASTNode* node, symtable_tree_ptr tree) {
    if (node == NULL)
        return;

    node = node->right;
    ASTNode *nextStatement = node->left;

    switch(node->type){

        case P_VAR_DECLARATION:
            symVarDec(node, tree);
            break;

        case ID:
            symIdStatement(node, tree);
            nextStatement = node->right;
            break;

        case P_IF_STATEMENT:
            symIfStatement(node, tree);
            break;

        case P_WHILE_LOOP:
            symWhileLoop(node, tree);
            break;
        
        case P_RETURN_STATEMENT:
            symReturnStatement(node, tree);
            break;

        case P_BREAK:
            symBreakStatement(node, tree);
            break;

        case P_CONTINUE:
            symContinueStatement(node, tree);
            break;

        case P_FOR_LOOP:
            symForLoop(node, tree);
            break;

        default:
            printf("unknown\n");
            break;
    }

    symStatement(nextStatement, tree);
}

void symParamList(ASTNode* node, symtable_tree_ptr tree) {
    if (node == NULL) return;
    
    ASTNode *id = node->right; // save ID
    symtable_insert(tree, id->token->value.string_value, T_VAR_SYM);
    node = id->left; // move to P_TYPE_COMPLETE

    symtable_node_ptr key = symtable_search(*tree, id->token->value.string_value);
        key->entry->isChanged = false;
        key->entry->hasExplicitType = true;
        key->entry->isUsed = false;
    if (node->right->type == P_QUESTION_MARK) {
        key->entry->isConst = true;
        key->entry->isNullable = true;
        key->entry->type = getRetType(convertToASTType(T_KW, node->right->left->right->token->value.keyword));
    }
    else if (node->right->type == P_TYPE) {
        key->entry->isConst = true;
        key->entry->isNullable = false;
        key->entry->type = getRetType(convertToASTType(T_KW, node->right->right->token->value.keyword));
    }
    
    node = node->left; // comma or null
    if (node != NULL) {
        symParamList(node->right, tree);
    }
}

void symVarDec(ASTNode* node, symtable_tree_ptr tree){
    node = node->right; // const or var
    bool isconst = node->token->value.keyword == KW_CONST ? true : false;
    node = node->left; // ID
    symtable_insert(tree, node->token->value.string_value, T_VAR_SYM);
    symtable_node_ptr key = symtable_search(*tree, node->token->value.string_value);
    node = node->left; // ASSGN OR P_TYPE_COMPLETE
    key->entry->isConst = isconst;
    key->entry->isUsed = false;
    key->entry->isChanged = false;
    ASTNode *Jozef = node->right;
    if (node->type == P_TYPE_COMPLETE) {
        key->entry->hasExplicitType = true;
        if(Jozef->type == P_TYPE) {
            key->entry->isNullable = false;
            Jozef = Jozef->right;
            key->entry->type = getRetType(convertToASTType(T_KW, Jozef->token->value.keyword));
        }
        else {
            key->entry->isNullable = true;
            Jozef = Jozef->left->right;
            key->entry->type = getRetType(convertToASTType(T_KW, Jozef->token->value.keyword));
        }
        node = node->left; // ASSGN
    }
    node = node->right->right; //P_EXPRESSION

    if (key->entry->hasExplicitType == true) {
        if (key->entry->type == checkExpr(node->right)) {
            return;
        } 
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);        
        error_exit(7, "ERROR: assigning wrong type\n");
    }
    else {
        key->entry->type = checkExpr(node->right);
        if (key->entry->type == T_NULL_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);        
            error_exit(8, "ERROR: cannot get type\n");
        }
        return;
    }
    
}

void symIdStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symIfStatement(ASTNode* node, symtable_tree_ptr tree){

}

void symWhileLoop(ASTNode* node, symtable_tree_ptr tree){
    
}

void symReturnStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symBreakStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symContinueStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symForLoop(ASTNode* node, symtable_tree_ptr tree){
    
}

ret_type checkExpr(ASTNode* node) {
    if(node == NULL) {
        return T_ERROR_RET;
    }
    if(isOperator(node->token)) {
        ret_type node_type;
        // unary boolean operation
        if(node->type == BANG) {
            if(checkUnType(node) == T_BOOL_RET) {
                return T_BOOL_RET;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for unary boolean operation!\n");
            }
        }
        // ternary operation
        if(node->type == T_IF) {
            return checkTernTypes(node->left);
        }
        // binary boolean operations
        if(node->type == T_AND || node->type == T_OR) {
            if(checkBool(node) == T_BOOL_RET) {
                return T_BOOL_RET;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for binary boolean operation!\n");
            }
        }
        // relation operation
        if(isRel(node->type)) {
            if(checkRel(node) == T_BOOL_RET) {
                return T_BOOL_RET;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for binary relation operation!\n");
            }
        }
        // binary arithmetic division
        if(node->type == DIV) {
            ret_type node_type = checkDiv(node);
            if(node_type == T_INT_RET || node_type == T_FLOAT) {
                return node_type == T_INT_RET ? T_INT_RET : T_FLOAT_RET;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for binary aritmetic operation!\n");
            }
        }
        // all other binary operations (arithmetic)
        node_type = checkAritTypes(node);
        if(node_type == T_INT_RET || node_type == T_FLOAT_RET) {
            return node_type == T_INT_RET ? T_INT_RET : T_FLOAT_RET;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type for binary aritmetic operation!\n");
        }
    }
    else if(isOperand(node->token)){
        // functions and IDs and literals
    }
}

void checkForMain() {
    symtable_node_ptr node = symtable_search(SymFunctionTree, "main");
    if (node == NULL) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(3, "ERROR: Missing main!\n");
    }
    if (node->entry->param_count != 0 || node->entry->returnsValue != false) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(4, "ERROR: Wrong main params or type!\n");
    }
}

void checkIfIdExits(symtable_node_ptr tree, char *key) {
    symtable_node_ptr node = symtable_search(tree, key);

    if (node == NULL) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(3, "ERROR: Missing definition!\n");
    }
}


void getFunctionParamInfo(symtable_node_ptr tree, char *key, ASTNode *ParamList, int i, int capacity) {
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
        node->entry->param_types[i] = getRetType(convertToASTType(T_KW, ParamList->right->left->right->token->value.keyword));
        i++;
    } else if (ParamList->right->type == P_TYPE) {
        node->entry->param_nullable[i] = false;
        node->entry->param_types[i] = getRetType(convertToASTType(T_KW, ParamList->right->right->token->value.keyword));
        i++;
    }
    if (ParamList->left != NULL) {
        if (ParamList->left->right != NULL) {
            getFunctionParamInfo(tree, key, ParamList->left->right, i, capacity);
        }
    }
}

void getFunctionType(symtable_node_ptr tree, char *key, ASTNode *FunctionType) {
    symtable_node_ptr node = symtable_search(tree, key);
    node->entry->isUsed = false;

    if (FunctionType->right->type == T_VOID) {
        node->entry->type = getRetType(T_VOID);
        node->entry->returnsValue = false;

    } else {
        // Move to P_TYPE_COMPLETE
        FunctionType = FunctionType->right;

        if (FunctionType->right->type == P_QUESTION_MARK) {
            node->entry->isNullable = true;
            node->entry->returnsValue = true;
            node->entry->type = getRetType(convertToASTType(T_KW, FunctionType->right->left->right->token->value.keyword));

        } else if (FunctionType->right->type == P_TYPE) {
            node->entry->isNullable = false;
            node->entry->returnsValue = true;
            node->entry->type = getRetType(convertToASTType(T_KW, FunctionType->right->right->token->value.keyword));
        }
    }
}

ret_type getRetType(ASTNodeType type) {
    switch(type) {
        case T__KW_I32: return T_INT_RET;
        case T_KW_F64: return T_FLOAT_RET;
        case T_KW_BOOL: return T_BOOL_RET;
        case T_U8: return T_STR_RET;
        case T_VOID: return T_NULL_RET;
        default: return 99;
    }
}

ret_type checkDiv(ASTNode* node) { 
    ret_type left_type;
    ret_type right_type;
    if(node->left->type == ID) {
        // get return_type, check if variable or function was defined
    }
    else if(isOperator(node->left->token)) {
        left_type = checkExpr(node->left);
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    // right node
    if(node->right->type == ID) {
        // get return type
    }
    else if(isOperator(node->right->token)) {
        right_type = checkExpr(node->right);
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    if(left_type == T_INT_RET && right_type == T_INT_RET) {
        node->token->type = T_IDIV;
        node->type = IDIV;
        return T_INT_RET;
    }
    else if((left_type == T_INT_RET || left_type == T_FLOAT_RET) && (right_type == T_INT_RET || right_type == T_FLOAT_RET)) {
        return checkAritTypes(node);
    }
    else {
        return T_ERROR_RET;
    }
}

ret_type checkAritTypes(ASTNode* node) {
    if(node->left == NULL || node->right == NULL) {
        return T_ERROR_RET;
    }
    ret_type left_type;
    ret_type right_type;
    // left node
    if(node->left->type == ID) {
        // get return_type, check if variable or function was defined
    }
    else if(isOperator(node->left->token)) {
        left_type = checkExpr(node->left);
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    // right node
    if(node->right->type == ID) {
        // get return type
    }
    else if(isOperator(node->right->token)) {
        right_type = checkExpr(node->right);
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    // create i2f conversion node for left operand
    if(left_type == T_INT_RET && right_type == T_FLOAT_RET) {
        ASTNode* temp = node->left;
        Token* new_token = (Token*)malloc(sizeof(Token));
        if (new_token == NULL){ 
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "Memory allocation failed"); 
        }
        new_token->type = T_I2F;
        // create node for I2F
        ASTNode* new_node = nodeCreate(I2F, new_token);
        new_node->right = temp;
        node->left = new_node;
        return T_FLOAT_RET;
    }
    // create i2f conversion node for right operand
    else if(right_type == T_INT_RET && left_type == T_FLOAT_RET) {
        ASTNode* temp = node->right;
        Token* new_token = (Token*)malloc(sizeof(Token));
        if (new_token == NULL){ 
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "Memory allocation failed"); 
        }
        new_token->type = T_I2F;
        // create node for I2F
        ASTNode* new_node = nodeCreate(I2F, new_token);
        new_node->right = temp;
        node->right = new_node;
        return T_FLOAT_RET;
    }
    else if(left_type != right_type) {
        return T_ERROR_RET;
    }
    else {
        // only int or float values are allowed in arithmetic types
        if(left_type == T_INT_RET && right_type == T_INT_RET) {
            return T_INT_RET;
        }
        else if(left_type == T_FLOAT_RET && right_type == T_FLOAT_RET) {
            return T_FLOAT_RET;
        }
        else {
            return T_ERROR_RET;
        }
    }
}

ret_type checkUnType(ASTNode* node) {
    if(isOperator(node->right->token)) {
        return (checkExpr(node->right) == T_BOOL_RET) ? T_BOOL_RET : T_ERROR_RET;
    }
    // ret_type of node
    ret_type node_type;
    if(node->left->type == ID) {
        // get return_type
        // node_type = ...
    }
    else {
        node_type = convertToRetType(node->type);
    }
    return (node_type == T_BOOL_RET) ? T_BOOL_RET : T_ERROR_RET;
            
}

ret_type checkTernTypes(ASTNode* node) {
    if(node == NULL) {
        return T_ERROR_RET;
    }
    if(node->type != P_EXPRESSION) {
        return T_ERROR_RET;
    }
    if(checkExpr(node->right) != T_BOOL_RET) {
        return T_ERROR_RET;
    }
    ret_type if_type = checkTernTypes(node->left);
    ret_type else_type = checkTernTypes(node->right);
}

ret_type checkBool(ASTNode* node) {
    ret_type left_type;
    ret_type right_type;
    if(node->left->type == ID) {
        // get return_type
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    if(node->right->type == ID) {
        // get return type
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    // checks if left node is an operator
    if(isOperator(node->left->token)) {
        if(checkExpr(node->left) == T_BOOL_RET) {
            left_type = T_BOOL_RET;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type in boolean operation (left operand)!\n");
        }
    }
    // checks if right node is an operator
    if(isOperator(node->right->token)) {
        if(checkExpr(node->right) == T_BOOL_RET) {
            right_type = T_BOOL_RET;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type in boolean operation (right operand)!\n");
        }
    }
    if(left_type != T_BOOL_RET || right_type != T_BOOL_RET) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(7, "ERROR: Wrong type in boolean operation!\n");
    }
}

ret_type checkRel(ASTNode* node) {
    // TODO
}

bool isRel(ASTNodeType type) {
    switch(type) {
        case EQ:
        case NEQ:
        case LESS:
        case LEQ:
        case MORE:
        case MEQ:
            return true;
        default:
            return false;
    }
}

ret_type convertToRetType(ASTNodeType node_type) {
    switch(node_type) {
        case TYPE_INT:
            return T_INT_RET;
        case TYPE_F64:
            return T_FLOAT_RET;
        case T_KW_BOOL:
            return T_BOOL_RET;
        case TYPE_STR:
            return T_STR_RET;
        default:
            return T_ERROR_RET;
    }
}