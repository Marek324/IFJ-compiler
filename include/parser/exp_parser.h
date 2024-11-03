/*
file: exp_parser.h
author: Adam Vesely
Header file for the expression parser.
*/

#ifndef EXP_PARSER_H
#define EXP_PARSER_H

#include "parser/ast.h"
#include "common/token.h"
#include "parser/parser.h"
#include "scanner/circ_buff.h"
#include "common/error.h"

// the associativity of operators
enum ASOC {
    L,  /* Lower precedence: "<" -> we push onto the stack (shift) */
    H,  /* Higher precedence: ">" -> we reduce */
    EQ, /* Equal precedence: "=" */
    UN  /* Undefined, causing an error */
}
// the indices of the precedence table
enum PREC_TABLE_INDX{
    // operators
    UNREACHABLE, /* ".?" postfix operator -> ("a.?" == "a orelse unreachable") -> if "a" is null -> print: "panic: reached unreachable code" return code "57" */
    NEGATE,      /* "!a" */
    MUL_DIV,     /* "*, /" */
    ADD_SUB,     /* "+, -" */
    ORELSE,      /* "x = a orelse b" -> if a is null x will be "b" with the same data type. Else it will be "a" with the same datatype. */
    RELATION,    /* "<, >, <=, >=, ==, !=" */
    AND,         /* "a and b" */
    OR,          /* "a or b" */

    L_PAR,       /* "(" */
    R_PAR,       /* ")" */
    ID,          /* Identifier */
    END          /* "$" */
};

ASTNode* parseExpression(Token* token, circ_buff_ptr buffer);

// MUL_DIV, ADD_SUB, ORELSE, RELATION, AND, OR
ASTNode* parseBinary(Token* token);
// UNREACHABLE, NEGATE
ASTNode* parseUnary(Token* token);
// Functions as expressions
ASTNode* parseFunctionCall(Token* token);


#endif // EXP_PARSER_H