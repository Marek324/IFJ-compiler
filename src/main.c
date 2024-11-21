#include <stdio.h>

#include "parser.h"
#include "utils.h"
#include "symtable.h"
#include "codegen.h"

// post-order printing 
void print_AVL(symtable_node_ptr node) {
    if (node == NULL) {
        return;
    }
    print_AVL(node->left);
    print_AVL(node->right);
    printf("%s,", node->key);
}

void test_symtable() {
    symtable_node_ptr symtable;
    symtable_init(&symtable);
    printf("TEST 1: Insert single node\n");
    symtable_insert(&symtable, "55", T_VAR_SYM);
    if (symtable_search(symtable, "j") == NULL) {
        printf("FAILED: Key1 not found after insertion.\n");
    } else {
        printf("PASSED: Key1 inserted successfully.\n");
    }

    printf("TEST 2: Insert multiple nodes with balancing\n");
    symtable_insert(&symtable, "54", T_VAR_SYM);
    symtable_insert(&symtable, "56", T_VAR_SYM);
    symtable_insert(&symtable, "53", T_VAR_SYM);
    symtable_insert(&symtable, "57", T_VAR_SYM);
    symtable_insert(&symtable, "52", T_VAR_SYM);

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
    symtable_insert(&symtable, "51", T_VAR_SYM);
    print_AVL(symtable);

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
    // test_symtable();
    circ_buff_ptr buffer = circ_buff_init();
    Parse(buffer);
    
    // Debug
    generate_d2_file(ASTRoot, "tree.d2");
    
    codegen();
    
    // test_symtable();
    circ_buff_free(buffer);
    freeAST(ASTRoot);
    return 0;
}
