#include <stdio.h>

#include "parser.h"
#include "utils.h"
#include "symtable.h"
#include "codegen.h"
#include "sem_anal.h"

// post-order printing 



void test_symtable() {
    symtable_node_ptr symtable;
    symtable_init(&symtable);
    printf("TEST 1: Insert single node\n");
    symtable_insert(&symtable, "j", T_VAR_SYM);
    if (symtable_search(symtable, "j") == NULL) {
        printf("FAILED: Key1 not found after insertion.\n");
    } else {
        printf("PASSED: Key1 inserted successfully.\n");
    }

    printf("TEST 2: Insert multiple nodes with balancing\n");
    symtable_insert(&symtable, "k", T_VAR_SYM);
    symtable_insert(&symtable, "i", T_VAR_SYM);
    symtable_insert(&symtable, "l", T_VAR_SYM);
    symtable_insert(&symtable, "h", T_VAR_SYM);
    symtable_insert(&symtable, "g", T_VAR_SYM);

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
    // symtable_insert(&symtable, "key2", T_VAR_SYM);
    // Expect an error_exit when attempting to insert a duplicate key
    fprintf(stderr, "EXPECTED: Error for duplicate key insertion...\n");

    printf("TEST 5: Right imbalance (rotate left)\n");
    printf("EXPECTED OUTPUT: h,i,k,n,m,l,j\n");
    print_AVL(symtable);

    printf("TEST 6: Left imbalance (rotate right)\n");
    symtable_insert(&symtable, "key2", T_VAR_SYM);

    printf("TEST 7: Cleanup\n");
    symtable_dispose(&symtable);
    if (symtable == NULL) {
        printf("PASSED: Symtable disposed successfully.\n");
    } else {
        printf("FAILED: Symtable disposal failed.\n");
    } 
}
// Example usage
int main() {
    circ_buff_ptr buffer = circ_buff_init();
    symtable_init(&SymFunctionTree);
    SCOPEStack = stackInit();
    Parse(buffer);
    analyse(ASTRoot);
    
    //print_AVL(SymFunctionTree);
    
    #ifdef DEBUG
    generate_d2_file(ASTRoot, "tree.d2");
    #endif
    
    //codegen();
     
    circ_buff_free(buffer);
    freeAST(ASTRoot); 
    symtable_dispose(&SymFunctionTree);
    return 0;
}
