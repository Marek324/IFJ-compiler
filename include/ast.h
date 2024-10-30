#ifndef AST_H__
#define AST_H__

#include <stdbool.h>  
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

typedef enum {
// Non-terminal nodes (for each rule in P)
    P_PROG,               // <prog> -> <prolog> <function_def> <end>
    P_PROLOG,             // <prolog> -> const ID = @ import ( <expression> ) ;
    P_END,                // <end> -> EOF
    P_FUNCTION_DEF,       // <function_def> -> pub fn ID (<param_list>) <type_complete> <block> <function_def> | ε
    P_PARAM_LIST,         // <param_list> -> ID : <type_complete> <comma_par_found> | ε
    P_COMMA_PAR_FOUND,    // <comma_par_found> -> , <param> | ε
    P_PARAM,              // <param> -> ID : <type_complete> <comma_par_found> | ε
    P_BLOCK,              // <block> -> { <statement> }
    P_STATEMENT,          // <statement> -> <var_declaration> <statement> | ID <ID_found> <statement> | ... | ε
    P_VAR_DECLARATION,    // <var_declaration> -> const ID : <type_complete> = <expression> ; | var ID : <type_complete> = <expression> ;
    P_IF_STATEMENT,       // <if_statement> -> if ( <expression> ) <optional_value> <block> <else_statement>
    P_ELSE_STATEMENT,     // <else_statement> -> else <block> | ε
    P_OPTIONAL_VALUE,     // <optional_value> -> | ID | | ε
    P_WHILE_LOOP,         // <while_loop> -> while ( <expression> ) <optional_value> <optional_statements> <block> <else_statement>
    P_RETURN_STATEMENT,   // <return_statement> -> return <expression> ;
    P_EXPRESSION_LIST,    // <expression_list> -> <expression> <comma_expr_found> | ε
    P_COMMA_EXPR_FOUND,   // <comma_expr_found> -> , <expression_list2> | ε
    P_EXPRESSION_LIST2,   // <expression_list2> -> <expression> <comma_expr_found> | ε
    P_TYPE,               // <type> -> i32 | f64 | [ ] u8 | void | bool
    P_FOR_LOOP,           // <for_loop> -> for ( <expression> ) <optional_value> <block>
    P_OPTIONAL_STATEMENTS,// <optional_statements> -> : <block> | ε
    P_ID_FOUND,           // <ID_found> -> = <expression> ; | ( <expression_list> ) | : <while_loop> | . ID ( <expression_list> );
    P_TYPE_COMPLETE,      // <type_complete> -> <question_mark> <type>
    P_QUESTION_MARK,      // <question_mark> -> ? | ε
    P_EXPRESSION,         // expession_parser()


// Terminal nodes (for each terminal in T)
    END_OF_FILE,
    T_CONST,
    T_VAR,
    AT_IMPORT,
    LPAREN,
    RPAREN,
    SEMICOLON,
    ASGN,
    PUB,
    FN,
    ID,
    COLON,
    LBRACKET,
    RBRACKET,
    IF_STATEMENT,
    ELSE_STATEMENT,
    VERTICALBAR,
    WHILE_STATEMENT,
    RETURN_STATEMENT,
    I32_TYPE,
    F64_TYPE,
    LBRACE,
    RBRACE,
    U8_TYPE,
    VOID_TYPE,
    BOOL_TYPE,
    FOR,
    COMMA,
    DOT

} ASTNodeType;



typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode *left;
    struct ASTNode *right;
    Token *token;

}ASTNode;

//ASTRoot globally initialized in case of internal or scanner errors 
ASTNode *ASTRoot;

void initializeAST();

ASTNode *nodeCreate(ASTNodeType type, Token *token);

void insertLeft(ASTNode *parent, ASTNode *leftChild);

void insertRight(ASTNode *parent,ASTNode *rightChild);

void freeAST(ASTNode *ptr);


#endif // AST_H__
