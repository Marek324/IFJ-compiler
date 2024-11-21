#include <stdlib.h>
#include <stdbool.h>

#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef enum {
    T_VAR_SYM,
    T_FUN_SYM,
} symtable_entry_type;

typedef enum {
    T_INT_RET,
    T_FLOAT_RET,
    T_BOOL_RET,
    T_STR_RET,
    T_NULL_RET
} ret_type;

typedef struct symtable_node_s *symtable_node_ptr;
typedef struct symtable_node_s **symtable_tree_ptr;

typedef struct symtable_entry_s{
    symtable_entry_type entry_type;
    
    // common
    ret_type type;
    bool isNullable;
    bool isUsed;
    bool isChanged;
    bool isDefined;

    // variables
    bool isConst; 
    bool hasExplicitType;

    // functions
    int param_count;
    bool *param_nullable;
    ret_type *param_types;
    bool returnsValue;
    symtable_tree_ptr local_symtable;
} symtable_entry_t, *symtable_entry_ptr;

typedef struct symtable_node_s{
    char *key;
    struct symtable_node_s *left, *right;
    symtable_entry_ptr entry;
    int balance_factor;
} symtable_node_t;

symtable_node_ptr symtable_node_create(char *key, symtable_entry_type type);

void symtable_dispose(symtable_tree_ptr tree);

char* my_str_dup(char* key);

int max(int a, int b);

int height(symtable_node_ptr node);

symtable_node_ptr symtable_search(symtable_node_ptr tree, char *key);

int get_balance (symtable_node_ptr node);

void symtable_free_entry(symtable_entry_ptr entry);

void symtable_init(symtable_tree_ptr tree);

void symtable_insert(symtable_tree_ptr tree, char* key, symtable_entry_type type);

void simple_right_rot(symtable_tree_ptr root);

void simple_left_rot(symtable_tree_ptr root);
// updates balances of all nodes and returns a bool (true->rebalance, false->all ok)
bool update_balances(symtable_tree_ptr tree);

void rebalance(symtable_tree_ptr root);

symtable_entry_ptr symtable_entry_init();

// symtable_entry_var_modify();

// symtable_entry_fun_modify();

#endif // SYMTABLE_H
