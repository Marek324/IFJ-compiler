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
    return node != NULL ? height(node->left) - height(node->right) : 0;
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
    symtable_node_ptr T2 = new_root->right;
    
    //Rotate
    new_root->right = *root;
    (*root)->left = T2;

    // Recalculate balance factors or heights here if needed
    (*root)->balance_factor = get_balance(*root);
    T2->balance_factor = get_balance(T2);
}

void simple_left_rot(symtable_tree_ptr root) {

    symtable_node_ptr new_root = (*root)->right;
    symtable_node_ptr T2 = new_root->left;

    // Rotation
    new_root->left = *root;
    (*root)->right = T2;
    
    // Recalculate balance factors or heights here if needed
    (*root)->balance_factor = get_balance(*root);

    T2->balance_factor = get_balance(T2);
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
