#include <stdio.h>

#include "parser.h"
#include "utils.h"

// Example usage
int main() {
    circ_buff_ptr buffer = circ_buff_init();
    Parse(buffer);

    // Debug
    print_tree(ASTRoot);
    
    // codegen();
    
    circ_buff_free(buffer);
    freeAST(ASTRoot);
    return 0;
}
