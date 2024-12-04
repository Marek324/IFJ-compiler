/**
 * file: stack_utils.h
 * author: Roman Andraščík xandrar00
 * Header file for stack utils.
 */
#ifndef STACK_U
#define STACK_U

#include "symtable.h"
#include "stack.h"

// Creates a new node pointer, copies data from old (param) node to new node which is then returned
symtable_node_ptr stackUtilCopy(symtable_node_ptr tree);
// Copies top of stack into a new node, pops it from stack and returns it
symtable_node_ptr stackUtilPop(stack_t* stack);
// Updates values isChanged & isUsed in the local table based on old table - only on nodes where key is equivalent
void updateTableBySameKey(symtable_node_ptr oldTable, symtable_tree_ptr localTable);

#endif