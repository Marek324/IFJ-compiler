/*
file: exp_parser.h
author: Adam Vesely
Header file for the expression parser.
*/

#ifndef EXP_PARSER_H
#define EXP_PARSER_H

#include "ast.h"
#include "token.h"
#include "parser.h"
#include "circ_buff.h"

enum PRIORITY{
    UNREACHABLE, /* ".?" postfix operator -> ("a.?" == "a orelse unreachable") -> if "a" is null -> print: "panic: reached unreachable code" return code "57" */
    MUL_DIV, /* "*, /" */
    ADD_SUB, /* "+, -" */
    ORELSE, /* "x = a orelse b" -> if a is null x will be "b" with the same data type. Else it will be "a" with the same datatype. */
    RELATION_OP /* "<, >, <=, >=, ==, !=" */
}

ASTNode* parseExpression(Token* token, circ_buff_ptr buffer);

ASTNode* parseAssignment(Token* token);
ASTNode* parseBinary(Token* token);
ASTNode* parseUnary(Token* token);
ASTNode* parsePrimary(Token* token);

int getPrecedence(TokenType type)

// ASTNode *parseFunctionCall(Token *token);
// ASTNode *parseIfStatement(Token *token);
// ASTNode *parseWhileLoop(Token *token);
// ASTNode *parseReturnStatement(Token *token);


#endif // EXP_PARSER_H