#include "stack_utils.h"



symtable_node_ptr stackUtilCopy(symtable_node_ptr tree){
    if (tree == NULL) {
        return NULL;
    }
    // create new node
    symtable_node_ptr tree_copy = malloc(sizeof(symtable_node_t));
    if(tree_copy == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for symtable_node_ptr-Utils\n");
    }
    // copy data
    tree_copy->balance_factor = tree->balance_factor;

    char* newStr = (char*)malloc(strlen(tree->key) + 1);
    if(newStr == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for newStr-Utils\n");
    }
    strcpy(newStr, tree->key);
    tree_copy->key = newStr;

    symtable_entry_ptr entryNew = symtable_entry_init(T_VAR_SYM);
    if (entryNew == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for symtable_entry_ptr\n");
    }

    tree_copy->entry = entryNew;
  
    tree_copy->entry->entry_type = tree->entry->entry_type;
    tree_copy->entry->hasExplicitType= tree->entry->hasExplicitType;
    tree_copy->entry->isChanged = tree->entry->isChanged;
    tree_copy->entry->isConst = tree->entry->isConst;
    tree_copy->entry->isNullable = tree->entry->isNullable;
    tree_copy->entry->isUsed = tree->entry->isUsed;
    tree_copy->entry->type = tree->entry->type;

    // copy next node
    tree_copy->left = stackUtilCopy(tree->left);
    tree_copy->right = stackUtilCopy(tree->right);

    return tree_copy;
}


symtable_node_ptr stackUtilPop(stack_t* stack){
    symtable_node_ptr tree = (symtable_node_ptr)stackGetTop(stack);
    symtable_node_ptr tree_copy = stackUtilCopy(tree);
    symtable_dispose(&tree);
    stackPop(stack);
    return tree_copy;
}

void updateTableBySameKey(symtable_node_ptr oldTable, symtable_tree_ptr localTable){
    if (oldTable == NULL || *localTable == NULL) return NULL;

    if (strcmp(oldTable->key, (*localTable)->key) == 0) {
        ((*localTable)->entry->isChanged) = oldTable->entry->isChanged;
        ((*localTable)->entry->isUsed) = oldTable->entry->isUsed;
    }
    // traverse old table
    updateTableBySameKey(oldTable->left,*localTable);
    updateTableBySameKey(oldTable->right,*localTable);
    // traverse local table
    updateTableBySameKey(oldTable,&((*localTable)->left));
    updateTableBySameKey(oldTable,&((*localTable)->right));

}