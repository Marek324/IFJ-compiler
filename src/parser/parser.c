#include "../include/parser.h"

void Parse() {
    circ_buff_ptr buffer = circ_buff_init();
    Token *token = get_token(buffer);
    initializeAST();
    Prog(token);

}

void Prog(Token *token) {
    ASTNode *prolog = nodeCreate(P_PROLOG, NULL);
    insertRight(ASTRoot,prolog);
    Prolog(token, prolog);
    ASTNode *function_def = nodeCreate(P_FUNCTION_DEF, NULL);
    insertLeft(prolog, function_def);
    FunctionDef(token, function_def);
    ASTNode *end = nodeCreate(P_END, NULL);
    insertLeft(function_def, end);
    End(token, end);
}


void Prolog(Token *token, ASTNode *ptr) {
    if (token->value.keyword == KW_CONST) {
         ASTNode *constFound = nodeCreate(T_CONST, token);
    }
    else {
        error_exit(1, "SYNTAX ERROR!\n");
    }
}

void FunctionDef(Token *token, ASTNode *ptr) {
    return;
}

void End(Token *token, ASTNode *ptr) {
    return;
}