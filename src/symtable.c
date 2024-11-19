#include <string.h>

#include "symtable.h"
#include "error.h"
#include "ast.h"

int max(int a, int b) {
    return a > b ? a : b;
}

char* myStrDup(char* key) {
    char* newStr = (char*)malloc(strlen(key) + 1);
    if(newStr == NULL) {
        freeAST(ASTRoot);
        error_exit(99, "ERROR: Unable to allocate memory for newStr\n");
    }
    strcpy(newStr, key);
    return newStr;
}

int height(symtable_node_ptr node){
    if (node == NULL) {
        return 0;
    }
    return 1 + max(height(node->left), height(node->right));
}

void symtable_free_entry(symtable_entry_ptr entry) {
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

void symtable_dispose(symtable_node_ptr root) {
    if (root == NULL) {
        return;
    }
    symtable_dispose(root->left);
    symtable_dispose(root->right);
    symtable_free_entry(root->entry);
    free(root);
    root = NULL;
}
