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

void symtable_init(symtable_tree_ptr tree) {
    *tree = NULL;
}

void symtable_insert(symtable_tree_ptr tree, char* new_key, symtable_entry_type type) {
    if (*tree == NULL) {
        *tree = symtable_node_create(new_key, type);
        if(update_balances(tree)) {
            rebalance(tree);
        }
        return;
    }
    // new_key is bigger than the node key (we go right)
    if(strcmp((*tree)->key, new_key) < 0) {
        symtable_insert(&((*tree)->right), new_key, type);
    }
    // new_key is smaller than the node key (we go left)
    else if(strcmp((*tree)->key, new_key) > 0) {
        symtable_insert(&((*tree)->left), new_key, type);
        fprintf(stderr, "**********\n");
    }
    else {
        // key already exists in symtable
        symtable_dispose(tree);
        freeAST(ASTRoot);
        error_exit(5, "ERROR: Redefinition of a variable or function!\n");
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
        simple_left_rot(tree);
    }
    else if((*tree)->balance_factor < -1) {
        simple_right_rot(tree);
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
    }
    free(*tree);    
    *tree = NULL;
}
