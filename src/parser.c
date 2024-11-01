#include "../include/parser.h"
           
ASTNode * checkToken(Token *token, TokenType wantedType, KeyWordType wantedKeyWord) {
    ASTNode *ptr = NULL;
    if (token->type == T_KW) {
        if (token->value.keyword == wantedKeyWord) {
            ptr = nodeCreate(convertToASTType(T_KW, wantedKeyWord), token);
            return ptr;
        }
    } else if (token->type == wantedType) {
        ptr = nodeCreate(convertToASTType(wantedType, NO_KW), token);
        return ptr;
    }
    free(ASTRoot);
    error_exit(2, "SYNTAX ERROR!\n"); 
    return NULL;
}

ASTNode *ruleNode(ASTNodeType rule) {
   return nodeCreate(rule, NULL);
}

void Parse() {
    circ_buff_ptr buffer = circ_buff_init();
    Token *token = get_token(buffer);
    initializeAST();
    Prog(token, buffer);
}

void Prog(Token *token, circ_buff_ptr buffer) {
    // P_PROLOG
    ASTNode *prolog = ruleNode(P_PROLOG); 
    insertRight(ASTRoot, prolog);
    Prolog(token, prolog, buffer);
    //P_FUNCTION_DEF
    ASTNode *function_def = ruleNode(P_FUNCTION_DEF); 
    insertLeft(prolog, function_def);
    FunctionDef(token, function_def, buffer);
    //P_END
    ASTNode *end = ruleNode(P_END); 
    insertLeft(function_def, end);
    End(token, end, buffer);
}

void Prolog(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    //CONST
    ASTNode *constFound = checkToken(token, T_KW, KW_CONST);
    insertRight(ptr, constFound);
    //ID
    token = get_token(buffer);
    ASTNode *idFound = checkToken(token, T_ID, NO_KW);
    insertLeft(constFound, idFound);
    // =
    token = get_token(buffer);
    ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW);
    insertLeft(idFound, asgnFound);
    // (
    token = get_token(buffer);
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    insertLeft(asgnFound, lParenFound);
    // P_EXPRESSION
    ASTNode *expression = ruleNode(P_EXPRESSION);
    insertLeft(lParenFound, expression);
    Expression(token, expression, buffer);
    // )
    token = get_token(buffer);
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(expression, rParenFound);
    // ;
    token = get_token(buffer);
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
    insertLeft(rParenFound, semiColonFound);
}

void Expression(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    (void)token; (void)ptr; (void)buffer;
    return;
}

void FunctionDef(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    (void)token; (void)ptr; (void)buffer;
    return;
}

void End(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    (void)token; (void)ptr; (void)buffer;
    return;
}
