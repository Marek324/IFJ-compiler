/**
 * file: ast.h
 * author: Mikuláš Lešiga xlesigm00
 * Header file for the AST.
 */
#ifndef AST_H__
#define AST_H__

#include <stdbool.h>  
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

typedef enum {
    // Non-terminal nodes (for each rule in P)
    P_PROG,                 // <prog> -> <prolog> <function_def> <end>
    P_PROLOG,               // <prolog> -> const ID = @import ( <expression> ) ;
    P_END,                  // <end> -> EOF
    P_FUNCTION_DEF,         // <function_def> -> pub fn ID (<param_list>) <function_type> <block> <function_def> | ε
    P_PARAM_LIST,           // <param_list> -> ID : <type_complete> <comma_par_found> | ε
    P_COMMA_PAR_FOUND,      // <comma_par_found> -> , <param_list> | ε
    P_BLOCK,                // <block> -> { <statement> } | <single_statement>
    P_STATEMENT,            // <statement> -> <var_declaration> <statement> | ID <ID_found> <statement> | <if_statement> <statement> | <for_loop> <statement> | <while_loop> <statement> | <return_statement> <statement> | <break> <statemet> | <continue> <statement> | ε
    P_ID_FOUND,             // <ID_found> -> = <asgn_found> ; | ( <expression_list> ); | : <while_loop>
    P_VAR_DECLARATION,      // <var_declaration> -> const ID : <type_complete> = <asgn_found> ; | var ID : <type_complete> = <asgn_found> ;
    P_IF_STATEMENT,         // <if_statement> -> if ( <expression> ) <if_found>
    P_IF_FOUND,             // <if_found> -> <optional_value> <block> <else_statement> | <single_statement> else <single_statement>
    P_ELSE_STATEMENT,       // <else_statement> -> else <block> | ε
    P_OPTIONAL_VALUE,       // <optional_value> -> | ID | | ε
    P_WHILE_LOOP,           // <while_loop> -> while ( <expression> ) <optional_value> <optional_statements> <block> <else_statement>
    P_RETURN_STATEMENT,     // <return_statement> -> return <expression> ;
    P_EXPRESSION_LIST,      // <expression_list> -> <expression> <comma_expr_found> | ε
    P_COMMA_EXPR_FOUND,     // <comma_expr_found> -> , <expression_list> | ε
    P_TYPE,                 // <type> -> i32 | f64 | [ ] u8 | bool
    P_FOR_LOOP,             // <for_loop> -> for ( <expression> ) <optional_value> <block>
    P_OPTIONAL_STATEMENTS,  // <optional_statements> -> : <block> | ε
    P_TYPE_COMPLETE,        // <type_complete> -> <question_mark> <type>
    P_QUESTION_MARK,        // <question_mark> -> ? | ε
    P_SINGLE_STATEMENT,     // <single_statement> -> <var_declaration> | ID <ID_found> | <if_statement> | <for_loop> | <while_loop> | <return_statement>
    P_FUNCTION_TYPE,        // <function_type> -> <type> | void
    P_EXPRESSION,           // expession_parser()
    P_ASGN_FOUND,           // <asgn_found> -> <expression> | if (<expression>) <expression> else <expression>
    P_CONTINUE,             // <continue> -> continue ;
    P_BREAK,                // <break> -> break ;


// Terminal nodes (for each terminal in T)
    ID,
    AT_IMPORT,

    TYPE_INT,
    TYPE_F64,
    TYPE_STR,

    ASSGN, EQ,
    BANG, NEQ,
    LESS, LEQ,
    MORE, MEQ,
    
    PLUS,
    MINUS,
    MUL,
    DIV,
    IDIV,
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACKET, RBRACKET,
    
    COMMA,
    DOT,
    SEMICOLON,
    COLON,
    PIPE,
    QMARK,
    AT_AS,
    T_TRUE,
    T_FALSE,

    END_OF_FILE,

    T_CONST,
    T_IF,
    T_ELSE,
    T_FN,
    T__KW_I32,
    T_KW_F64,
    T_KW_BOOL,
    T_NULL,
    T_PUB,
    T_RETURN,
    T_U8,
    T_VAR,
    T_VOID,
    T_WHILE,
    T_UNREACHABLE,
    T_AND,
    T_OR,
    T_ORELSE,
    I2F,
    F2I

} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode *left;
    struct ASTNode *right;
    Token *token;
}ASTNode;

//ASTRoot globally initialized in case of internal or scanner errors 
extern ASTNode *ASTRoot;
// creates an AST node
ASTNode *nodeCreate(ASTNodeType type, Token *token);
// inserts an AST node into the tree to the left of the parent
void insertLeft(ASTNode *parent, ASTNode *leftChild);
// inserts an AST node into the tree to the right of the parent
void insertRight(ASTNode *parent,ASTNode *rightChild);
// frees the AST and all of its components
void freeAST(ASTNode *ptr);
// converts the TokenType into its respective ASTNodeType
ASTNodeType convertToASTType(TokenType type, KeyWordType keyword);


#endif // AST_H__