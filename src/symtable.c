#include "symtable.h"

int max(int a, int b) {
    return a > b ? a : b;
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
