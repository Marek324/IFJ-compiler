/*
file: exp_parser.h
author: Adam Vesely
Header file for the expression parser.
*/

#ifndef EXP_PARSER_H
#define EXP_PARSER_H

#include <stdbool.h>

#include "parser/ast.h"
#include "common/token.h"
#include "parser/parser.h"
#include "scanner/circ_buff.h"
#include "common/error.h"

// the associativity of operators (operator on top of stack vs new operator)
typedef enum {
    L,  /* Lower precedence: "<" -> we push onto the stack (shift) */
    H,  /* Higher precedence: ">" -> we reduce (will be lower in the tree) -> down-top parsing */
    E, /* Equal precedence: "=" (reduce) */
    U  /* Undefined, causing an error */
} ASOC;
// the indices of the precedence table
typedef enum {
    // operators
    UNREACHABLE, /* ".?" postfix binary operator -> ("a.?" == "a orelse unreachable") -> if "a" is null -> print: "panic: reached unreachable code" return code "57" */
    NEGATE,      /* "!a" */
    MUL_DIV,     /* "*, /" */
    ADD_SUB,     /* "+, -" */
    ORELSE,      /* "x = a orelse b" -> if a is null x will be "b" with the same data type. Else it will be "a" with the same datatype. */
    RELATION,    /* "<, >, <=, >=, ==, !=" */
    AND,         /* "a and b" */
    OR,          /* "a or b" */

    L_PAR,       /* "(" */
    R_PAR,       /* ")" */
    ID,          /* Identifier/literal */
    END          /* "$" -> can be ')', ';', ... */
} PREC_TABLE_INDEX;

ASTNode* parseExpression(Token* token, circ_buff_ptr buffer);

// ID
ASTNode* parsePrimary(Token* token, circ_buff_ptr buffer);
// UNREACHABLE, NEGATE
ASTNode* parseUnary(Token* token, circ_buff_ptr buffer);
// NEGATE
ASTNode* preUnary(Token* token, circ_buff_ptr buff);
// UNREACHABLE
ASTNode* postUnary(Token* token, circ_buff_ptr buff);
// MUL_DIV, ADD_SUB, ORELSE, RELATION, AND, OR
ASTNode* parseBinary(Token* token, circ_buff_ptr buffer);
// Functions as expressions
ASTNode* parseFunctionCall(Token* token, circ_buff_ptr buffer);
// pops the top operator and the needed amount of operands to link them together, creating a tree
// pushes the newly created tree (root of tree) onto the operand stack 
void reduce(stack_t* operand_stack, stack_t* operator_stack);
// returns the precedence table index for the token
PREC_TABLE_INDEX getIndex(Token* token);
// checks if the token is a operator
bool isOperator(Token* token);
// checks if the token is an operand
bool isOperand(Token* token);
// checks if the token is a potential end for the expression
bool isEnd(Token* token);
// checks if the expression has ended
bool expressionEnd(Token* token, bool operand_stack_isEmpty, bool operator_stack_isEmpty);


#endif // EXP_PARSER_H