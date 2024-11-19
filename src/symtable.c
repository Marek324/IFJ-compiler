#include <string.h>
#include "symtable.h"
#include "error.h"
#include "ast.h"

symtable_node_ptr symtable_node_create(char *key) {
    symtable_node_ptr node = (symtable_node_ptr)malloc(sizeof(symtable_node_t));
    node->key = myStrDup(key);
    node->left = NULL;
    node->right = NULL;
    node->entry = (symtable_entry_ptr) malloc(sizeof(symtable_entry_t));
    node->balance_factor = 1;

    return node;
}

int max(int a, int b) {
    return a > b ? a : b;
}

char* my_str_dup(char* key) {
    char* newStr = (char*)malloc(strlen(key) + 1);
    if(newStr == NULL) {
        freeAST(ASTRoot);
        error_exit(99, "ERROR: Unable to allocate memory for newStr\n");
    }
    strcpy(newStr, key);
    return newStr;
}

int get_balance (symtable_node_ptr node) {
    return node != NULL ? height(node->right) - height(node->left) : 0;
}

int height(symtable_node_ptr node){
    if (node == NULL) {
        return 0;
    }
    return 1 + max(height(node->left), height(node->right));
}

void symtable_free_entry(symtable_entry_ptr entry) {
    if(entry->isNullable != NULL) {
        free(entry->isNullable);
    }
    if(entry->param_types != NULL) {
        free(entry->param_types);
    }
    symtable_dispose(entry->local_symtable);
    free(entry);
}

void symtable_init(symtable_tree_ptr tree) {
    *tree = NULL;
}

void symtable_insert(symtable_node_ptr node) {
    if (node == NULL) {
        return;
    }
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

void rebalance(symtable_node_ptr *root) {
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


void symtable_dispose(symtable_node_ptr root) {
    if (root == NULL) {
        return;
    }
    symtable_dispose(root->left);
    symtable_dispose(root->right);
    symtable_free_entry(root->entry);
    free(root->key);
    free(root);
    root = NULL;
}
