#include <string.h>
#include "symtable.h"
#include "error.h"
#include "ast.h"

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
        entry->param_nullable = malloc(sizeof(ret_type));
        if(entry->param_nullable == NULL) {
            freeAST(ASTRoot);
            error_exit(99, "ERROR: Unable to allocate memory for entry->param_nullable\n");
        }
        entry->param_types =  malloc(sizeof(ret_type));
        if(entry->param_types == NULL) {
            freeAST(ASTRoot);
            error_exit(99, "ERROR: Unable to allocate memory for entry->param_types\n");
        }
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

int max(int a, int b) {
    return a > b ? a : b;
}

int height(symtable_node_ptr node) {
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
        if(entry->param_nullable != NULL) {
            free(entry->param_nullable);
        }

        if(entry->param_types != NULL) {
            free(entry->param_types);
        }
        symtable_dispose(entry->local_symtable);
        free(entry);
    }
}

void symtable_dispose(symtable_tree_ptr tree) {
    if (*tree == NULL) {
        return;
    }
    symtable_dispose(&((*tree)->left));
    symtable_dispose(&((*tree)->right));
    symtable_free_entry((*tree)->entry);
    free((*tree)->key);
    free((*tree));
    *tree = NULL;
}

void symtable_init(symtable_tree_ptr tree) {
    *tree = NULL;
}

void symtable_insert(symtable_tree_ptr tree, char* new_key, symtable_entry_type type) {
    if (*tree == NULL) {
        *tree = symtable_node_create(new_key, type);
        return;
    }
    // (*tree)->balance_factor = 
}

void symtable_remove(symtable_node_ptr node) {
    if (node == NULL) {
        return;
    }
}

void simple_right_rot(symtable_tree_ptr root) {
    symtable_node_ptr new_root = (*root)->left;
    (*root)->left = new_root->right;
    new_root->right = *root;

    // Update root pointer to the new root
    *root = new_root;

    // Recalculate balance factors or heights here if needed
    (*root)->balance_factor = get_balance(*root);
    (*root)->right->balance_factor = get_balance((*root)->right);
}

void simple_left_rot(symtable_tree_ptr root) {
    symtable_node_ptr new_root = (*root)->right;
    (*root)->right = new_root->left;
    new_root->left = *root;

    // Update root pointer to the new root
    *root = new_root;

    // Recalculate balance factors or heights here if needed
    (*root)->balance_factor = get_balance(*root);
    (*root)->left->balance_factor = get_balance((*root)->left);
}

void right_left_rot(symtable_tree_ptr root) {
    // Perform a simple left rotation on the left child
    simple_left_rot(&((*root)->left));

    // Perform a simple right rotation on the root
    simple_right_rot(root);
}

void left_right_rot(symtable_tree_ptr root) {
    // Perform a simple right rotation on the right child
    simple_right_rot(&((*root)->right));

    // Perform a simple left rotation on the root
    simple_left_rot(root);
}

void rebalance(symtable_tree_ptr root) {
    int balance = get_balance(*root);

    if (balance > 1) {  // Right heavy
        if (get_balance((*root)->right) < 0) {
            right_left_rot(root);  // Right-Left case
        } else {
            simple_left_rot(root);  // Right-Right case
        }
    } else if (balance < -1) {  // Left heavy
        if (get_balance((*root)->left) > 0) {
            left_right_rot(root);  // Left-Right case
        } else {
            simple_right_rot(root);  // Left-Left case
        }
    }
}


