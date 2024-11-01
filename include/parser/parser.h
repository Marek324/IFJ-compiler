#ifndef PARSER__H_
#define PARSER__H_

#include "ast.h"



void Parse();

void Prog(Token *token);

void Prolog(Token *token, ASTNode *ptr);

void FunctionDef(Token *token, ASTNode *ptr);

void End(Token *token, ASTNode *ptr); 


#endif // PARSER__H_