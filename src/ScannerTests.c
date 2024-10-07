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
#include "../include/dyn_str.h"


//#include "../include/token.h"
///////////////////////////// SET UP

FILE *fptrTest1;
FILE *fptrTest2;
Token *token;

////////////////////////////// TESTS

void test1(void **state) {
    int line = 1;

    // token = (Token *)malloc(sizeof(Token));
    // if (token == NULL) {
    //         printf("Memory allocation failed - Test 1 \n");
    //         return;
    // }

    TokenType test1Array[14] = {
    T_KW,
    T_ID,
    T_AT_IMPORT,
    
    T_INT,
    T_F64,
    T_STR,

    T_ASGN, T_EQ,
    T_BANG, T_NEQ,
    T_LESS, T_LEQ,
    T_MORE, T_MEQ
    };

    for (int i = 0; i < 14; i++)
    {
        token = get_token(fptrTest1, &line);
        if (token == NULL) {
            printf("Error: token is NULL at index %d\n", i);
            assert_int_equal(i, 15); // Force fail the test
            return;
        } 
        printf("Test2: %i   value :%d string:%s \n",i,token->type, token->value.string_value->str);
        assert_true(token->type == test1Array[i]);
        free(token);
    }
}

void test2(void **state) {
    int line = 1;
    // token = (Token *)malloc(sizeof(Token));
    // if (token == NULL) {
    //     printf("Memory allocation failed - Test 2 \n");
    //     return;
    // }

    TokenType test2Array[21] = {
    T_PLUS, T_PASGN,
    T_MINUS, T_MASGN,
    T_MUL, T_MULASGN,
    T_DIV, T_DASGN,

    T_LPAREN, T_RPAREN,
    T_LBRACE, T_RBRACE,
    T_LBRACKET, T_RBRACKET,

    T_COMMA,
    T_DOT,
    T_SEMICOL,
    T_COLON,
    T_PIPE,
    T_QMARK,

    T_EOF
    };

    for (int i = 0; i < 21; i++)
    {
        token = get_token(fptrTest2, &line);
        if (token == NULL) {
            printf("Error: token is NULL at index %d\n", i);
            assert_int_equal(i, 22); // Force fail the test
            return;
        }       
        printf("Test2: %i   value :%d string:%s \n",i,token->type, token->value.string_value->str); 
        assert_true(token->type == test2Array[i]);
        free(token);
    }
}

int runTests(){
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test1),
        cmocka_unit_test(test2)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

int main(void) {
    ///////////////////////////// SET UP
    fptrTest1 = fopen("TestDocuments/TokenTest1.txt","r");
    if(fptrTest1 == NULL){
    printf("Error opening file - TokenTest1.txt\n");   
    exit(1);             
    }
    fptrTest2 = fopen("TestDocuments/TokenTest2.txt","r");
    if(fptrTest2 == NULL){
    printf("Error opening file - TokenTest2.txt\n");   
    exit(1);             
    }

    //////////////////////// RUNNING TESTS
    runTests();
    // const struct CMUnitTest tests[] = { 
    //     cmocka_unit_test(test1),
    //     cmocka_unit_test(test2)
    // };

    //////////////////// CLOSING DOCUMENTS
    //fclose(fptrHelloWorld);
    fclose(fptrTest1);
    fclose(fptrTest2);
    //return cmocka_run_group_tests(tests, NULL, NULL);
}


