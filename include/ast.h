#ifndef AST_H__
#define AST_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  
#include "error.h"
#include "token.h"

typedef enum {
// Non-terminal nodes (for each rule in P)
    PROG,               // <prog> -> <prolog> <function_def> <end>
    PROLOG,             // <prolog> -> const ID = @ import ( <expression> ) ;
    END,                // <end> -> EOF
    FUNCTION_DEF,       // <function_def> -> pub fn ID (<param_list>) <type_complete> <block> <function_def> | ε
    PARAM_LIST,         // <param_list> -> ID : <type_complete> <comma_par_found> | ε
    COMMA_PAR_FOUND,    // <comma_par_found> -> , <param> | ε
    PARAM,              // <param> -> ID : <type_complete> <comma_par_found> | ε
    BLOCK,              // <block> -> { <statement> }
    STATEMENT,          // <statement> -> <var_declaration> <statement> | ID <ID_found> <statement> | ... | ε
    VAR_DECLARATION,    // <var_declaration> -> const ID : <type_complete> = <expression> ; | var ID : <type_complete> = <expression> ;
    IF_STATEMENT,       // <if_statement> -> if ( <expression> ) <optional_value> <block> <else_statement>
    ELSE_STATEMENT,     // <else_statement> -> else <block> | ε
    OPTIONAL_VALUE,     // <optional_value> -> | ID | | ε
    WHILE_LOOP,         // <while_loop> -> while ( <expression> ) <optional_value> <optional_statements> <block> <else_statement>
    RETURN_STATEMENT,   // <return_statement> -> return <expression> ;
    EXPRESSION_LIST,    // <expression_list> -> <expression> <comma_expr_found> | ε
    COMMA_EXPR_FOUND,   // <comma_expr_found> -> , <expression_list2> | ε
    EXPRESSION_LIST2,   // <expression_list2> -> <expression> <comma_expr_found> | ε
    TYPE,               // <type> -> i32 | f64 | [ ] u8 | void | bool
    FOR_LOOP,           // <for_loop> -> for ( <expression> ) <optional_value> <block>
    OPTIONAL_STATEMENTS,// <optional_statements> -> : <block> | ε
    ID_FOUND,           // <ID_found> -> = <expression> ; | ( <expression_list> ) | : <while_loop> | . ID ( <expression_list> );
    TYPE_COMPLETE,      // <type_complete> -> <question_mark> <type>
    QUESTION_MARK,      // <question_mark> -> ? | ε
    EXPRESSION          // expession_parser()
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
