#include <string.h>
#include "symtable.h"
#include "error.h"


symtable_node_ptr SymFunctionTree = NULL;

char* my_str_dup(char* key) {
    char* newStr = (char*)malloc(strlen(key) + 1);
    if(newStr == NULL) {
        freeAST(ASTRoot);
        error_exit(99, "ERROR: Unable to allocate memory for newStr\n");
    }
    strcpy(newStr, key);
    return newStr;
}

symtable_entry_ptr symtable_entry_init(symtable_entry_type type) {
    symtable_entry_ptr entry = malloc(sizeof(symtable_entry_t));
    if(entry == NULL) {
        freeAST(ASTRoot);
        error_exit(99, "ERROR: Unable to allocate memory for symtable_entry_ptr\n");
    }
    if(type == T_FUN_SYM) {
        entry->param_count = 0;
        entry->param_nullable = NULL;
        entry->param_types = NULL;
        entry->local_symtable = malloc(sizeof(symtable_node_t));
        if(entry->local_symtable == NULL) {
            freeAST(ASTRoot);
            error_exit(99, "ERROR: Unable to allocate memory for entry->local_symtable\n");
        }
        symtable_init(entry->local_symtable);
    }
    entry->entry_type = type;
    return entry;
}

symtable_node_ptr symtable_node_create(char *key, symtable_entry_type type) {
    symtable_node_ptr node = (symtable_node_ptr)malloc(sizeof(symtable_node_t));
    if(node == NULL) {
        freeAST(ASTRoot);
        error_exit(99, "ERROR: Unable to allocate memory for symtable_node_ptr\n");
    }
    node->key = my_str_dup(key);
    node->left = NULL;
    node->right = NULL;
    node->entry = symtable_entry_init(type);
    node->balance_factor = 0;

    return node;
}
void symtable_get_function_param_info(symtable_node_ptr tree, char *key, ASTNode *ParamList, int i, int capacity) {
    symtable_node_ptr node = symtable_search(tree, key);
    
    if (node->entry->param_nullable == NULL && node->entry->param_types == NULL) {
        node->entry->param_nullable = malloc(capacity*sizeof(bool));
        node->entry->param_types = malloc(capacity*sizeof(ret_type));
        if (node->entry->param_nullable == NULL || node->entry->param_types == NULL) {
            freeAST(ASTRoot);
            error_exit(99, "ERROR: Unable to allocate memory\n");
        }
    }

    if (i == capacity - 1) {
        capacity = 2 * capacity;
        node->entry->param_nullable = realloc(node->entry->param_nullable, capacity * sizeof(bool));
        node->entry->param_types = realloc(node->entry->param_types, capacity * sizeof(ret_type));
        if (node->entry->param_nullable == NULL || node->entry->param_types == NULL) {
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
int max(int a, int b) {
    return a > b ? a : b;
}

int height(symtable_node_ptr node){
    if (node == NULL) {
        return 0;
    }
    return 1 + max(height(node->left), height(node->right));
}

int get_balance (symtable_node_ptr node) {
    return node != NULL ? height(node->right) - height(node->left) : 0;
}

void symtable_free_entry(symtable_entry_ptr entry) {   
    if (entry != NULL) {
        if(entry->entry_type == T_FUN_SYM) {
            if(entry->param_nullable != NULL) {
                free(entry->param_nullable);
                entry->param_nullable = NULL;
            }

            if(entry->param_types != NULL) {
                free(entry->param_types);
                entry->param_types = NULL;
            }
            symtable_dispose(entry->local_symtable);
            entry->local_symtable = NULL;    
        }
        free(entry);
        entry = NULL;
    }
}

void symtable_init(symtable_tree_ptr tree) {
    *tree = NULL;
}

void symtable_insert(symtable_tree_ptr tree, char* new_key, symtable_entry_type type) {
    if (*tree == NULL) {
        *tree = symtable_node_create(new_key, type);
        return;
    }
    // new_key is bigger than the node key (we go right)
    if(strcmp((*tree)->key, new_key) < 0) {
        symtable_insert(&((*tree)->right), new_key, type);
    }
    // new_key is smaller than the node key (we go left)
    else if(strcmp((*tree)->key, new_key) > 0) {
        symtable_insert(&((*tree)->left), new_key, type);
    }
    else {
        // key already exists in symtable
        symtable_dispose(tree);
        freeAST(ASTRoot);
        error_exit(5, "ERROR: Redefinition of a variable or function!\n");
    }
    if(update_balances(tree)) {
        rebalance(tree);
    }
}

// we pass in the node that has the imbalance
// then we take its left child and make it the new root. The old root will be the right child of the new root
// If the new root had a right child tree it will now be the left child tree of the old node
void simple_right_rot(symtable_tree_ptr root) {

    symtable_node_ptr new_root = (*root)->left;
    symtable_node_ptr old_root_child = new_root->right;
    
    //Rotate
    new_root->right = *root;
    (*root)->left = old_root_child;
    *root = new_root;
}

// we pass in the node that has the imbalance
// then we take its left child and make it the new root. The old root will be the right child of the new root
// If the new root had a right child tree it will now be the left child tree of the old node
void simple_left_rot(symtable_tree_ptr root) {
    symtable_node_ptr new_root = (*root)->right;
    symtable_node_ptr old_root_child = new_root->left;

    // Rotation
    new_root->left = *root;
    (*root)->right = old_root_child;
    *root = new_root;
}

void left_right_rot(symtable_tree_ptr root) {
    simple_left_rot(&((*root)->left));
    simple_right_rot(root);
}

void right_left_rot(symtable_tree_ptr root) {
    simple_right_rot(&((*root)->right));
    simple_left_rot(root);
}

symtable_node_ptr symtable_search(symtable_node_ptr tree, char *key) {
    if (tree == NULL) return NULL;

  if (strcmp(tree->key, key) == 0) {
    return tree;
  }
  else if (strcmp(tree->key, key) < 0 ) {
    return symtable_search(tree->right, key);
  }
  else {
      return symtable_search(tree->left, key);
    }
}

bool update_balances(symtable_tree_ptr tree) {
    if (*tree == NULL) {
        return false;
    }

    bool left_unbalanced = update_balances(&((*tree)->left));
    bool right_unbalanced = update_balances(&((*tree)->right));

    // Update the current node's balance factor
    (*tree)->balance_factor = get_balance(*tree);

    // Check if the current node or any subtree is unbalanced
    if (((*tree)->balance_factor > 1) || ((*tree)->balance_factor < -1) || left_unbalanced || right_unbalanced) {
        // Tree is unbalanced.
        return true;
    }

    // Tree is balanced.
    return false;
}

void rebalance(symtable_tree_ptr tree) {
    if (tree == NULL || (*tree) == NULL) return;
    rebalance(&((*tree)->left));
    rebalance(&((*tree)->right));
    if((*tree)->balance_factor > 1) {
        if(((*tree)->right)->balance_factor < 0) {
            right_left_rot(tree);
        }
        else {
            simple_left_rot(tree);
        }
    }
    else if((*tree)->balance_factor < -1) {
        if(((*tree)->left)->balance_factor > 0) {
            left_right_rot(tree);
        }
        else {
            simple_right_rot(tree);
        }
    }
    if(update_balances(tree)) {
        rebalance(tree);
    }
}

void symtable_dispose(symtable_tree_ptr tree) {
    if (tree == NULL || (*tree) == NULL) {
        return;
    }
    symtable_dispose(&((*tree)->left));
    symtable_dispose(&((*tree)->right));
    symtable_free_entry((*tree)->entry);
    if((*tree)->key != NULL) {
        free((*tree)->key);
        (*tree)->key = NULL;
    }
    free(*tree);    
    *tree = NULL;
}
