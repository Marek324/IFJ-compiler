#ifndef PARSER__H_
#define PARSER__H_

#include "ast.h"
#include "scanner.h"
#include "exp_parser.h"

void Parse(circ_buff_ptr buffer);

void Prog(Token **token,  ASTNode *ptr, circ_buff_ptr buffer);

void Prolog(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void FunctionDef(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void End(Token **token, ASTNode *ptr); 

void Expression(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void ParamList(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void CommaParFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void TypeComplete(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void FunctionType(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void QMark(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void Type(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void Block(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void Statement(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void VarDeclaration(Token **token, ASTNode *ptr, circ_buff_ptr buffer, bool semic);

void IdFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer, bool semic);

void ExpressionList(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void SingleStatement(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void ExprCommaFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void IfStatement(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void IfFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void OptionalValue(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void ElseStatement(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void While(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

void Return(Token **token, ASTNode *ptr, circ_buff_ptr buffer);

#endif // PARSER__H_