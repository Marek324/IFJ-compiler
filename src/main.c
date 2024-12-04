/**
 * main.c
 * author: Marek Hric xhricma00
 * author: Adam Veselý xvesela00
 * author: Mikuláš Lešiga xlesigm00
 * author: Roman Andraščík xandrar00
 */

#include <stdio.h>

#include "parser.h"
#include "symtable.h"
#include "codegen.h"
#include "sem_anal.h"

int main() {
    c_buff_ptr buffer = c_buff_init();
    symtable_init(&SymFunctionTree);
    SCOPEStack = stackInit();
    Parse(buffer);
    analyse(ASTRoot);
        
    int err = codegen();
    stackClear(SCOPEStack);
    c_buff_free(buffer);
    freeAST(ASTRoot); 
    symtable_dispose(&SymFunctionTree);
    return err; 
}
