#include <stdlib.h>

#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef struct {
    int data;
} symtable_entry_t, *symtable_entry_ptr;

typedef struct {
    symtable_node_ptr left, right;
    symtable_entry_ptr entry;
    int balance_factor;
} symtable_node_t, *symtable_node_ptr, **symtable_tree_ptr;

#endif // SYMTABLE_H