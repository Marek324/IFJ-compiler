#include <stdio.h>

#include "parser.h"
#include "utils.h"
#include "symtable.h"

void test_symtable() {
    symtable_node_ptr symtable;
    symtable_init(&symtable);
    printf("TEST 1: Insert single node\n");
    symtable_insert(&symtable, "key1", T_VAR_SYM);
    if (symtable_search(symtable, "key1") == NULL) {
        printf("FAILED: Key1 not found after insertion.\n");
        return;
    } else {
        printf("PASSED: Key1 inserted successfully.\n");
    }

    printf("TEST 2: Insert multiple nodes with balancing\n");
    symtable_insert(&symtable, "key2", T_VAR_SYM);
    symtable_insert(&symtable, "key0", T_VAR_SYM);
    symtable_insert(&symtable, "key3", T_VAR_SYM);
    symtable_insert(&symtable, "key-1", T_VAR_SYM);
    symtable_insert(&symtable, "key4", T_VAR_SYM);

    // Check the balance factors for nodes
    if (update_balances(&symtable)) {
        printf("PASSED: Tree balanced after multiple insertions.\n");
    } else {
        printf("FAILED: Tree not balanced after multiple insertions.\n");
    }

    printf("TEST 3: Search for existing and non-existing keys\n");
    if (symtable_search(symtable, "key2") != NULL) {
        printf("PASSED: Key2 found in symtable.\n");
    } else {
        printf("FAILED: Key2 not found in symtable.\n");
    }

    if (symtable_search(symtable, "key999") == NULL) {
        printf("PASSED: Key999 not found as expected.\n");
    } else {
        printf("FAILED: Key999 should not exist in symtable.\n");
    }

    printf("TEST 4: Handle duplicate insertion\n");
    // Expect an error_exit when attempting to insert a duplicate key
    printf("EXPECTED: Error for duplicate key insertion...\n");

    printf("TEST 5: Cleanup\n");
    symtable_dispose(&symtable);
    if (symtable == NULL) {
        printf("PASSED: Symtable disposed successfully.\n");
    } else {
        printf("FAILED: Symtable disposal failed.\n");
    }
}
// Example usage
int main() {
    //circ_buff_ptr buffer = circ_buff_init();
    //Parse(buffer);

    // Debug
    //print_tree(ASTRoot);
    
    // codegen();
    
    //circ_buff_free(buffer);
    //freeAST(ASTRoot);
    test_symtable();
    return 0;
}
