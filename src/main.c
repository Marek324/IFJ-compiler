#include <stdio.h>

#include "parser.h"
#include "utils.h"
#include "symtable.h"
#include "codegen.h"
#include "sem_anal.h"

int main() {
    circ_buff_ptr buffer = circ_buff_init();
    symtable_init(&SymFunctionTree);
    SCOPEStack = stackInit();
    Parse(buffer);
    analyse(ASTRoot);
    
    #ifdef DEBUG
    generate_d2_file(ASTRoot, "tree.d2");
    setbuf(stdout, NULL);
    #endif
    
    int err = codegen(); // can be 0 or 99 on allocation error
    // int err = 0;
    stackClear(SCOPEStack);
    circ_buff_free(buffer);
    freeAST(ASTRoot); 
    symtable_dispose(&SymFunctionTree);
    return err; 
}
