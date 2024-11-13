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
    freeAST(ASTRoot);
    error_exit(2, "SYNTAX ERROR!\n"); 
    return NULL;
}

ASTNode *ruleNode(ASTNodeType rule) {
   ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        freeAST(ASTRoot);
        error_exit(99, "Malloc failed\n");
    }
    node->type = rule;
    node->left = NULL;
    node->right = NULL;
    node->token = NULL;
    return node;
}

void Parse() {
    circ_buff_ptr buffer = circ_buff_init();
    Token *token = NULL;
    initializeAST();
    Prog(token, buffer);
} 

void Prog(Token *token, circ_buff_ptr buffer) {
    // P_PROLOG
    ASTNode *prolog = ruleNode(P_PROLOG); 
    insertRight(ASTRoot, prolog);
    token = get_token(buffer);
    Prolog(token, prolog, buffer);
    // P_FUNCTION_DEF
    ASTNode *function_def = ruleNode(P_FUNCTION_DEF); 
    insertLeft(prolog, function_def);
    FunctionDef(token, function_def, buffer);
    // P_END
    ASTNode *end = ruleNode(P_END); 
    insertLeft(function_def, end);
    End(token, end, buffer);
}

void Prolog(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // CONST
    ASTNode *constFound = checkToken(token, T_KW, KW_CONST);
    insertRight(ptr, constFound);
    // ID 
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
    token = get_token(buffer);
    Expression(token, expression, buffer);
    if (expression->right == NULL) {
        freeAST(ASTRoot); 
        error_exit(2, "SYNTAX ERROR!\n"); 
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(expression, rParenFound);
    // ;
    token = get_token(buffer);
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
    insertLeft(rParenFound, semiColonFound);
    token = get_token(buffer);
}

void Expression(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    ASTNode *expressionFound = parseExpression(token, buffer);
    if (expressionFound != NULL) {
        insertRight(ptr, expressionFound);
    }
}

void FunctionDef(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // PUB
    ASTNode *pubFound = checkToken(token, T_KW, KW_PUB);
    insertRight(ptr, pubFound);
    // FN
    token = get_token(buffer);
    ASTNode *fnFound = checkToken(token, T_KW, KW_PUB);
    insertLeft(pubFound, fnFound);
    // ID
    token = get_token(buffer);
    ASTNode *idFound = checkToken(token, T_ID, NO_KW);
    insertLeft(fnFound, idFound);
    // (
    token = get_token(buffer);
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    insertLeft(idFound, lParenFound);
    // P_PARAM_LIST
    token = get_token(buffer);
    if(token->type == T_ID) {
    ASTNode *paramList = ruleNode(P_PARAM_LIST);
    insertLeft(lParenFound,paramList);
    ParamList(token, paramList, buffer);
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(paramList, rParenFound);
    }
    else {
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(lParenFound, rParenFound);
    }
    // P_TYPE_COMPLETE
    ASTNode *function_type = ruleNode(P_FUNCTION_TYPE);
    token = get_token(buffer);
    FunctionType(token, function_type, buffer);
    // P_BLOCK
    ASTNode *block = ruleNode(P_BLOCK);
    insertLeft(function_type, block);
    Block(token, block, buffer);
    // RECURSIVELY CALL Function Def
    if (token->type == T_KW && token->value.keyword == KW_PUB) {
        ASTNode *function_def = ruleNode(P_FUNCTION_DEF);
        insertLeft(block, function_def);
        FunctionDef(token, function_def, buffer);
    } else token = get_token(buffer);
}

void ParamList(Token *token, ASTNode *ptr, circ_buff_ptr buffer){
    // ID
    ASTNode *idFound = checkToken(token, T_ID, NO_KW);
    insertRight(ptr, idFound);
    // :
    token = get_token(buffer);
    ASTNode *colonFound = checkToken(token, T_COLON, NO_KW);
    insertLeft(idFound, colonFound);
    // P_TYPE_COMPLETE
    token = get_token(buffer);
    ASTNode *typeComplete = ruleNode(P_TYPE_COMPLETE);
    insertLeft(colonFound, typeComplete);
    TypeComplete(token, typeComplete, buffer);
    //P_COMMA_PAR_FOUND
    if (token->type == T_COMMA) {
    ASTNode *commaFound = ruleNode(P_COMMA_PAR_FOUND);
    insertLeft (typeComplete, commaFound);
    CommaParFound(token, commaFound, buffer);
    }
    else token = get_token(buffer);
}

void CommaParFound(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ,
    ASTNode *commaFound = checkToken(token, T_COMMA, NO_KW);
    insertRight(ptr, commaFound);
    // P_PARAM_LIST
    token = get_token(buffer);
    if(token->type == T_ID) {
    ASTNode *paramList = ruleNode(P_PARAM_LIST);
    insertLeft(commaFound,paramList);
    ParamList(token, paramList, buffer);
    }
    else token = get_token(buffer);
}

void TypeComplete(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ?
    if (token->type == T_QMARK) {
        ASTNode *qmark = ruleNode(P_QUESTION_MARK);
        insertRight(ptr, qmark);
        QMark(token, qmark, buffer);
    // P_TYPE
        ASTNode *type = ruleNode(P_TYPE);
        insertLeft(qmark, type);
        Type(token, type, buffer);
    } else {
        ASTNode *type = ruleNode(P_TYPE);
        insertRight(ptr, type);
        Type(token, type, buffer);
    }
}

void QMark(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ?
    ASTNode *qmarkFound = checkToken(token, T_QMARK, NO_KW);
    insertRight(ptr, qmarkFound);

    token = get_token(buffer);
}

void Type(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // [
    if (token->type == T_LBRACKET) {
        ASTNode *lBracketFound = checkToken(token, T_LBRACKET, NO_KW);
        insertRight(ptr, lBracketFound);
        token = get_token(buffer);
    // ]
        ASTNode *rBracketFound = checkToken(token, T_RBRACKET, NO_KW);
        insertLeft(lBracketFound, rBracketFound);
        token = get_token(buffer);
    // u8
        ASTNode *u8Found = checkToken(token, T_KW, KW_U8);
        insertLeft(rBracketFound, u8Found);
    }
    else {
        if (token->type == T_KW) {
            if (token->value.keyword == KW_I32) {
                ASTNode *i32Found = checkToken(token, T_KW, KW_I32);
                insertRight(ptr, i32Found);
            }
            if (token->value.keyword == KW_F64) {
                ASTNode *f64Found = checkToken(token, T_KW, KW_F64);
                insertRight(ptr, f64Found);
            }
            if (token->value.keyword == KW_BOOL) {
                ASTNode *boolFound = checkToken(token, T_KW, KW_BOOL);
                insertRight(ptr, boolFound);
            }
        }
        else {
            freeAST(ASTRoot);
            error_exit(2, "SYNTAX ERROR!\n"); 
        }
    }
    token = get_token(buffer);
}

void FunctionType(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // P_TYPE
    if(token->type == T_QMARK || token->type == T_LBRACKET || (token->type == T_KW && (token->value.keyword == KW_I32 || token->value.keyword == KW_F64 || token->value.keyword == KW_BOOL) )) {
        ASTNode *type_complete = ruleNode(P_TYPE);
        insertRight(ptr, type_complete);
        TypeComplete(token, type_complete, buffer);
    }
    // VOID
    else if (token->value.keyword == KW_VOID) {
                ASTNode *voidFound = checkToken(token, T_KW, KW_VOID);
                insertRight(ptr, voidFound);
                token = get_token(buffer);
            }
        else {
            freeAST(ASTRoot);
            error_exit(2, "SYNTAX ERROR!\n"); 
        }
}

void Block(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // {
    ASTNode *lBraceFound = checkToken(token, T_LBRACE, NO_KW);
    insertRight(ptr, lBraceFound);
    token = get_token(buffer);
    //P_STATEMENT
    ASTNode *statement = ruleNode(P_STATEMENT);
    insertLeft(lBraceFound, statement);
    Statement(token, statement, buffer);
    // }
    ASTNode *rBraceFound = checkToken(token, T_RBRACE, NO_KW);
    insertLeft(statement, rBraceFound);
    token = get_token(buffer);
}

void Statement(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    ASTNode *statementRule = NULL;
    // P_VAR_DECLARATION
    if (token->type == T_KW && (token->value.keyword == KW_CONST || token->value.keyword == KW_VAR)) {
        ASTNode *statementRule = ruleNode(P_VAR_DECLARATION);
        insertRight(ptr, statementRule);
        VarDeclaration(token, statementRule, buffer);
    } 
    // ID
    else if (token->type == T_ID) {
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertRight(ptr, idFound);
    // P_ID_FOUND
        token = get_token(buffer);
        ASTNode *statementRule = ruleNode(P_ID_FOUND);
        insertLeft(idFound, statementRule);
        IdFound(token, statementRule, buffer);
    }
    // P_IF_STATEMENT
    else if (token->type == T_KW && token->value.keyword == KW_IF ) {
        ASTNode *statementRule = ruleNode(P_IF_STATEMENT);
        insertRight(ptr, statementRule);
        IfStatement(token, statementRule, buffer);
    }
    // P_WHILE_STATEMENT
    else if (token->type == T_KW && token->value.keyword == KW_WHILE) {
        ASTNode *statementRule = ruleNode(P_WHILE_LOOP);
        insertRight(ptr, statementRule);
        While(token, statementRule, buffer);
    } 
    //P_RETURN_STATEMENT
    else if (token->type == T_KW && token->value.keyword == KW_RETURN) {
        ASTNode *statementRule = ruleNode(P_RETURN_STATEMENT);
        insertRight(ptr, statementRule);
        Return(token, statementRule, buffer);
    }
    // More statements
    if (token->type == T_ID || (token->type == T_KW && (token->value.keyword == KW_CONST || token->value.keyword == KW_VAR || token->value.keyword == KW_WHILE || token->value.keyword == KW_IF || token->value.keyword == KW_RETURN))) {
        ASTNode *nextStatement = ruleNode(P_STATEMENT);
        insertLeft(statementRule, nextStatement);
        Statement(token, nextStatement, buffer);
    }
}

void VarDeclaration(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {

    if (token->type == T_KW && (token->value.keyword == KW_CONST || token->value.keyword == KW_VAR)) {
        ASTNode *node = NULL;
    // CONST
        if (token->value.keyword == KW_CONST ) {
            ASTNode *node = checkToken(token, T_KW, KW_CONST);
            insertRight(ptr, node);
        }
    // VAR
        else {
            ASTNode *node = checkToken(token, T_KW, KW_VAR);
            insertRight(ptr, node);
        }
        token = get_token(buffer);
    // ID
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertLeft(node, idFound);
        token = get_token(buffer);
    // :
        ASTNode *colonFound = checkToken(token, T_COLON, NO_KW);
        insertLeft(idFound, colonFound);
        token = get_token(buffer);
    // P_TYPE_COMPLETE
        ASTNode *type_complete = ruleNode(P_TYPE_COMPLETE);
        insertLeft(colonFound, type_complete);
        TypeComplete(token, type_complete, buffer);
    // =
        ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW);
        insertLeft(type_complete, asgnFound);
        token = get_token(buffer);
    // P_Expression
        ASTNode *expressionFound = ruleNode(P_EXPRESSION);
        insertLeft(asgnFound, expressionFound);
        Expression(token, expressionFound, buffer);
        if (expressionFound->right == NULL) {
            freeAST(ASTRoot); 
            error_exit(2, "SYNTAX ERROR!\n"); 
        }
    // ;
        ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
        insertLeft(expressionFound, semiColonFound);
        token = get_token(buffer);
    }
    else {
        freeAST(ASTRoot);
        error_exit(2, "SYNTAX ERROR!\n"); 
    }
}
 
void IdFound(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    if (token->type == T_ASGN) {
    // =
        ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW);
        insertRight(ptr, asgnFound);
        token = get_token(buffer);
    // P_EXPRESSION
        ASTNode *expressionFound = ruleNode(P_EXPRESSION);
        insertLeft(asgnFound, expressionFound);
        Expression(token, expressionFound, buffer);
        if (expressionFound->right == NULL) {
            freeAST(ASTRoot); 
            error_exit(2, "SYNTAX ERROR!\n"); 
        }
    // ;
        ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
        insertLeft(expressionFound, semiColonFound);
        token = get_token(buffer);
    }
    else if (token->type == T_LBRACKET) {
    // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
        insertRight(ptr, lParenFound);
        token = get_token(buffer);
    
    // P_EXPRESSION_LIST
        ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
        insertLeft(lParenFound, expressionListRule);
        ExpressionList(token, expressionListRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        insertLeft(expressionListRule, rParenFound);
        token = get_token(buffer);
    // ;
        ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
        insertLeft(rParenFound, semiColonFound);
        token = get_token(buffer);
    } 
    else if (token->type == T_DOT) {
    // . 
        ASTNode *dotFound = checkToken(token, T_DOT, NO_KW);
        insertRight(ptr, dotFound);
        token = get_token(buffer);
    // ID 
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertLeft(dotFound, idFound);
        token = get_token(buffer);
    // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
        insertLeft(idFound, lParenFound);
        token = get_token(buffer);
    // P_EXPRESSION_LIST
        ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
        insertLeft(lParenFound, expressionListRule);
        ExpressionList(token, expressionListRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        insertLeft(expressionListRule, rParenFound);
        token = get_token(buffer);
    // ;
        ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
        insertLeft(rParenFound, semiColonFound);
        token = get_token(buffer);
    }
}

void ExpressionList(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    //P_EXPRESSION
    if (ASTNode *expressionFound == parseExpression(token, buffer) != NULL ) {
        ASTNode *expression = ruleNode(P_EXPRESSION);
        insertRight(ptr, expression);
        insertRight(expression, expressionFound);
    // ,
        if (token->type == T_COMMA) {
            ASTNode *exprCommaFound = ruleNode(P_COMMA_EXPR_FOUND);
            insertLeft(expressionFound, exprCommaFound);
            ExprCommaFound(token, exprCommaFound, buffer);
        }
    }  else token = get_token(buffer);
} 

void IfStatement(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    //IF
    ASTNode *ifFound = checkToken(token, T_KW, KW_IF);
    insertRight(ptr, ifFound);
    token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    insertLeft(ifFound, lParenFound);
    token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertLeft(lParenFound, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
        freeAST(ASTRoot); 
        error_exit(2, "SYNTAX ERROR!\n");
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(expressionRule, rParenFound);
    token = get_token(buffer);
    ASTNode *ifFoundRule = ruleNode(P_IF_FOUND);
    insertLeft(rParenFound, ifFoundRule);
    IfFound(token, ifFoundRule, buffer);
}

void IfFound(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    if (token->type == T_PIPE) {
    //P_OPTIONAL_VALUE
        ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
        insertRight(ptr, OptionalValueRule);
        OptionalValue(token, OptionalValueRule, buffer);
    //P_BLOCK
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertLeft(OptionalValueRule, BlockRule);
        Block(token, BlockRule, buffer);
    //P_ELSE_STATEMENT
        ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
        insertLeft(BlockRule, ElseRule);
        ElseStatement(token, ElseRule, buffer);
    }
    else if (token->type == T_LBRACE) {
    //P_BLOCK
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertLeft(ptr, BlockRule);
        Block(token, BlockRule, buffer);
    //P_ELSE_STATEMENT
        ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
        insertLeft(BlockRule, ElseRule);
        ElseStatement(token, ElseRule, buffer);
    }
}

void OptionalValue(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // | 
    ASTNode *lPipeFound = checkToken(token, T_PIPE, NO_KW);
    insertRight(ptr, lPipeFound);
    token = get_token(buffer);
    // ID
    ASTNode *idFound = checkToken(token,T_ID, NO_KW);
    insertLeft(lPipeFound, idFound);
    token = get_token(buffer);
    // | 
    ASTNode *rPipeFound = checkToken(token, T_PIPE, NO_KW);
    insertLeft(idFound, rPipeFound);
    token = get_token(buffer);
}
void ElseStatement(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // else 
    ASTNode *elseFound = checkToken(token, T_KW, KW_ELSE);
    insertRight(ptr, elseFound);
    token = get_token(buffer);
    // P_BLOCK
    ASTNode *BlockRule = ruleNode(P_BLOCK);
    insertLeft(elseFound, BlockRule);
    Block(token, BlockRule, buffer);
}

void ExprCommaFound(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ,
    ASTNode *commaFound = checkToken(token, T_COMMA, NO_KW);
    insertRight(ptr, commaFound);
    // P_EXPRESSION_LIST
    ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
    insertLeft(commaFound, expressionListRule);
    ExpressionList(token, expressionListRule, buffer);

}

void While(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    //while
    ASTNode *whileFound = checkToken(token, T_KW, KW_WHILE);
    insertRight(ptr, whileFound);
    token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    insertLeft(whileFound, lParenFound);
    token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertLeft(lParenFound, expressionRule);
    Expression(token, expressionRule, buffer);
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(expressionRule, rParenFound);
    token = get_token(buffer);
    //P_OPTIONAL_VALUE
    if (token->type == T_PIPE) {
        ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
        insertLeft(rParenFound, OptionalValueRule);
        OptionalValue(token, OptionalValueRule, buffer);
    //P_BLOCK
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertLeft(OptionalValueRule, BlockRule);
        Block(token, BlockRule, buffer);
    }
    //P_BLOCK
    else {
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertLeft(rParenFound, BlockRule);
        Block(token, BlockRule, buffer);
    }
}
void Return(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    //RETURN
    ASTNode *returnFound = checkToken(token, T_KW, KW_RETURN);
    insertRight(ptr, returnFound);
    token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertLeft(returnFound, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
        freeAST(ASTRoot); 
        error_exit(2, "SYNTAX ERROR!\n");
    }
    // ;
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
    insertLeft(expressionRule, semiColonFound);
    token = get_token(buffer);
}

void End(Token *token, ASTNode *ptr, circ_buff_ptr buffer) {
    ASTNode *eofFound = checkToken(token,T_EOF, NO_KW);
    insertRight(ptr, eofFound);
}
