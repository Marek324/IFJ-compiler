/**
 * file: parser.h
 * author: Mikuláš Lešiga xlesigm00
 * Header file for the parser.
 */
#ifndef PARSER__H_
#define PARSER__H_

#include "ast.h"
#include "scanner.h"
#include "exp_parser.h"

// Creates a node without token
ASTNode *ruleNode(ASTNodeType rule);
// Main function for parsing
void Parse(c_buff_ptr buffer);
// Starts parsing
void Prog(Token **token,  ASTNode *ptr, c_buff_ptr buffer);
// checks Prolog
void Prolog(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks Function
void FunctionDef(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks for EOF 
void End(Token **token, ASTNode *ptr); 
// calls expression parser
void Expression(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks Parameter
void ParamList(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for comma -> potentionally more Params
void CommaParFound(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for whole type ?type or only type
void TypeComplete(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for function type
void FunctionType(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for ?
void QMark(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for Type
void Type(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for Block
void Block(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// looks for statements
void Statement(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks declaration of var or const
void VarDeclaration(Token **token, ASTNode *ptr, c_buff_ptr buffer, bool semic);
// after finding ID decides next path
void IdFound(Token **token, ASTNode *ptr, c_buff_ptr buffer, bool semic);
// checks argument in a function call
void ExpressionList(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// makes sure there will be only a single statement
void SingleStatement(Token **token, ASTNode *ptr, c_buff_ptr buffer, bool semic);
// looks for comma -> potentionally more arguments
void ExprCommaFound(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks if Statement
void IfStatement(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks continue 
void Continue(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks break
void Break(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// after finding if (expression) decides next path
void IfFound(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks optional value
void OptionalValue(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks else statement
void ElseStatement(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks while statement
void While(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// signilizes assignment
void AsgnFound(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks for OptionaL statements in while
void OptionalStatements(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks return
void Return(Token **token, ASTNode *ptr, c_buff_ptr buffer);
// checks for 
void For(Token **token, ASTNode *ptr, c_buff_ptr buffer);

#endif // PARSER__H_