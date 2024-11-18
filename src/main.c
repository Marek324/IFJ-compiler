#include <stdio.h>

#include "parser.h"
#include "utils.h"

// Example usage
int main() {
    circ_buff_ptr buffer = circ_buff_init();
    Parse(buffer);
    print_tree(ASTRoot);
    circ_buff_free(buffer);
    freeAST(ASTRoot);
    return 0;
}
