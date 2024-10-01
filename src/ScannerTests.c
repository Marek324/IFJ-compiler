/*
file: ScannerTests.c
author: Roman Andraščík
Tests....for the scanner.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../include/scanner.h"
#include "../include/error.h"
//#include "../include/dyn_str.h"
//#include "../include/token.h"
///////////////////////////// SET UP
// FILE *fptrHelloWorld;
// fptrHelloWorld = fopen("../TestDocuments/HelloWord.zig","r");
// if(fptrHelloWorld == NULL){
//     printf("Error opening file - HelloWord.zig");   
//     exit(1);             
// }
FILE *fptrTest1;
int *line;
Token *token;

//////////////////////////////

void test1(void **state) {

    *line = 1;
    token = get_token(fptrTest1, line);
    assert_true(token->type == T_KW );


}

int main(void) {
    ///////////////////////////// SET UP
    fptrTest1 = fopen("../TestDocuments/TokenTest1.txt","r");
    if(fptrTest1 == NULL){
    printf("Error opening file - TokenTest1.txt");   
    exit(1);             
    }

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test1),
    };

    //////////////////// CLOSING DOCUMENTS
    //fclose(fptrHelloWorld);
    fclose(fptrTest1);
    return cmocka_run_group_tests(tests, NULL, NULL);
}


