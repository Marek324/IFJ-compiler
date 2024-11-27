#include <math.h>

#include "sem_anal.h"
#include "ast.h"
#include "error.h"
#include "exp_parser.h"

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
void print_AVL(symtable_node_ptr node) {
    if (node == NULL) {
        return;
    }
    print_AVL(node->left);
    print_AVL(node->right);
    fprintf(stderr,"%s,", node->key);
    /*printf("%d", node->entry->type);
    printf("%i", node->entry->isNullable);
    printf("%d", node->entry->param_nullable[0]);
    printf("%d", node->entry->param_types[0]);
    printf("%d", node->entry->param_nullable[1]);
    printf("%d", node->entry->param_types[1]);
    printf("%d", node->entry->param_nullable[5]);
    printf("%d", node->entry->param_types[5]);*/
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
            nextStatement = nextStatement->left;
            break;

        case P_IF_STATEMENT:
            symIfStatement(node, tree);
            break;

        case P_WHILE_LOOP:
            symWhileLoop(node, tree, NULL);
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
    symEnd(tree);
}
void symEnd(symtable_tree_ptr tree) {
    symtable_dispose(tree);
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
    key->entry->hasExplicitType = false;
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
        if (key->entry->type == checkExpr(node->right,*tree)) {
            return;
        } 
        
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);        
        error_exit(7, "ERROR: assigning wrong type\n");
    }
    else {
        key->entry->type = checkExpr(node->right, *tree);
        if (key->entry->type == T_NULL_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);        
            error_exit(8, "ERROR: cannot get type\n");
        }
        return;
    }
    
}

void symIdStatement(ASTNode* node, symtable_tree_ptr tree){
    ASTNode *id = node; // save id 
    symtable_node_ptr key = NULL;
    bool ifjFound = false;
    if (strcmp(node->token->value.string_value, "ifj") == 0){
        ifjFound = true;
    }
    node = node->left->right; // LPAREN or ID or P_WHILE_LOOP or P_ASGN_FOUND
    if (node->type == LPAREN) {
        checkIfIdExits(SymFunctionTree, id->token->value.string_value);
        fprintf(stderr,"%s",id->token->value.string_value);
        key = symtable_search(SymFunctionTree, id->token->value.string_value);
        node = node->left; //P_EXPRESSION_LIST
        if (node->right != NULL) {
            node = node->right; //P_EXPRESSION

            for (int i = 0; i < key->entry->param_count; i++) {
                if (!(key->entry->param_types[i] == checkExpr(node->right,*tree))) {
                    freeAST(ASTRoot);
                    symtable_dispose(&SymFunctionTree);
                    error_exit(7, "ERROR: assigning wrong type\n");
                }

            }
        }
    }
    else if (node->type == ID) {
        
    }
    else if (node->type == P_WHILE_LOOP){
        symWhileLoop(node, tree, id);
    }
    else if (node->type == P_ASGN_FOUND) {
        checkIfIdExits(*tree, id->token->value.string_value);
        key = symtable_search(*tree, id->token->value.string_value);
        node = node->right; // P_EXPRESSION
        if (!(key->entry->type == checkExpr(node->right, *tree))) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);        
            error_exit(7, "ERROR: assigning wrong type\n");
        }
    }   
}

void symIfStatement(ASTNode* node, symtable_tree_ptr tree){

}

void symWhileLoop(ASTNode* node, symtable_tree_ptr tree, ASTNode* id){
    
}

void symReturnStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symBreakStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symContinueStatement(ASTNode* node, symtable_tree_ptr tree){
    
}

void symForLoop(ASTNode* node, symtable_tree_ptr tree){
    
}

ret_type checkExpr(ASTNode* node, symtable_node_ptr local_table) {
    if(node == NULL) {
        return T_ERROR_RET;
    }
    if(isOperator(node->token)) {
        ret_type node_type;
        // unary boolean operation
        if(node->type == BANG) {
            if(checkUnType(node, local_table) == T_BOOL_RET) {
                return T_BOOL_RET;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for unary boolean operation!\n");
            }
        }
        // binary boolean operations
        if(node->type == T_AND || node->type == T_OR) {
            if(checkBool(node, local_table) == T_BOOL_RET) {
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
            if(checkRel(node, local_table) == T_BOOL_RET) {
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
            ret_type node_type = checkDiv(node, local_table);
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
        node_type = checkAritTypes(node, local_table);
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
        // functions, IDs and literals
        if(node->type == T_NULL) {
            return T_NULL_RET;
        }
        if(node->type == ID) {
            symtable_node_ptr sym_node = symtable_search(local_table, node->token->value.string_value);
            if(sym_node == NULL) {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(3, "ERROR: Undefined identifier!\n");
            }
            // variables
            if(sym_node->entry->entry_type == T_VAR_SYM) {
                sym_node->entry->isUsed = true;
                return sym_node->entry->type;
            }
            // functions
            else if(sym_node->entry->entry_type == T_FUN_SYM) {
                sym_node->entry->isUsed = true;
                /*TODO: go through all parameters and check if the datatypes and count is good*/
                return sym_node->entry->type;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(10, "ERROR: Uknown symtable return type!\n");
            }
        }
        return convertToRetType(node->type);
    }
    else {
        // ternary operation
        if(node->type == T_IF) {
            ret_type node_type = checkTernTypes(node->left, local_table);
            if(node_type == T_ERROR_RET) {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong types for ternary operation!\n");
            }
            return node_type;
        }
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

    if (i >= capacity - 1) {
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

ret_type checkDiv(ASTNode* node, symtable_node_ptr local_table) { 
    ret_type left_type;
    ret_type right_type;
    symtable_node_ptr left_id;
    symtable_node_ptr right_id;
    // left node is ID
    if(node->left->type == ID) {
        // get return_type, check if variable or function was defined
        left_id = symtable_search(local_table, node->token->value.string_value);
        if(left_id == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(left_id->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function in arithmetic operation!\n");
        }
        left_type = left_id->entry->type;
        if(left_id->entry->entry_type == T_VAR_SYM) {
            left_id->entry->isUsed = true;
        }
        else if(left_id->entry->entry_type == T_FUN_SYM) {
            left_id->entry->isUsed = true;
            /*TODO: go through all parameters and check if the datatypes and count is good*/
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown state!\n");
        }
    }
    else if(isOperator(node->left->token)) {
        left_type = checkExpr(node->left, local_table);
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    // right node is ID
    if(node->right->type == ID) {
        // get return_type, check if variable or function was defined
        right_id = symtable_search(local_table, node->token->value.string_value);
        if(right_id == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(right_id->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function in arithmetic operation!\n");
        }
        right_type = right_id->entry->type;
        if(right_id->entry->entry_type == T_VAR_SYM) {
            right_id->entry->isUsed = true;
        }
        else if(right_id->entry->entry_type == T_FUN_SYM) {
            right_id->entry->isUsed = true;
            /*TODO: go through all parameters and check if the datatypes and count is good*/
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown state!\n");
        }
    }
    else if(isOperator(node->right->token)) {
        right_type = checkExpr(node->right, local_table);
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    // type control
    if(left_type == T_INT_RET && right_type == T_INT_RET) {
        node->token->type = T_IDIV;
        node->type = IDIV;
        return T_INT_RET;
    }
    else if(left_type == T_FLOAT_RET && right_type == T_FLOAT_RET) {
        node->token->type = T_DIV;
        node->type = DIV;
        return T_FLOAT_RET;
    }
    else if((left_type == T_INT_RET || left_type == T_FLOAT_RET) && (right_type == T_INT_RET || right_type == T_FLOAT_RET)) {
        // f64 to i32 if the decimal part is 0 (only literals and constants)
        // if()
         
    }
    else {
        return T_ERROR_RET;
    }
}

ret_type checkAritTypes(ASTNode* node, symtable_node_ptr local_table) {
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
        left_type = checkExpr(node->left, local_table);
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    // right node
    if(node->right->type == ID) {
        // get return type
    }
    else if(isOperator(node->right->token)) {
        right_type = checkExpr(node->right, local_table);
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    // create i2f conversion node for left operand
    if(left_type == T_INT_RET && right_type == T_FLOAT_RET) {
        if(1==1/*get bool isConst, variables cannot be converted*/) {
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
    }
    // create i2f conversion node for right operand
    else if(right_type == T_INT_RET && left_type == T_FLOAT_RET) {
        if(1==1/*get bool isConst, variables cannot be converted*/) {
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
    }
    if(left_type != right_type) {
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

ret_type checkUnType(ASTNode* node, symtable_node_ptr local_table) {
    if(isOperator(node->right->token)) {
        return (checkExpr(node->right, local_table) == T_BOOL_RET) ? T_BOOL_RET : T_ERROR_RET;
    }
    // ret_type of node
    ret_type node_type;
    if(node->left->type == ID) {
        // get return_type
        symtable_node_ptr sym_node = symtable_search(local_table, node->token->value.string_value);
        if(sym_node == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined identifier!\n");
        }
        if(sym_node->entry->entry_type == T_VAR_SYM) {
            sym_node->entry->isUsed = true;
            return sym_node->entry->type;
        }
        else if(sym_node->entry->entry_type == T_FUN_SYM) {
            sym_node->entry->isUsed = true;
            /*TODO: go through all parameters and check if the datatypes and count is good*/
            return sym_node->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else {
        node_type = convertToRetType(node->type);
    }
    return (node_type == T_BOOL_RET) ? T_BOOL_RET : T_ERROR_RET;
            
}

ret_type checkTernTypes(ASTNode* node, symtable_node_ptr local_table) {
    if(node == NULL) {
        return T_ERROR_RET;
    }
    if(node->type != P_EXPRESSION) {
        return T_ERROR_RET;
    }
    if(checkExpr(node->right, local_table) != T_BOOL_RET) {
        return T_ERROR_RET;
    }
    ret_type if_type = checkExpr(node->left->right, local_table);
    ret_type else_type = checkExpr(node->left->left->right, local_table);
    if(if_type != else_type) {
        return T_ERROR_RET;
    }
    else {
        return if_type;
    }
}

ret_type checkBool(ASTNode* node, symtable_node_ptr local_table) {
    ret_type left_type;
    ret_type right_type;
    if(node->left->type == ID) {
        symtable_node_ptr sym_node = symtable_search(local_table, node->token->value.string_value);
        if(sym_node == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined identifier!\n");
        }
        if(sym_node->entry->entry_type == T_VAR_SYM) {
            sym_node->entry->isUsed = true;
            left_type = sym_node->entry->type;
        }
        else if(sym_node->entry->entry_type == T_FUN_SYM) {
            sym_node->entry->isUsed = true;
            /*TODO: go through all parameters and check if the datatypes and count is good*/
            left_type = sym_node->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    if(node->right->type == ID) {
        symtable_node_ptr sym_node = symtable_search(local_table, node->token->value.string_value);
        if(sym_node == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined identifier!\n");
        }
        if(sym_node->entry->entry_type == T_VAR_SYM) {
            sym_node->entry->isUsed = true;
            right_type = sym_node->entry->type;
        }
        else if(sym_node->entry->entry_type == T_FUN_SYM) {
            sym_node->entry->isUsed = true;
            /*TODO: go through all parameters and check if the datatypes and count is good*/
            right_type = sym_node->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    // checks if left node is an operator
    if(isOperator(node->left->token)) {
        if(checkExpr(node->left, local_table) == T_BOOL_RET) {
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
        if(checkExpr(node->right, local_table) == T_BOOL_RET) {
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
    else {
        return T_BOOL_RET;
    }
}

ret_type checkRel(ASTNode* node, symtable_node_ptr local_table) {
    if(node->type) {
    // TODO
    }
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
void insertBuiltInFun() {
    symtable_node_ptr key = NULL;
    // ifj.readstr
    symtable_insert(&SymFunctionTree, "ifj.readstr", T_FUN_SYM);
    key = symtable_search(SymFunctionTree, "ifj.readstr");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry = {
        .entry_type = T_FUN_SYM,
        .type = T_STR_RET,
        .isUsed = true,
        .isNullable = true,
        .param_count = 0,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    *key->entry = entry;
    // ifj.readi32
    symtable_insert(&SymFunctionTree,"ifj.readi32",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.readi32");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry2 = {
        .entry_type = T_FUN_SYM,
        .type = T_INT_RET,
        .isUsed = true,
        .isNullable = true,
        .param_count = 0,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    *key->entry = entry2;
    // ifj.readi64
    symtable_insert(&SymFunctionTree,"ifj.readf64",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.readf64");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry3 = {
        .entry_type = T_FUN_SYM,
        .type = T_FLOAT_RET,
        .isUsed = true,
        .isNullable = true,
        .param_count = 0,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    *key->entry = entry3;
    // ifj.write
    symtable_insert(&SymFunctionTree,"ifj.write",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.write");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    //will need changes - term
    symtable_entry_t entry4 = {
        .entry_type = T_FUN_SYM,
        .type = T_NULL_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = false,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    *key->entry = entry4;
    // ifj.i2f
    symtable_insert(&SymFunctionTree,"ifj.i2f",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.i2f");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    //will need changes - term
    symtable_entry_t entry5 = {
        .entry_type = T_FUN_SYM,
        .type = T_FLOAT_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry5.param_nullable = malloc(sizeof(bool));
    if (entry5.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry5.param_nullable[0] = false;

    entry5.param_types = malloc(sizeof(ret_type));
    if (entry5.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry5.param_types[0] = T_FLOAT_RET;
    *key->entry = entry5;
    // ifj.f2i
    symtable_insert(&SymFunctionTree,"ifj.f2i",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.f2i");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    //will need changes - term
    symtable_entry_t entry6 = {
        .entry_type = T_FUN_SYM,
        .type = T_INT_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry6.param_nullable = malloc(sizeof(bool));
    if (entry6.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry6.param_nullable[0] = false;

    entry6.param_types = malloc(sizeof(ret_type));
    if (entry6.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry6.param_types[0] = T_INT_RET;
    *key->entry = entry6;
    // ifj.string
    symtable_insert(&SymFunctionTree,"ifj.string",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.string");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry7 = {
        .entry_type = T_FUN_SYM,
        .type = T_STR_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry7.param_nullable = malloc(sizeof(bool));
    if (entry7.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry7.param_nullable[0] = false;

    entry7.param_types = malloc(sizeof(ret_type));
    if (entry7.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry7.param_types[0] = T_STR_RET;
    *key->entry = entry7;
    // ifj.length
    symtable_insert(&SymFunctionTree,"ifj.length",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.length");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry8 = {
        .entry_type = T_FUN_SYM,
        .type = T_INT_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry8.param_nullable = malloc(sizeof(bool));
    if (entry8.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry8.param_nullable[0] = false;

    entry8.param_types = malloc(sizeof(ret_type));
    if (entry8.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry8.param_types[0] = T_STR_RET;
    *key->entry = entry8;
    // ifj.concat
    symtable_insert(&SymFunctionTree,"ifj.concat",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.concat");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry9 = {
        .entry_type = T_FUN_SYM,
        .type = T_STR_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 2,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry9.param_nullable = malloc(2 * sizeof(bool));
    if (entry9.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry9.param_nullable[0] = false;
    entry9.param_nullable[1] = false;

    entry9.param_types = malloc(2 * sizeof(ret_type));
    if (entry9.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry9.param_types[0] = T_STR_RET;
    entry9.param_types[1] = T_STR_RET;
    *key->entry = entry9;
    // ifj.substring
    symtable_insert(&SymFunctionTree,"ifj.substring",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.substring");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry10 = {
        .entry_type = T_FUN_SYM,
        .type = T_STR_RET,
        .isUsed = true,
        .isNullable = true,
        .param_count = 3,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry10.param_nullable = malloc(3 * sizeof(bool));
    if (entry10.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry10.param_nullable[0] = false;
    entry10.param_nullable[1] = false;
    entry10.param_nullable[2] = false;

    entry10.param_types = malloc(3 * sizeof(ret_type));
    if (entry10.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry10.param_types[0] = T_STR_RET;
    entry10.param_types[1] = T_INT_RET;
    entry10.param_types[2] = T_INT_RET;
    *key->entry = entry10;
    // ifj.strcmp
    symtable_insert(&SymFunctionTree,"ifj.strcmp",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.strcmp");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry11 = {
        .entry_type = T_FUN_SYM,
        .type = T_INT_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 2,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry11.param_nullable = malloc(2 * sizeof(bool));
    if (entry11.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry11.param_nullable[0] = false;
    entry11.param_nullable[1] = false;

    entry11.param_types = malloc(2 * sizeof(ret_type));
    if (entry11.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry11.param_types[0] = T_STR_RET;
    entry11.param_types[1] = T_STR_RET;
    *key->entry = entry11;
    // ifj.ord
    symtable_insert(&SymFunctionTree,"ifj.ord",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.ord");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry12 = {
        .entry_type = T_FUN_SYM,
        .type = T_INT_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 2,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry12.param_nullable = malloc(2 * sizeof(bool));
    if (entry12.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry12.param_nullable[0] = false;
    entry12.param_nullable[1] = false;

    entry12.param_types = malloc(2 * sizeof(ret_type));
    if (entry12.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry12.param_types[0] = T_STR_RET;
    entry12.param_types[1] = T_INT_RET;
    *key->entry = entry12;
    // ifj.chr
    symtable_insert(&SymFunctionTree,"ifj.chr",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.chr");
    free(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry13 = {
        .entry_type = T_FUN_SYM,
        .type = T_STR_RET,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = true,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry13.param_nullable = malloc(1 * sizeof(bool));
    if (entry13.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry13.param_nullable[0] = false;

    entry13.param_types = malloc(1 * sizeof(ret_type));
    if (entry13.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry13.param_types[0] = T_INT_RET;

    *key->entry = entry13;
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

bool floatIsInt(float value) {
    // Tolerance for floating-point precision
    const float epsilon = 1e-6;
    return (fabs((value - (int)value)) < epsilon) ? true : false;
}