#include "stack_utils.h"



symtable_node_ptr stackUtilCopy(symtable_node_ptr tree){

    if (tree == NULL) {
        return NULL;
    }
    // create new node
    symtable_node_ptr tree_copy = (symtable_node_ptr)malloc(sizeof(symtable_node_t));
    
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



    tree_copy->entry->entry_type = tree->entry->entry_type;
    tree_copy->entry->hasExplicitType= tree->entry->hasExplicitType;
    tree_copy->entry->isChanged = tree->entry->isChanged;
    tree_copy->entry->isConst = tree->entry->isConst;
    tree_copy->entry->isNullable = tree->entry->isNullable;
    tree_copy->entry->isUsed = tree->entry->isUsed;
    tree_copy->entry->param_count = tree->entry->param_count;
    tree_copy->entry->param_nullable = tree->entry->param_nullable;
    tree_copy->entry->param_types = tree->entry->param_types;
    tree_copy->entry->returnsValue = tree->entry->returnsValue;
    tree_copy->entry->type = tree->entry->type;

    tree_copy->entry->local_symtable = (symtable_tree_ptr)malloc(sizeof(symtable_node_t));
    if(tree_copy->entry->local_symtable == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for local_symtable-Utils\n");
    }
    tree_copy->entry->local_symtable = tree->entry->local_symtable;

    // copy next node
    tree_copy->left = stackUtilCopy(tree->left);
    tree_copy->right = stackUtilCopy(tree->right);

    return tree_copy;
}

symtable_node_ptr stackUtilPop(stack_t* stack){
    symtable_node_ptr tree_copy = stackUtilCopy((symtable_node_ptr)stackGetTop(stack));
    stackPop(stack);
    return tree_copy;
}