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

ASTNode* parseExpression(Token* token);

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