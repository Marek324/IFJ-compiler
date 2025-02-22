/**
 * file: sem_anal.c
 * author: Adam Veselý xvesela00
 * author: Mikuláš Lešiga xlesigm00
 * Implementation of the semantic analyzer.
 */
#include <math.h>

#include "sem_anal.h"
#include "ast.h"
#include "error.h"
#include "exp_parser.h"
#include "stack_utils.h"

stack_t *SCOPEStack = NULL;

int scope = 0;
void print_AVL(symtable_node_ptr node) {
    if (node == NULL) {
        return;
    }
    print_AVL(node->left);
    print_AVL(node->right);
    fprintf(stderr,"%s : %i,", node->key, node->entry->scopeLevel);
    //fprintf(stderr,"%d,", node->entry->isChanged);
    //fprintf(stderr,"%d,", node->entry->isUsed);
    /*printf("%d", node->entry->type);
    printf("%i", node->entry->isNullable);
    printf("%d", node->entry->param_nullable[0]);
    printf("%d", node->entry->param_types[0]);
    printf("%d", node->entry->param_nullable[1]);
    printf("%d", node->entry->param_types[1]);
    printf("%d", node->entry->param_nullable[5]);
    printf("%d", node->entry->param_types[5]);*/
}
void findReturn(ASTNode *root) {
    root = root->right->left; // skip prolog
    while (root != NULL) {
        root = root->right; // ID
        symtable_node_ptr function = symtable_search(SymFunctionTree, root->token->value.string_value);
        root = root->left->left; //RPAREN or P_PARAM_LIST
        if (root->type == P_PARAM_LIST) {
            root = root->left; // RPAREN
        }
        root = root->left->left; // P_BLOCK

        ASTNode *lookForReturn = root->right;
        while (lookForReturn != NULL) {
            if (lookForReturn->right != NULL) {
                lookForReturn = lookForReturn->right;
            }
            if (lookForReturn->type == P_RETURN_STATEMENT) {
                function->entry->hasReturn = true;
            }
            else if (lookForReturn->type == ID) {
                lookForReturn = lookForReturn->left->left;
            }
            else {
                lookForReturn = lookForReturn->left;
            }
        }
        root = root->left;
    }
}

void analyse (ASTNode* root) {
    checkForMain();
    insertBuiltInFun();
    findReturn(root);
    root = root->right->left; // skip prolog
    symFuncDef(root);
}
void checkForReturn(symtable_node_ptr function) {
    if (function->entry->returnsValue == true && function->entry->hasReturn != true) {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(6, "ERROR: function doesnt return anything!\n");
    }
}
void symFuncDef(ASTNode* node){
    if (node == NULL || node->type == END_OF_FILE) return;
    // function Sym node 
    node = node->right; // ID
    symtable_node_ptr function = symtable_search(SymFunctionTree, node->token->value.string_value);
    symtable_node_ptr local_table = *function->entry->local_symtable;
    
    node = node->left->left; // params or RPAREN
    if (node->type == P_PARAM_LIST){

        symParamList(node, &local_table);
        
        node = node->left; // RPAREN
    }

    node = node->left->left; // block

    symBlock(node, &local_table, NULL, T_ANY, NULL, function);

    symFuncDef(node->left);
    checkForReturn(function);
    symtable_dispose(&local_table);
}
void symBlock(ASTNode* node, symtable_tree_ptr local_table, ASTNode* optionalValue, ret_type type, ASTNode* whileId, symtable_node_ptr function) {
    if (local_table != NULL) {
        scope++;
        stackPush(SCOPEStack, (long)(*local_table));
        *local_table = stackUtilCopy(*local_table);
    }
    if (optionalValue != NULL) {
        symtable_insert(local_table, optionalValue->right->token->value.string_value, T_VAR_SYM);
        symtable_node_ptr key = symtable_search(*local_table, optionalValue->right->token->value.string_value);
        key->entry->type = type;
        key->entry->isConst = true;
        key->entry->isNullable = false;
        key->entry->scopeLevel = scope;
    }
    if (whileId != NULL) {
        symtable_insert(local_table, whileId->token->value.string_value, T_VAR_SYM);
        symtable_node_ptr key = symtable_search(*local_table, whileId->token->value.string_value);
        key->entry->type = T_WHILE_RET;
        key->entry->isConst = true;
        key->entry->scopeLevel = scope-1;
    }
    symStatement(node->right, local_table, function);
    checkVarsAndConsts(*local_table);

    if (!stackIsEmpty(SCOPEStack)) {
        symtable_node_ptr old_table = *local_table; // Store old table for cleanup
        *local_table = stackUtilPop(SCOPEStack); //POP FROM STACK
        updateTableBySameKey(old_table, local_table);
        scope--;
        symtable_dispose(&old_table); // Free the previous scope's memory
    }
}

void checkVarsAndConsts(symtable_node_ptr local_table) {
    if (local_table == NULL) return;

    checkVarsAndConsts(local_table->right);
    checkVarsAndConsts(local_table->left);
    if (scope == local_table->entry->scopeLevel) {
        if (local_table->entry->isConst == true) {
            if (local_table->entry->isUsed != true) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);        
                error_exit(9, "ERROR: %s constant not used\n",local_table->key);
            }
        }
        else {
            if (local_table->entry->isUsed != true || local_table->entry->isChanged != true) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(9, "ERROR: %s variable either not used or not modified\n", local_table->key);
                symtable_dispose(&SymFunctionTree);
            }
        }
    }
}
void symStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function) {
    if (node == NULL)
        return;

    node = (node->right != NULL) ? node->right : node;

    ASTNode *nextStatement = node->left;
    switch(node->type){

        case P_VAR_DECLARATION:
            symVarDec(node, local_table);
            break;

        case ID:
            symIdStatement(node, local_table, function);
            nextStatement = nextStatement->left;
            break;

        case P_IF_STATEMENT:
            symIfStatement(node, local_table, function);
            break;

        case P_WHILE_LOOP:
            symWhileLoop(node, local_table, NULL, function);
            break;
        
        case P_RETURN_STATEMENT:
            symReturnStatement(node, local_table, function);
            break;

        case P_BREAK:
            symBreakStatement(node, local_table);
            break;

        case P_CONTINUE:
            symContinueStatement(node, local_table);
            break;

        case P_FOR_LOOP:
            symForLoop(node, local_table, function);
            break;

        default:
            nextStatement = NULL;
            break;
    }

    symStatement(nextStatement, local_table, function);
    
}

void symParamList(ASTNode* node, symtable_tree_ptr local_table) {
    if (node == NULL) return;
    
    ASTNode *id = node->right; // save ID
    symtable_insert(local_table, id->token->value.string_value, T_VAR_SYM);
    node = id->left; // move to P_TYPE_COMPLETE

    symtable_node_ptr key = symtable_search(*local_table, id->token->value.string_value);
        key->entry->isChanged = false;
        key->entry->hasExplicitType = true;
        key->entry->isUsed = false;
        key->entry->scopeLevel = 1;
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
        symParamList(node->right, local_table);
    }
}

void symVarDec(ASTNode* node, symtable_tree_ptr local_table){
    node = node->right; // const or var
    bool isconst = node->token->value.keyword == KW_CONST ? true : false;
    node = node->left; // ID
    if (strcmp("_", node->token->value.string_value) == 0) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(10, "ERROR: _ is not a valid ID name!\n");
    }
    symtable_insert(local_table, node->token->value.string_value, T_VAR_SYM);
    symtable_node_ptr key = symtable_search(*local_table, node->token->value.string_value);
    node = node->left; // ASSGN OR P_TYPE_COMPLETE
    key->entry->isConst = isconst;
    key->entry->isUsed = false;
    key->entry->isChanged = false;
    key->entry->hasExplicitType = false;
    key->entry->isNullable = false;
    key->entry->scopeLevel = scope;
    fprintf(stderr,"V:%s = %i:%i\n",key->key, key->entry->scopeLevel,scope);
    ASTNode *Jozef = node->right;
    if (node->type == P_TYPE_COMPLETE) {
        key->entry->hasExplicitType = true;
        if(Jozef->type == P_TYPE) {
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

    // variable/constant or literal, sets isConvertable flag or isNull flag (variable is null)
    if(node->right != NULL) {
        if(node->right->type == TYPE_F64) {
            if(floatIsInt(node->right->token->value.float_value)) {
                key->entry->isConvertable = true;
            }
        }
        else if(node->right->type == ID) {
            // only variables/constants
            if(node->right->left == NULL) {
               symtable_node_ptr expressionID = symtable_search(*local_table, node->right->token->value.string_value); 
               if(expressionID != NULL) {
                    key->entry->isConvertable = expressionID->entry->isConvertable;
                    if(expressionID->entry->isNull) {
                        key->entry->isNull = true;
                    }
                }               
            }
        }
        else if(node->right->type == T_NULL) {
            key->entry->isNull = true;
        }
    }
    // end of isConvertable/isNull flag setting

    symtable_node_ptr expressionID = NULL;
    if (key->entry->hasExplicitType == true) {
        ret_type type = checkExpr(node->right,*local_table);
        if (type != T_NULL_RET) {
            if (key->entry->type == type) {
                if (node->right->type == ID) {
                    // functions
                    if(node->right->left != NULL) {
                        if(node->right->left->type == P_EXPRESSION_LIST) {
                            // built-in functions
                            if(node->right->right != NULL) {
                                if(node->right->right->type == ID) {
                                    // if the first ID isn't "ifj"
                                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Missing definition!\n");
                                    }
                                    // if the 2nd part of the built-in function is too long
                                    if (strlen(node->right->right->token->value.string_value) > 9) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Missing definition!\n");
                                    }
                                    char builtinFun[14] = "ifj.";
                                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                                    expressionID = symtable_search(SymFunctionTree, builtinFun);
                                    if(expressionID == NULL) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Undefined identifier!\n");
                                    }
                                    checkArguments(local_table, node->right->left, expressionID);
                                }
                            }
                            // normal functions
                            else {
                                expressionID = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                                if(expressionID == NULL) {
                                    symtable_dispose(&SymFunctionTree);
                                    freeAST(ASTRoot);
                                    error_exit(3, "ERROR: Undefined identifier!\n");
                                }
                                checkArguments(local_table, node->right->left, expressionID);
                            }
                        }
                    }
                    // variables
                    else {
                        expressionID = symtable_search(*local_table, node->right->token->value.string_value);
                    }
                    if (expressionID->entry->isNullable && !key->entry->isNullable) {
                        freeAST(ASTRoot);
                        symtable_dispose(&SymFunctionTree);        
                        error_exit(7, "ERROR: assigning wrong type (symVarDec1)\n");
                    }
                }
                return;
            } 
        }
        else {
            if (key->entry->isNullable == true){
                return;
            } 
        }
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);        
        error_exit(7, "ERROR: assigning wrong type (symVarDec2)\n");
    }
    else {
        key->entry->type = checkExpr(node->right, *local_table);
        if (node->right->type == ID) {
            // functions
            if(node->right->left != NULL) {
                if(node->right->left->type == P_EXPRESSION_LIST) {
                    // built-in functions
                    if(node->right->right != NULL) {
                        if(node->right->right->type == ID) {
                            // if the first ID isn't "ifj"
                            if(strcmp(node->right->token->value.string_value, "ifj")) {
                                symtable_dispose(&SymFunctionTree);
                                freeAST(ASTRoot);
                                error_exit(3, "ERROR: Missing definition!\n");
                            }
                            // if the 2nd part of the built-in function is too long
                            if (strlen(node->right->right->token->value.string_value) > 9) {
                                symtable_dispose(&SymFunctionTree);
                                freeAST(ASTRoot);
                                error_exit(3, "ERROR: Missing definition!\n");
                            }
                            char builtinFun[14] = "ifj.";
                            strcpy(builtinFun+4, node->right->right->token->value.string_value);
                            expressionID = symtable_search(SymFunctionTree, builtinFun);
                            if(expressionID == NULL) {
                                symtable_dispose(&SymFunctionTree);
                                freeAST(ASTRoot);
                                error_exit(3, "ERROR: Undefined identifier!\n");
                            }
                            checkArguments(local_table, node->right->left, expressionID);
                        }
                    }
                    // normal functions
                    else {
                        expressionID = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                        if(expressionID == NULL) {
                            symtable_dispose(&SymFunctionTree);
                            freeAST(ASTRoot);
                            error_exit(3, "ERROR: Undefined identifier!\n");
                        }
                        checkArguments(local_table, node->right->left, expressionID);
                    }
                }
            }
            // variables
            else {
                expressionID = symtable_search(*local_table, node->right->token->value.string_value);
            }
            key->entry->isNullable = expressionID->entry->isNullable;
        }
        if (key->entry->type == T_NULL_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);        
            error_exit(8, "ERROR: cannot get type\n");
        }
        return;
    }
    // sets isNullable if first operand in orelse is NULL and 2nd one is also NULL/NULLABLE
    // probably won't work for variables containing null as the first operand
    if(node->right != NULL) {
        // orelse
        if(node->right->type == T_ORELSE) {
            if(node->right->left != NULL) {
                // left operand is null
                if(node->right->left->type == T_NULL) {
                    if(node->right->right != NULL) {
                        if(expressionID != NULL) {
                            if(node->right->right->type == T_NULL || expressionID->entry->isNullable) {
                                key->entry->isNullable = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void symIdStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function){
    ASTNode *id = node; // save id 
    symtable_node_ptr key = NULL;
    bool ifjFound = false;
    if (strcmp(node->token->value.string_value, "ifj") == 0){
        ifjFound = true;
    }
    node = node->left->right; // LPAREN or ID or P_WHILE_LOOP or P_ASGN_FOUND
    if (node->type == LPAREN) {
        fprintf(stderr,"FUN\n");
        checkIfIdExits(SymFunctionTree, id->token->value.string_value);
        key = symtable_search(SymFunctionTree, id->token->value.string_value);
        key->entry->isUsed = true;
        if (key->entry->returnsValue) {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(4, "ERROR: Function returning value has to be assigned!\n");

        }
        node = node->left; // P_EXPRESSION_LIST
        checkArguments(local_table, node, key);
    }
    else if (node->type == ID) {
        if (ifjFound) {
           fprintf(stderr,"BUILTIN FUN\n");
            if (strlen(node->token->value.string_value) > 9) {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(3, "ERROR: Missing definition!\n");
            }
            char builtinFun[14] = "ifj.";
            strcpy(builtinFun+4, node->token->value.string_value);
            checkIfIdExits(SymFunctionTree, builtinFun);
            key = symtable_search(SymFunctionTree, builtinFun);
            key->entry->isUsed = true;
            if (key->entry->returnsValue) {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(4, "ERROR: Function returning value has to be assigned!\n");
            }
            node = node->left->left; // P_EXPRESSION_LIST 
            checkArguments(local_table, node, key);
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Missing definition!\n");
        }
    }
    else if (node->type == P_WHILE_LOOP){
        symWhileLoop(node, local_table, id, function);
    }
    else if (node->type == P_ASGN_FOUND) {
        fprintf(stderr,"ASGN\n");
        if (!(strcmp(id->token->value.string_value, "_") == 0)) {
            checkIfIdExits(*local_table, id->token->value.string_value);
            key = symtable_search(*local_table, id->token->value.string_value);
            if (key->entry->isConst) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);        
                error_exit(5, "ERROR: assigning to CONST!\n");
            }
            key->entry->isChanged = true;
            key->entry->isUsed = true;
            node = node->right; // P_EXPRESSION
            ret_type type = checkExpr(node->right, *local_table);
            if (type != T_NULL_RET) {
                if (!(key->entry->type == type)) {
                    freeAST(ASTRoot);
                    symtable_dispose(&SymFunctionTree);        
                    error_exit(7, "ERROR: assigning wrong type (symIdStatement1)\n");
                }
            }
            else {
                if(!(key->entry->isNullable == true)) {
                    freeAST(ASTRoot);
                    symtable_dispose(&SymFunctionTree);        
                    error_exit(7, "ERROR: assigning wrong type (symIdStatement2)\n");
                }
            }
            // setting isNull
            if(node->right != NULL) {
                if(node->right->type == ID) {
                    // only variables
                    if(node->right->left == NULL) {
                        symtable_node_ptr temp_node = symtable_search(*local_table, node->right->token->value.string_value);
                        if(temp_node != NULL) {
                            if(temp_node->entry->isNull) {
                                key->entry->isNull = true;
                            }
                        }
                    }
                }
                else if(node->right->type == T_NULL) {
                    key->entry->isNull = true;
                }
            }
        }
        else {
            node = node->right; // P_EXPRESSION
            ret_type type = checkExpr(node->right, *local_table);
            (void) type;
        }
    }   
}

void checkArguments(symtable_tree_ptr tree, ASTNode* node, symtable_node_ptr key) {
    if (node->right != NULL) {
            node = node->right; // P_EXPRESSION
            ret_type type = T_NULL_RET;
            int i = 0;
            while (1) {
                if (i < key->entry->param_count && key->entry->param_types[i] == T_ANY) {
                    if (node->right->type == ID) {
                        symtable_node_ptr expressionID;
                        // functions
                        if(node->right->left != NULL) {
                            if(node->right->left->type == P_EXPRESSION_LIST) {
                                // built-in functions
                                if(node->right->right != NULL) {
                                    if(node->right->right->type == ID) {
                                        // if the first ID isn't "ifj"
                                        if(strcmp(node->right->token->value.string_value, "ifj")) {
                                            symtable_dispose(&SymFunctionTree);
                                            freeAST(ASTRoot);
                                            error_exit(3, "ERROR: Missing definition!\n");
                                        }
                                        // if the 2nd part of the built-in function is too long
                                        if (strlen(node->right->right->token->value.string_value) > 9) {
                                            symtable_dispose(&SymFunctionTree);
                                            freeAST(ASTRoot);
                                            error_exit(3, "ERROR: Missing definition!\n");
                                        }
                                        char builtinFun[14] = "ifj.";
                                        strcpy(builtinFun+4, node->right->right->token->value.string_value);
                                        expressionID = symtable_search(SymFunctionTree, builtinFun);
                                        if(expressionID == NULL) {
                                            symtable_dispose(&SymFunctionTree);
                                            freeAST(ASTRoot);
                                            error_exit(3, "ERROR: Undefined identifier!\n");
                                        }
                                        checkArguments(tree, node->right->left, expressionID);
                                    }
                                }
                                // normal functions
                                else {
                                    expressionID = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                                    if(expressionID == NULL) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Undefined identifier!\n");
                                    }
                                    checkArguments(tree, node->right->left, expressionID);
                                }
                            }
                        }
                        // variables
                        else {
                            expressionID = symtable_search(*tree, node->right->token->value.string_value);
                            if(expressionID == NULL) {
                                symtable_dispose(&SymFunctionTree);
                                freeAST(ASTRoot);
                                error_exit(3, "ERROR: Undefined identifier!\n");
                            }
                        }
                        expressionID->entry->isUsed = true;
                    }
                    goto skip;
                }
                type = checkExpr(node->right, *tree);
                if (node->right->type == ID) {
                    symtable_node_ptr expressionID;
                    // functions
                    if(node->right->left != NULL) {
                        if(node->right->left->type == P_EXPRESSION_LIST) {
                            // built-in functions
                            if(node->right->right != NULL) {
                                if(node->right->right->type == ID) {
                                    // if the first ID isn't "ifj"
                                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Missing definition!\n");
                                    }
                                    // if the 2nd part of the built-in function is too long
                                    if (strlen(node->right->right->token->value.string_value) > 9) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Missing definition!\n");
                                    }
                                    char builtinFun[14] = "ifj.";
                                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                                    expressionID = symtable_search(SymFunctionTree, builtinFun);
                                    if(expressionID == NULL) {
                                        symtable_dispose(&SymFunctionTree);
                                        freeAST(ASTRoot);
                                        error_exit(3, "ERROR: Undefined identifier!\n");
                                    }
                                    checkArguments(tree, node->right->left, expressionID);
                                }
                            }
                            // normal functions
                            else {
                                expressionID = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                                if(expressionID == NULL) {
                                    symtable_dispose(&SymFunctionTree);
                                    freeAST(ASTRoot);
                                    error_exit(3, "ERROR: Undefined identifier!\n");
                                }
                                checkArguments(tree, node->right->left, expressionID);
                            }
                        }
                    }
                    // variables
                    else {
                        expressionID = symtable_search(*tree, node->right->token->value.string_value);
                    }
                    expressionID->entry->isUsed = true;
                }

                if (type != T_NULL_RET) {
                        if (i < key->entry->param_count && !(key->entry->param_types[i] == type)) {
                            freeAST(ASTRoot);
                            symtable_dispose(&SymFunctionTree);
                            error_exit(4, "ERROR: assigning wrong type! (checkArguments)\n");
                        }
                }
                else {
                        if (i < key->entry->param_count && (key->entry->param_nullable[i] != true)) {
                            freeAST(ASTRoot);
                            symtable_dispose(&SymFunctionTree);
                            error_exit(4, "ERROR: assigning NULL!\n");
                        }
                } 
                skip: i++;
                node = node->left; // null or P_COMMA_EXPR_FOUND
                if ((node != NULL && node->left != NULL && node->left->right != NULL) && i <= key->entry->param_count + 1) {
                    node = node->left->right; // P_EXPRESSION
                }
                else {
                    break;
                }
            }
            
            if (i != key->entry->param_count) {
                    freeAST(ASTRoot);
                    symtable_dispose(&SymFunctionTree);
                    error_exit(4, "ERROR: wrong argument count!\n");
                }
        } 
        else {
            if (key->entry->param_count != 0) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(4, "ERROR: wrong argument count!\n");
            }
        }
}
        
void symIfStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function) {
    fprintf(stderr,"IF\n");
    node = node->right; // P_EXPRESSION
    ASTNode *expressionValue = node->right;
    ret_type type = checkExpr(expressionValue, *local_table);
    node = node->left->right; // P_BLOCK or P_OPTIONAL_VALUE
    if (node->type == P_BLOCK && type != T_BOOL_RET) {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(7, "ERROR: wrong condition!\n");
    }
    ASTNode *optionalValue = NULL;

    if (node->type == P_OPTIONAL_VALUE) {
        if (expressionValue->type == ID) {
            symtable_node_ptr conditionID = symtable_search(*local_table,expressionValue->token->value.string_value);
            if (conditionID->entry->isNullable == false) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(7, "ERROR: has to be nullable ID or Function returning null!\n");
            }
        }
        else {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(7, "ERROR: has to be nullable ID or Function returning null!\n");
        }
            optionalValue = node;
            node = node->left; // P_BLOCK
    }
    symBlock(node, local_table, optionalValue, type, NULL, function);

    if (node->left != NULL) {
        node = node->left; // P_ELSE_STATEMENT
        if(node->right->type == P_BLOCK) {
                node = node->right; //P_BLOCK
            }
        fprintf(stderr,"ELSE\n");
        symBlock(node, local_table, NULL, T_ANY, NULL, function);
    }
}

void symWhileLoop(ASTNode* node, symtable_tree_ptr local_table, ASTNode* id, symtable_node_ptr function){
    fprintf(stderr,"While\n");
    node = node->right; // P_EXPRESSION
    ASTNode *expressionValue = node->right;
    ret_type type = checkExpr(expressionValue, *local_table);
    node = node->left; // P_BLOCK or P_OPTIONAL_VALUE or P_OPTIONAL_STATEMENTS
    if (node->type == P_BLOCK && type != T_BOOL_RET) {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(7, "ERROR: wrong condition!\n");
    }
    if (node->type == P_OPTIONAL_STATEMENTS) {
        if (type != T_BOOL_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(7, "ERROR: wrong condition!\n");
        }
        ASTNode* optionalStatements = node;
        if(node->right->type == P_BLOCK) {
            optionalStatements = node->right;
        }
        symBlock(optionalStatements, local_table, NULL, T_ANY, id, function);
        node = node->left; // P_BLOCK
    }

    ASTNode *optionalValue = NULL;
    if (node->type == P_OPTIONAL_VALUE) {
        if (expressionValue->type == ID) {
            symtable_node_ptr conditionID = symtable_search(*local_table,expressionValue->token->value.string_value);
            if (conditionID->entry->isNullable == false) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(7, "ERROR: has to be nullable ID or Function returning null!\n");
            }
        }
        else {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(7, "ERROR: has to be nullable ID or Function returning null!\n");
        }
        optionalValue = node;
        node = node->left; // P_BLOCK or P_OPTIONAL_STATEMENTS
        if (node->type == P_OPTIONAL_STATEMENTS) {
            ASTNode* optionalStatements = node;
            if(node->right->type == P_BLOCK) {
                optionalStatements = node->right;
            }
            symBlock(optionalStatements, local_table, optionalValue, type, id, function);
            node = node->left; // P_BLOCK
        }
    }  
    symBlock(node, local_table, optionalValue, type, id, function);
    if (node->left != NULL) {
        node = node->left; // P_ELSE_STATEMENT
        if(node->right->type == P_BLOCK) {
            node = node->right; //P_BLOCK
        }
        symBlock(node, local_table, NULL, T_ANY, NULL, function);
    }
}

void symReturnStatement(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function){
    node = node->right; // P_EXPRESSION
    if (function->entry->type == T_NULL_RET && node->right != NULL) {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(6, "ERROR: has to be nullable ID or Function returning null!\n");
    }
    if (function->entry->type != T_NULL_RET) {
        if (node->right == NULL) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(6, "ERROR: has to be nullable ID or Function returning null!\n");
        }
        ret_type type = checkExpr(node->right, *local_table);
        if (type != T_NULL_RET) {
            if (function->entry->type != type) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(4, "ERROR: returning wrong type!\n");
            }
        }
        else {
            if (function->entry->isNullable != true) {
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(7, "ERROR: function cannot return NULL!\n");
            } 
        }
    } 
}

void symBreakStatement(ASTNode* node, symtable_tree_ptr local_table){
    if (node->right != NULL) {
        node = node->right; //ID
        checkIfIdExits(*local_table, node->token->value.string_value);
        symtable_node_ptr id = symtable_search(*local_table, node->token->value.string_value);
        id->entry->isUsed = true;
        if (id->entry->type != T_WHILE_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(4, "ERROR: wrong type of ID after break!\n");
        }
    }
}

void symContinueStatement(ASTNode* node, symtable_tree_ptr local_table){
    if (node->right != NULL) {
        node = node->right; //ID
        checkIfIdExits(*local_table, node->token->value.string_value);
        symtable_node_ptr id = symtable_search(*local_table, node->token->value.string_value);
        id->entry->isUsed = true;
        if (id->entry->type != T_WHILE_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(4, "ERROR: wrong type of ID after continue!\n");
        }
    }
}

void symForLoop(ASTNode* node, symtable_tree_ptr local_table, symtable_node_ptr function){
    fprintf(stderr,"FOR\n");
    node = node->right; // P_EXPRESSION
    ASTNode *expressionValue = node->right;
    ret_type type = checkExpr(expressionValue, *local_table);
    if (expressionValue->type == ID) {
        symtable_node_ptr conditionID = symtable_search(*local_table,expressionValue->token->value.string_value);
        if (conditionID->entry->isNullable == true || type != T_STR_RET) {
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(7, "ERROR: has to be u8 ID or Function returning u8 !\n");
        }
    }
    else {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(7, "ERROR: has to be u8 ID or Function returning u8!\n");
    }
    node = node->left; // P_OPTIONAL_VALUE
    ASTNode *optionalValue = node;
    
    node = node->left; // P_BLOCK
    if (strcmp(optionalValue->right->token->value.string_value,"_") != 0) {
        symBlock(node, local_table, optionalValue, T_INT_RET, NULL, function);
    }
    else {
        symBlock(node, local_table, NULL, T_ANY, NULL, function);
    }

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
            node_type = checkDiv(node, local_table);
            if(node_type == T_INT_RET || node_type == T_FLOAT_RET) {
                return node_type == T_INT_RET ? T_INT_RET : T_FLOAT_RET;
            }
            else {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for division operation!\n");
            }
        }
        if(node->type == T_ORELSE) {
            node_type = checkOrElse(node, local_table);
            if(node_type == T_ERROR_RET) {
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(7, "ERROR: Wrong type for binary orelse operation!\n");
            }
            return node_type;
        }
        // all other binary operations (arithmetic)
        node_type = checkAritTypes(node, local_table);
        if(node_type == T_INT_RET || node_type == T_FLOAT_RET) {
            return node_type == T_INT_RET ? T_INT_RET : T_FLOAT_RET;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type for binary arithmetic operation!\n");
        }
    }
    else if(isOperand(node->token)){
        // functions, IDs and literals
        if(node->type == T_NULL) {
            return T_NULL_RET;
        }
        if(node->type == AT_AS) {
            return checkAtAs(node, local_table);
        }
        if(node->type == ID) {
            symtable_node_ptr sym_node;
            if(node->left != NULL) {
                if(node->left->type == P_EXPRESSION_LIST) {
                    // built-in function
                    if(node->right != NULL) {
                        if(node->right->type != ID) {
                            return T_ERROR_RET;
                        }
                        // if the first ID isn't "ifj"
                        if(strcmp(node->token->value.string_value, "ifj")) {
                            symtable_dispose(&SymFunctionTree);
                            freeAST(ASTRoot);
                            error_exit(3, "ERROR: Missing definition!\n");
                        }
                        // if the 2nd part of the built-in function is too long
                        if (strlen(node->right->token->value.string_value) > 9) {
                            symtable_dispose(&SymFunctionTree);
                            freeAST(ASTRoot);
                            error_exit(3, "ERROR: Missing definition!\n");
                        }
                        char builtinFun[14] = "ifj.";
                        strcpy(builtinFun+4, node->right->token->value.string_value);
                        sym_node = symtable_search(SymFunctionTree, builtinFun);
                        if(sym_node == NULL) {
                            symtable_dispose(&SymFunctionTree);
                            freeAST(ASTRoot);
                            error_exit(3, "ERROR: Undefined identifier!\n");
                        }
                        checkArguments(&local_table, node->left, sym_node);
                    }
                    else {
                        sym_node = symtable_search(SymFunctionTree, node->token->value.string_value);
                        if(sym_node == NULL) {
                            symtable_dispose(&SymFunctionTree);
                            freeAST(ASTRoot);
                            error_exit(3, "ERROR: Undefined identifier!\n");
                        }
                        checkArguments(&local_table, node->left, sym_node);
                    }
                }
                else {
                    return T_ERROR_RET;
                }
            }
            else {
                sym_node = symtable_search(local_table, node->token->value.string_value);
            }
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
    return T_ERROR_RET;
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
            node->entry->type = getRetType(convertToASTType(T_KW, FunctionType->right->left->right->token->value.keyword)); //accessing TYPE DATA

        } else if (FunctionType->right->type == P_TYPE) {
            node->entry->isNullable = false;
            node->entry->returnsValue = true;
            node->entry->type = getRetType(convertToASTType(T_KW, FunctionType->right->right->token->value.keyword)); //accessing TYPE DATA
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
        if(node->left->left != NULL) {
            if(node->left->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->left->right != NULL) {
                    if(node->left->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->left->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->left->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->left->right->token->value.string_value);
                    left_id = symtable_search(SymFunctionTree, builtinFun);
                    if(left_id == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, left_id);
                }
                else {
                    left_id = symtable_search(SymFunctionTree, node->left->token->value.string_value);
                    if(left_id == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, left_id);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            left_id = symtable_search(local_table, node->left->token->value.string_value);
        }
        if(left_id == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(left_id->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function in division operation (left)!\n");
        }
        left_type = left_id->entry->type;
        if(left_id->entry->entry_type == T_VAR_SYM) {
            left_id->entry->isUsed = true;
        }
        else if(left_id->entry->entry_type == T_FUN_SYM) {
            left_id->entry->isUsed = true;
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
        if(node->right->left != NULL) {
            if(node->right->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->right->right != NULL) {
                    if(node->right->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                    right_id = symtable_search(SymFunctionTree, builtinFun);
                    if(right_id == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, right_id);
                }
                else {
                    right_id = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                    if(right_id == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, right_id);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            right_id = symtable_search(local_table, node->right->token->value.string_value);
        }
        if(right_id == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(right_id->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function in division operation (right)!\n");
        }
        right_type = right_id->entry->type;
        if(right_id->entry->entry_type == T_VAR_SYM) {
            right_id->entry->isUsed = true;
        }
        else if(right_id->entry->entry_type == T_FUN_SYM) {
            right_id->entry->isUsed = true;
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
    if((left_type != T_INT_RET && left_type != T_FLOAT_RET) || (right_type != T_INT_RET && right_type != T_FLOAT_RET)) {
        return T_ERROR_RET;
    }
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
    else {
        // f64 to i32 if the decimal part is 0 (only literals and constants)
        if(left_type == T_FLOAT_RET) {
            if(node->left->type == ID) {
                // variables/constants
                if(node->left->left == NULL) {
                    if(!left_id->entry->isConst) {
                        return T_ERROR_RET;
                    }
                    if(!left_id->entry->isConvertable) {
                        return T_ERROR_RET;
                    }
                }
                else {
                    return T_ERROR_RET;
                }
            }
            if(!floatIsInt(node->left->token->value.float_value)) {
                return T_ERROR_RET;
            }
            ASTNode* temp = node->left;
            Token* new_token = (Token*)malloc(sizeof(Token));
            if (new_token == NULL){ 
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(99, "Memory allocation failed"); 
            }
            new_token->type = T_F2I;
            // create node for F2I
            ASTNode* new_node = nodeCreate(F2I, new_token);
            new_node->right = temp;
            node->left = new_node;
            // set division to be for whole numbers
            node->token->type = T_IDIV;
            node->type = IDIV;
            return T_INT_RET;
        }
        if(right_type == T_FLOAT_RET) {
            if(node->right->type == ID) {
                // variables/constants
                if(node->right->left == NULL) {
                    if(!right_id->entry->isConst) {
                        return T_ERROR_RET;
                    }
                    if(!right_id->entry->isConvertable) {
                        return T_ERROR_RET;
                    }
                }
                else {
                    return T_ERROR_RET;
                }
            }
            if(!floatIsInt(node->right->token->value.float_value)) {
                return T_ERROR_RET;
            }
            ASTNode* temp = node->right;
            Token* new_token = (Token*)malloc(sizeof(Token));
            if (new_token == NULL){ 
                symtable_dispose(&SymFunctionTree);
                freeAST(ASTRoot);
                error_exit(99, "Memory allocation failed"); 
            }
            new_token->type = T_F2I;
            // create node for F2I
            ASTNode* new_node = nodeCreate(F2I, new_token);
            new_node->right = temp;
            node->right = new_node;
            // set division to be for whole numbers
            node->token->type = T_IDIV;
            node->type = IDIV;
            return T_INT_RET;
        }
    }
    return T_ERROR_RET;
}

ret_type checkAritTypes(ASTNode* node, symtable_node_ptr local_table) {
    if(node->left == NULL || node->right == NULL) {
        return T_ERROR_RET;
    }
    ret_type left_type;
    ret_type right_type;
    symtable_node_ptr sym_node_left, sym_node_right;
    // left node
    if(node->left->type == ID) {
        if(node->left->left != NULL) {
            if(node->left->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->left->right != NULL) {
                    if(node->left->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->left->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->left->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->left->right->token->value.string_value);
                    sym_node_left = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node_left == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node_left);
                }
                else {
                    sym_node_left = symtable_search(SymFunctionTree, node->left->token->value.string_value);
                    if(sym_node_left == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node_left);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node_left = symtable_search(local_table, node->left->token->value.string_value);
        }
        if(sym_node_left == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(sym_node_left->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function in arithmetic operation (left)!\n"); 
        }
        left_type = sym_node_left->entry->type;
        if(sym_node_left->entry->entry_type == T_VAR_SYM) {
            sym_node_left->entry->isUsed = true;
        }
        else if(sym_node_left->entry->entry_type == T_FUN_SYM) {
            sym_node_left->entry->isUsed = true;
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
    // right node
    if(node->right->type == ID) {
        if(node->right->left != NULL) {
            if(node->right->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->right->right != NULL) {
                    if(node->right->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                    sym_node_right = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node_right == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node_right);
                }
                else {
                    sym_node_right = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                    if(sym_node_right == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node_right);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node_right = symtable_search(local_table, node->right->token->value.string_value);
        }
        if(sym_node_right == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(sym_node_right->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function in arithmetic operation (right)!\n"); 
        }
        right_type = sym_node_right->entry->type;
        if(sym_node_right->entry->entry_type == T_VAR_SYM) {
            sym_node_right->entry->isUsed = true;
        }
        else if(sym_node_right->entry->entry_type == T_FUN_SYM) {
            sym_node_right->entry->isUsed = true;
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
    // create i2f conversion node for left operand
    if(left_type == T_INT_RET && right_type == T_FLOAT_RET) {
        if(node->left->type == ID && !sym_node_left->entry->isConst) {
            return T_ERROR_RET;
        }
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
        if(node->right->type == ID && !sym_node_right->entry->isConst) {
            return T_ERROR_RET;
        }
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
    if(node->right->type == ID) {
        symtable_node_ptr sym_node;
        // functions
        if(node->right->left != NULL) {
            if(node->right->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->right->right != NULL) {
                    if(node->right->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                    sym_node = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node);
                }
                // user defined functions
                else {
                    sym_node = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        // variables
        else {
            sym_node = symtable_search(local_table, node->right->token->value.string_value);
        }
        if(sym_node->entry->entry_type == T_VAR_SYM) {
            sym_node->entry->isUsed = true;
            return sym_node->entry->type;
        }
        else if(sym_node->entry->entry_type == T_FUN_SYM) {
            sym_node->entry->isUsed = true;
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
    // TODO: nullable types in expressions valid?
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
        symtable_node_ptr sym_node;
        if(node->left->left != NULL) {
            if(node->left->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->left->right != NULL) {
                    if(node->left->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->left->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->left->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->left->right->token->value.string_value);
                    sym_node = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node);
                }
                else {
                    sym_node = symtable_search(SymFunctionTree, node->left->token->value.string_value);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node = symtable_search(local_table, node->left->token->value.string_value);
        }
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
            left_type = sym_node->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else if(isOperator(node->left->token)) {
        if(checkExpr(node->left, local_table) == T_BOOL_RET) {
            left_type = T_BOOL_RET;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type in boolean operation (left operand)!\n");
        }
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    if(node->right->type == ID) {
        symtable_node_ptr sym_node;
        if(node->right->left != NULL) {
            if(node->right->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->right->right != NULL) {
                    if(node->right->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                    sym_node = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node);
                }
                else {
                    sym_node = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node = symtable_search(local_table, node->right->token->value.string_value);
        }
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
            right_type = sym_node->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else if(isOperator(node->right->token)) {
         if(checkExpr(node->right, local_table) == T_BOOL_RET) {
            right_type = T_BOOL_RET;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type in boolean operation (right operand)!\n");
        }
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    if(left_type != T_BOOL_RET || right_type != T_BOOL_RET) {
        return T_ERROR_RET;
    }
    return T_BOOL_RET;
}

ret_type checkRel(ASTNode* node, symtable_node_ptr local_table) {
    ret_type left_type, right_type;
    symtable_node_ptr sym_node_right, sym_node_left;
    if(node->left->type == ID) {
        if(node->left->left != NULL) {
            if(node->left->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->left->right != NULL) {
                    if(node->left->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->left->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->left->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->left->right->token->value.string_value);
                    sym_node_left = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node_left == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node_left);
                }
                else {
                    sym_node_left = symtable_search(SymFunctionTree, node->left->token->value.string_value);
                    if(sym_node_left == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node_left);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node_left = symtable_search(local_table, node->left->token->value.string_value);
        }
        if(sym_node_left == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined identifier!\n");
        }
        if(sym_node_left->entry->entry_type == T_VAR_SYM) {
            sym_node_left->entry->isUsed = true;
            left_type = sym_node_left->entry->type;
        }
        else if(sym_node_left->entry->entry_type == T_FUN_SYM) {
            sym_node_left->entry->isUsed = true;
            left_type = sym_node_left->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else if(isOperator(node->left->token)) {
        left_type = checkExpr(node->left, local_table);
    }
    // literals
    else {
        left_type = convertToRetType(node->left->type);
    }
    if(node->right->type == ID) {
        if(node->right->left != NULL) {
            if(node->right->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->right->right != NULL) {
                    if(node->right->right->type != ID) {
                        return T_ERROR_RET;
                    } 
                    // if the first ID isn't "ifj"
                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                    sym_node_right = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node_right == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node_right);
                }
                else {
                    sym_node_right = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                    if(sym_node_right == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node_right);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node_right = symtable_search(local_table, node->right->token->value.string_value);
        }
        if(sym_node_right == NULL || sym_node_right->entry == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined identifier!\n");
        }
        if(sym_node_right->entry->entry_type == T_VAR_SYM) {
            sym_node_right->entry->isUsed = true;
            right_type = sym_node_right->entry->type;
        }
        else if(sym_node_right->entry->entry_type == T_FUN_SYM) {
            sym_node_right->entry->isUsed = true;
            right_type = sym_node_right->entry->type;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown symtable return type!\n");
        }
    }
    else if(isOperator(node->right->token)) {
        right_type = checkExpr(node->right, local_table);
    }
    // literals
    else {
        right_type = convertToRetType(node->right->type);
    }
    if(node->left == NULL || node->right == NULL) {
        return T_ERROR_RET;
    }

    bool isNull_left = false;
    bool isNull_right = false;
    if(node->left->type == ID) {
        if(sym_node_left->entry->isNullable) {
            isNull_left = true;
        }
    }
    else {
        if(node->left->type == T_NULL) {
            isNull_left = true;
        }
    }
    if(node->right->type == ID) {
        if(sym_node_right->entry->isNullable) {
            isNull_right = true;
        }
    }
    else {
        if(node->right->type == T_NULL) {
            isNull_right = true;
        }
    }
    // check if both operands are nullable in "==" || "!="
    if(node->type == EQ || node->type == NEQ) {
        if(isNull_left && !isNull_right) {
            return T_ERROR_RET;
        }
        if(!isNull_left && isNull_right) {
            return T_ERROR_RET;
        }
        if(node->type == ID) {
            if(sym_node_left->entry->isNullable && sym_node_right->entry->isNullable) {
                if(sym_node_left->entry->type != sym_node_right->entry->type) {
                    return T_ERROR_RET;
                }
            }
        }
        
        if(node->left->type != T_NULL && node->right->type != T_NULL) {
            if(node->left->type == TYPE_STR || node->right->type == TYPE_STR) {
                return T_ERROR_RET;
            }
        }
    }
    // cannot have nullable operands
    else {
        if(isNull_left || isNull_right) {
            return T_ERROR_RET;
        }
        if(node->left->type == TYPE_STR || node->right->type == TYPE_STR) {
            return T_ERROR_RET;
        }
    }
    if(left_type == T_INT_RET && right_type == T_FLOAT_RET) {
        if(node->left->type == ID && !sym_node_left->entry->isConst) {
            return T_ERROR_RET;
        }
        else {
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
            return T_BOOL_RET;
        }
    }
    // create i2f conversion node for right operand
    else if(right_type == T_INT_RET && left_type == T_FLOAT_RET) {
        if(node->right->type == ID && !sym_node_right->entry->isConst) {
            return T_ERROR_RET;
        }
        else {
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
            return T_BOOL_RET;
        }
    }
    if(left_type != right_type) {
        return T_ERROR_RET;
    }
    else {
        return T_BOOL_RET;
    }
}

ret_type checkOrElse(ASTNode* node, symtable_node_ptr local_table) {
    ret_type left_type;
    ret_type right_type;
    symtable_node_ptr sym_node_left;
    symtable_node_ptr sym_node_right;
    if(node->left->type == ID) {
        if(node->left->left != NULL) {
            if(node->left->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->left->right != NULL) {
                    if(node->left->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->left->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->left->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->left->right->token->value.string_value);
                    sym_node_left = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node_left == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node_left);
                }
                else {
                    sym_node_left = symtable_search(SymFunctionTree, node->left->token->value.string_value);
                    if(sym_node_left == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->left, sym_node_left);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node_left = symtable_search(local_table, node->left->token->value.string_value);
        }
        if(sym_node_left == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        if(!sym_node_left->entry->isNullable) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Nullable variable/function is not in orelse operation!\n");
        }
        left_type = sym_node_left->entry->type;
        if(sym_node_left->entry->entry_type == T_VAR_SYM) {
            sym_node_left->entry->isUsed = true;
        }
        else if(sym_node_left->entry->entry_type == T_FUN_SYM) {
            sym_node_left->entry->isUsed = true;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown state!\n");
        }
    }
    else if(isOperator(node->left->token)) {
        if(node->left->type != T_ORELSE) {
            return T_ERROR_RET;
        }
        else {
            left_type = checkExpr(node->left, local_table);
        }
    }
    else {
        left_type = convertToRetType(node->left->type);
    }
    if(node->right->type == ID) {
        if(node->right->left != NULL) {
            if(node->right->left->type == P_EXPRESSION_LIST) {
                // built-in function
                if(node->right->right != NULL) {
                    if(node->right->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->right->right->token->value.string_value);
                    sym_node_right = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node_right == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node_right);
                }
                // normal functions
                else {
                    sym_node_right = symtable_search(SymFunctionTree, node->right->token->value.string_value);
                    if(sym_node_right == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->right->left, sym_node_right);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node_right = symtable_search(local_table, node->right->token->value.string_value);
        }
        if(sym_node_right == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        right_type = sym_node_right->entry->type;
        if(sym_node_right->entry->entry_type == T_VAR_SYM) {
            sym_node_right->entry->isUsed = true;
        }
        else if(sym_node_right->entry->entry_type == T_FUN_SYM) {
            sym_node_right->entry->isUsed = true;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown state!\n");
        }
    }
    else if(isOperator(node->left->token)) {
        right_type = checkExpr(node->left, local_table);
    }
    else {
        right_type = convertToRetType(node->right->type);
    }
    if((left_type == T_NULL_RET || node->left->type == ID) && right_type == T_UNREACHABLE_RET) {
        if(node->left->type == ID) {
            if(sym_node_left->entry->entry_type == T_VAR_SYM) {
                if(sym_node_left->entry->isNull) {
                    symtable_dispose(&SymFunctionTree);
                    freeAST(ASTRoot);
                    error_exit(57, "panic: reached unreachable code\n");
                }
            }
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(57, "panic: reached unreachable code\n");
        }
    }
    if(left_type != right_type && right_type != T_UNREACHABLE_RET) {
        return T_ERROR_RET;
    }
    else {
        return left_type;
    }
}

ret_type checkAtAs(ASTNode* node, symtable_node_ptr local_table) {
    // AT_AS
    if(node == NULL) {
        return T_ERROR_RET;
    }
    // P_EXPRESSION_LIST
    if(node->left == NULL) {
        return T_ERROR_RET;
    }
    if(node->left->type != P_EXPRESSION_LIST) {
        return T_ERROR_RET;
    }
    // P_EXPRESSION
    if(node->left->right == NULL) {
        return T_ERROR_RET;
    }
    if(node->left->right->type != P_EXPRESSION) {
        return T_ERROR_RET;
    }
    if(node->left->right->right == NULL) {
        return T_ERROR_RET;
    }
    ret_type node_type;
    symtable_node_ptr sym_node;
    // variables and functions
    if(node->left->right->right->type == ID) {
        // functions
        if(node->left->right->right->left != NULL) {
            if(node->left->right->right->left->type == P_EXPRESSION_LIST) {
                // built-in functions (write, ord ...)
                if(node->left->right->right->right != NULL) {
                    if(node->left->right->right->right->type != ID) {
                        return T_ERROR_RET;
                    }
                    // if the first ID isn't "ifj"
                    if(strcmp(node->left->right->right->token->value.string_value, "ifj")) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    // if the 2nd part of the built-in function is too long
                    if (strlen(node->left->right->right->right->token->value.string_value) > 9) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Missing definition!\n");
                    }
                    char builtinFun[14] = "ifj.";
                    strcpy(builtinFun+4, node->left->right->right->right->token->value.string_value);
                    sym_node = symtable_search(SymFunctionTree, builtinFun);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    checkArguments(&local_table, node->left->right->right->left, sym_node);
                }
                // normal functions
                else {
                    sym_node = symtable_search(SymFunctionTree, node->left->right->right->token->value.string_value);
                    if(sym_node == NULL) {
                        symtable_dispose(&SymFunctionTree);
                        freeAST(ASTRoot);
                        error_exit(3, "ERROR: Undefined identifier!\n");
                    }
                    // pass in P_EXPRESSION_LIST of function
                    checkArguments(&local_table, node->left->right->right->left, sym_node);
                }
            }
            else {
                return T_ERROR_RET;
            }
        }
        else {
            sym_node = symtable_search(local_table, node->left->right->right->token->value.string_value);
        }
        if(sym_node == NULL) {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(3, "ERROR: Undefined variable!\n");
        }
        node_type = sym_node->entry->type;
        if(sym_node->entry->entry_type == T_VAR_SYM) {
            sym_node->entry->isUsed = true;
        }
        else if(sym_node->entry->entry_type == T_VAR_SYM) {
            sym_node->entry->isUsed = true;
        }
        else {
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(10, "ERROR: Uknown state!\n");
        }
    }
    else if(isOperator(node->left->right->right->token)) {
        node_type = checkExpr(node->left->right->right, local_table);
    }
    // literals
    else {
        node_type = convertToRetType(node->left->right->right->type);
    }
    return (node_type == T_INT_RET) ? T_INT_RET : T_ERROR_RET;
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    //will need changes - term
    symtable_entry_t entry4 = {
        .entry_type = T_FUN_SYM,
        .type = T_ANY,
        .isUsed = true,
        .isNullable = false,
        .param_count = 1,
        .returnsValue = false,
        .param_nullable = NULL,
        .param_types = NULL,
        .local_symtable = NULL,
    };
    entry4.param_nullable = malloc(sizeof(bool));
    if (entry4.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry4.param_nullable[0] = true;

    entry4.param_types = malloc(sizeof(ret_type));
    if (entry4.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry4.param_types[0] = T_ANY;
    *key->entry = entry4;
    // ifj.i2f
    symtable_insert(&SymFunctionTree,"ifj.i2f",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.i2f");
    symtable_free_entry(key->entry);
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
    entry5.param_types[0] = T_INT_RET;
    *key->entry = entry5;
    // ifj.f2i
    symtable_insert(&SymFunctionTree,"ifj.f2i",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.f2i");
    symtable_free_entry(key->entry);
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
    entry6.param_types[0] = T_FLOAT_RET;
    *key->entry = entry6;
    // ifj.string
    symtable_insert(&SymFunctionTree,"ifj.string",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"ifj.string");
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    symtable_free_entry(key->entry);
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
    // @as
    symtable_insert(&SymFunctionTree,"@as",T_FUN_SYM);
    key = symtable_search(SymFunctionTree,"@as");
    symtable_free_entry(key->entry);
    key->entry = malloc(sizeof(symtable_entry_t));
    if (key->entry == NULL) {
        error_exit(99,"malloc failed");
    }
    symtable_entry_t entry14 = {
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
    entry14.param_nullable = malloc(1 * sizeof(bool));
    if (entry14.param_nullable==NULL) {
        error_exit(99,"malloc failed");
    }
    entry14.param_nullable[0] = false;

    entry14.param_types = malloc(1 * sizeof(ret_type));
    if (entry14.param_types==NULL) {
        error_exit(99,"malloc failed");
    }
    entry14.param_types[0] = T_INT_RET;

    *key->entry = entry14;
}
ret_type convertToRetType(ASTNodeType node_type) {
    switch(node_type) {
        case T_UNREACHABLE:
            return T_UNREACHABLE_RET;
        case T_NULL:
            return T_NULL_RET;
        case TYPE_INT:
            return T_INT_RET;
        case TYPE_F64:
            return T_FLOAT_RET;
        case T_KW_BOOL:
        case T_TRUE:
        case T_FALSE:
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