#include "parser.h"
#include "exp_parser.h"
           
ASTNode * checkToken(Token **token, TokenType wantedType, KeyWordType wantedKeyWord) {
    ASTNode *ptr = NULL;
    if ((*token)->type == T_KW) {
        if ((*token)->value.keyword == wantedKeyWord) {
            ptr = nodeCreate(convertToASTType(T_KW, wantedKeyWord), *token);
            return ptr;
        }
    } else if ((*token)->type == wantedType) {
        ptr = nodeCreate(convertToASTType(wantedType, NO_KW), *token);
        return ptr;
    }
    free_token(*token);
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

void Parse(circ_buff_ptr buffer) {
    Token *token = NULL;
    ASTNode *progFound = ruleNode(P_PROG);
    ASTRoot = progFound;
    Prog(&token, progFound, buffer);
} 

void Prog(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // P_PROLOG
    ASTNode *prolog = ruleNode(P_PROLOG); // save string and id
    insertRight(ptr, prolog);
    *token = get_token(buffer);
    Prolog(token, prolog, buffer);
    // P_FUNCTION_DEF
    ASTNode *function_def = ruleNode(P_FUNCTION_DEF); 
    insertLeft(prolog, function_def);
    FunctionDef(token, function_def, buffer);
    ASTNode *end = ruleNode(P_END); 
    insertLeft(function_def, end);
    End(token, end);
}

void Prolog(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // CONST
    ASTNode *constFound = checkToken(token, T_KW, KW_CONST);
    freeAST(constFound);
    // ID 
    *token = get_token(buffer);
    ASTNode *idFound = checkToken(token, T_ID, NO_KW);
    insertRight(ptr, idFound);
    // =
    *token = get_token(buffer);
    ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW);
    freeAST(asgnFound);
    // @import
    *token = get_token(buffer);
    ASTNode *importFound = checkToken(token, T_AT_IMPORT, NO_KW);
    freeAST(importFound);
    // (
    *token = get_token(buffer);
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    freeAST(lParenFound);
    // P_EXPRESSION
    ASTNode *expression = ruleNode(P_EXPRESSION);
    insertLeft(idFound, expression);
    *token = get_token(buffer);
    Expression(token, expression, buffer);
    if (expression->right == NULL) {
        free_token(*token);
        freeAST(ASTRoot); 
        error_exit(2, "SYNTAX ERROR: Prolog (Expression)\n"); 
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    freeAST(rParenFound);
    // ;
    *token = get_token(buffer);
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
    freeAST(semiColonFound);
    *token = get_token(buffer);
}

void Expression(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    ASTNode *expressionFound = parseExpression(token, buffer);
    if (expressionFound != NULL) {
        insertRight(ptr, expressionFound);
    }
}

void FunctionDef(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // PUB
    ASTNode *pubFound = checkToken(token, T_KW, KW_PUB);
    freeAST(pubFound);
    // FN
    *token = get_token(buffer);
    ASTNode *fnFound = checkToken(token, T_KW, KW_FN);
    freeAST(fnFound);
    // ID
    *token = get_token(buffer);
    ASTNode *idFound = checkToken(token, T_ID, NO_KW);
    insertRight(ptr, idFound);
    // (
    *token = get_token(buffer);
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    insertLeft(idFound, lParenFound);
    // P_PARAM_LIST
    *token = get_token(buffer);
    ASTNode *rParenFound = NULL;
    if((*token)->type == T_ID) {
        ASTNode *paramList = ruleNode(P_PARAM_LIST);
        insertLeft(lParenFound,paramList);
        ParamList(token, paramList, buffer);
    // )
        rParenFound = checkToken(token, T_RPAREN, NO_KW);
        insertLeft(paramList, rParenFound);
        *token = get_token(buffer);
    } else {
    // )
    rParenFound = checkToken(token, T_RPAREN, NO_KW);
    insertLeft(lParenFound, rParenFound);
    *token = get_token(buffer);
    }
    // P_TYPE_COMPLETE
    ASTNode *function_type = ruleNode(P_FUNCTION_TYPE);
    insertLeft(rParenFound, function_type);
    FunctionType(token, function_type, buffer);
    // P_BLOCK
    ASTNode *block = ruleNode(P_BLOCK);
    insertLeft(function_type, block);
    Block(token, block, buffer);
    // RECURSIVELY CALL Function Def
    if ((*token)->type == T_KW && (*token)->value.keyword == KW_PUB) {
        ASTNode *function_def = ruleNode(P_FUNCTION_DEF);
        insertLeft(block, function_def);
        FunctionDef(token, function_def, buffer);
    }
}

void ParamList(Token **token, ASTNode *ptr, circ_buff_ptr buffer){
    // ID
    ASTNode *idFound = checkToken(token, T_ID, NO_KW);
    insertRight(ptr, idFound);
    // :
    *token = get_token(buffer);
    ASTNode *colonFound = checkToken(token, T_COLON, NO_KW);
    freeAST(colonFound);
    // P_TYPE_COMPLETE
    *token = get_token(buffer);
    ASTNode *typeComplete = ruleNode(P_TYPE_COMPLETE);
    insertLeft(idFound, typeComplete);
    TypeComplete(token, typeComplete, buffer);
    //P_COMMA_PAR_FOUND
    if ((*token)->type == T_COMMA) {
        ASTNode *commaFound = ruleNode(P_COMMA_PAR_FOUND);
        insertLeft (typeComplete, commaFound);
        CommaParFound(token, commaFound, buffer);
    }
}

void CommaParFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ,
    ASTNode *commaFound = checkToken(token, T_COMMA, NO_KW);
    freeAST(commaFound);
    // P_PARAM_LIST
    *token = get_token(buffer);
    if((*token)->type == T_ID) {
    ASTNode *paramList = ruleNode(P_PARAM_LIST);
    insertRight(ptr,paramList);
    ParamList(token, paramList, buffer);
    }
}

void TypeComplete(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ?
    if ((*token)->type == T_QMARK) {
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

void QMark(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ?
    ASTNode *qmarkFound = checkToken(token, T_QMARK, NO_KW);
    insertRight(ptr, qmarkFound);

    *token = get_token(buffer);
}

void Type(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // [
    if ((*token)->type == T_LBRACKET) {
        ASTNode *lBracketFound = checkToken(token, T_LBRACKET, NO_KW);
        freeAST(lBracketFound);
        *token = get_token(buffer);
    // ]
        ASTNode *rBracketFound = checkToken(token, T_RBRACKET, NO_KW);
        freeAST(rBracketFound);
        *token = get_token(buffer);
    // u8
        ASTNode *u8Found = checkToken(token, T_KW, KW_U8);
        insertRight(ptr, u8Found);
    }
    else {
        if ((*token)->type == T_KW && (*token)->value.keyword != KW_VOID) {
            if ((*token)->value.keyword == KW_I32) {
                ASTNode *i32Found = checkToken(token, T_KW, KW_I32);
                insertRight(ptr, i32Found);
            }
            if ((*token)->value.keyword == KW_F64) {
                ASTNode *f64Found = checkToken(token, T_KW, KW_F64);
                insertRight(ptr, f64Found);
            }
            if ((*token)->value.keyword == KW_BOOL) {
                ASTNode *boolFound = checkToken(token, T_KW, KW_BOOL);
                insertRight(ptr, boolFound);
            }
        }
        else {
            free_token(*token);
            freeAST(ASTRoot);
            error_exit(2, "SYNTAX ERROR!\n"); 
        }
    }
    *token = get_token(buffer);
}

void FunctionType(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // P_TYPE
    if((*token)->type == T_QMARK || (*token)->type == T_LBRACKET || ((*token)->type == T_KW && ((*token)->value.keyword == KW_I32 || (*token)->value.keyword == KW_F64 || (*token)->value.keyword == KW_BOOL) )) {
        ASTNode *type_complete = ruleNode(P_TYPE);
        insertRight(ptr, type_complete);
        TypeComplete(token, type_complete, buffer);
    }
    // VOID
    else if ((*token)->value.keyword == KW_VOID) {
                ASTNode *voidFound = checkToken(token, T_KW, KW_VOID);
                insertRight(ptr, voidFound);
                *token = get_token(buffer);
            }
    else {
        freeAST(ASTRoot);
        error_exit(2, "SYNTAX ERROR!\n"); 
    }
}

void Block(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // {
    ASTNode *lBraceFound = checkToken(token, T_LBRACE, NO_KW);
    freeAST(lBraceFound);
    *token = get_token(buffer);
    //P_STATEMENT
    ASTNode *statement = ruleNode(P_STATEMENT);
    insertRight(ptr, statement);
    Statement(token, statement, buffer);
    // }
    ASTNode *rBraceFound = checkToken(token, T_RBRACE, NO_KW);
    freeAST(rBraceFound);
    *token = get_token(buffer);
}

void Statement(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    ASTNode *statementRule = NULL;
    // P_VAR_DECLARATION
    if ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR)) {
        statementRule = ruleNode(P_VAR_DECLARATION);
        insertRight(ptr, statementRule);
        VarDeclaration(token, statementRule, buffer, true);
    } 
    // ID
    else if ((*token)->type == T_ID) {
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertRight(ptr, idFound);
    // P_ID_FOUND
        *token = get_token(buffer);
        statementRule = ruleNode(P_ID_FOUND);
        insertLeft(idFound, statementRule);
        IdFound(token, statementRule, buffer, true);
    }
    // P_IF_STATEMENT
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_IF ) {
        statementRule = ruleNode(P_IF_STATEMENT);
        insertRight(ptr, statementRule);
        IfStatement(token, statementRule, buffer);
    }
    // P_WHILE_STATEMENT
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_WHILE) {
        statementRule = ruleNode(P_WHILE_LOOP);
        insertRight(ptr, statementRule);
        While(token, statementRule, buffer);
    } 
    //P_RETURN_STATEMENT
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_RETURN) {
        statementRule = ruleNode(P_RETURN_STATEMENT);
        insertRight(ptr, statementRule);
        Return(token, statementRule, buffer);
    }
    // More statements
    if ((*token)->type == T_ID || ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR || (*token)->value.keyword == KW_WHILE || (*token)->value.keyword == KW_IF || (*token)->value.keyword == KW_RETURN))) {
        ASTNode *nextStatement = ruleNode(P_STATEMENT);
        insertLeft(statementRule, nextStatement);
        Statement(token, nextStatement, buffer);
    }
}

void VarDeclaration(Token **token, ASTNode *ptr, circ_buff_ptr buffer, bool semic) {
     // T_KW
    if ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR)) {
        ASTNode *node = NULL;
    // CONST
        if ((*token)->value.keyword == KW_CONST ) {
            node = checkToken(token, T_KW, KW_CONST);
            insertRight(ptr, node);
        }
    // VAR
        else {
            node = checkToken(token, T_KW, KW_VAR);
            insertRight(ptr, node);
        }
        *token = get_token(buffer);
    // ID
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertLeft(node, idFound);
        *token = get_token(buffer);
        ASTNode* asgnFound = NULL;
        // if we decide to include the datatype during variable declaration
        if((*token)->type == T_COLON) {
            // :
            ASTNode *colonFound = checkToken(token, T_COLON, NO_KW);
            freeAST(colonFound);
            *token = get_token(buffer);

            // P_TYPE_COMPLETE
            ASTNode *type_complete = ruleNode(P_TYPE_COMPLETE);
            insertLeft(idFound, type_complete);
            TypeComplete(token, type_complete, buffer);

            // =
            asgnFound = checkToken(token, T_ASGN, NO_KW);
            insertLeft(type_complete, asgnFound);
            *token = get_token(buffer);
        }
        else { 
            // =
            asgnFound = checkToken(token, T_ASGN, NO_KW);
            insertLeft(idFound, asgnFound);
            *token = get_token(buffer);
        }
    // P_ASGN_FOUND
        ASTNode *asgnFoundRule = ruleNode(P_ASGN_FOUND);
        insertRight(asgnFound, asgnFoundRule);
        AsgnFound(token, asgnFoundRule, buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    }
    else {
        free_token(*token);
        freeAST(ASTRoot);
        error_exit(2, "SYNTAX ERROR!\n");
    }
}
 
void IdFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer, bool semic) {
    if ((*token)->type == T_ASGN) {
    // =
        ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW);
        freeAST(asgnFound);
        *token = get_token(buffer);
    // P_ASGN_FOUND
        ASTNode *asgnFoundRule = ruleNode(P_ASGN_FOUND);
        insertRight(ptr, asgnFoundRule);
        AsgnFound(token, asgnFoundRule, buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    }
    else if ((*token)->type == T_LPAREN) {
    // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
        insertRight(ptr, lParenFound);
        *token = get_token(buffer);
    // P_EXPRESSION_LIST
        ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
        insertLeft(lParenFound, expressionListRule);
        ExpressionList(token, expressionListRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        insertLeft(expressionListRule, rParenFound);
        *token = get_token(buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    } 
    else if ((*token)->type == T_DOT) {
    // . 
        ASTNode *dotFound = checkToken(token, T_DOT, NO_KW);
        freeAST(dotFound);
        *token = get_token(buffer);
    // ID 
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertRight(ptr, idFound);
        *token = get_token(buffer);
    // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
        insertLeft(idFound, lParenFound);
        *token = get_token(buffer);
    // P_EXPRESSION_LIST
        ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
        insertLeft(lParenFound, expressionListRule);
        ExpressionList(token, expressionListRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        insertLeft(expressionListRule, rParenFound);
        *token = get_token(buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    }
    // :
    else if ((*token)->type == T_COLON) {
        ASTNode *colonFound = checkToken(token, T_COLON, NO_KW);
        freeAST(colonFound);
        *token = get_token(buffer);
    // P_WHILE_STATEMENT
        ASTNode* whileStatementRule = ruleNode(P_WHILE_LOOP);
        insertRight(ptr, whileStatementRule);
        While(token, whileStatementRule, buffer);
    }
}
void AsgnFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    if ((*token)->type == T_KW && (*token)->value.keyword == KW_IF) {
        //IF
        ASTNode *ifFound = checkToken(token, T_KW, KW_IF);
        freeAST(ifFound);
        *token = get_token(buffer);
        // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
        freeAST(lParenFound);
        *token = get_token(buffer);
        //P_EXPRESSION
        ASTNode *expressionRule = ruleNode(P_EXPRESSION);
        insertRight(ptr, expressionRule);
        Expression(token, expressionRule, buffer);
        if (expressionRule->right == NULL) {
            free_token(*token);
            freeAST(ASTRoot);
            error_exit(2, "SYNTAX ERROR1!\n");
        }
        // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        freeAST(rParenFound);
        *token = get_token(buffer);
        // P_EXPRESSION
        ASTNode *expressionRule2 = ruleNode(P_EXPRESSION);
        insertLeft(expressionRule, expressionRule2);
        Expression(token, expressionRule2, buffer);
        if (expressionRule2->right == NULL) {
            free_token(*token);
            freeAST(ASTRoot);
            error_exit(2, "SYNTAX ERROR2!\n");
        }
        // ELSE
        ASTNode *elseFound = checkToken(token, T_KW, KW_ELSE);
        freeAST(elseFound);
        *token = get_token(buffer);
        // P_EXPRESSION
        ASTNode *expressionRule3 = ruleNode(P_EXPRESSION);
        insertLeft(expressionRule2, expressionRule3);
        Expression(token, expressionRule3, buffer);
        if (expressionRule3->right == NULL) {
            free_token(*token);
            freeAST(ASTRoot);
            error_exit(2, "SYNTAX ERROR3!\n");
        }
    }
    // P_EXPRESSION
    else {
        ASTNode *expressionFound = ruleNode(P_EXPRESSION);
        insertRight(ptr, expressionFound);
        Expression(token, expressionFound, buffer);
        if (expressionFound->right == NULL) {
            free_token(*token);
            freeAST(ASTRoot); 
            error_exit(2, "SYNTAX ERROR!\n"); 
        }
    }
}
void ExpressionList(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    //P_EXPRESSION
    ASTNode *expressionFound = parseExpression(token, buffer);
    if (expressionFound != NULL ) {
        ASTNode *expression = ruleNode(P_EXPRESSION);
        insertRight(ptr, expression);
        insertRight(expression, expressionFound);
    // ,
        if ((*token)->type == T_COMMA) {
            ASTNode *exprCommaFound = ruleNode(P_COMMA_EXPR_FOUND);
            insertLeft(expression, exprCommaFound);
            ExprCommaFound(token, exprCommaFound, buffer);
        }
    }
} 

void IfStatement(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    //IF
    ASTNode *ifFound = checkToken(token, T_KW, KW_IF);
    freeAST(ifFound);
    *token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    freeAST(lParenFound);
    *token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
        free_token(*token);
        freeAST(ASTRoot);
        error_exit(2, "SYNTAX ERROR!\n");
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    freeAST(rParenFound);
    *token = get_token(buffer);
    // P_IF_FOUND
    ASTNode *ifFoundRule = ruleNode(P_IF_FOUND);
    insertLeft(expressionRule, ifFoundRule);
    IfFound(token, ifFoundRule, buffer);
}

void IfFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    if ((*token)->type == T_PIPE) {
    //P_OPTIONAL_VALUE
        ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
        insertRight(ptr, OptionalValueRule);
        OptionalValue(token, OptionalValueRule, buffer);
    //P_BLOCK
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertLeft(OptionalValueRule, BlockRule);
        Block(token, BlockRule, buffer);
    //P_ELSE_STATEMENT
        if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
            ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
            insertLeft(BlockRule, ElseRule);
            ElseStatement(token, ElseRule, buffer);
        }
    }

    else if ((*token)->type == T_LBRACE) {
    //P_BLOCK
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertRight(ptr, BlockRule);
        Block(token, BlockRule, buffer);
    //P_ELSE_STATEMENT
        if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
            ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
            insertLeft(BlockRule, ElseRule);
            ElseStatement(token, ElseRule, buffer);
        }
    }

    else {
        free_token(*token);
        freeAST(ASTRoot);
        error_exit(2, "SYNTAX ERROR!\n");
    }
}
void SingleStatement(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    ASTNode *statementRule = NULL;
    // P_VAR_DECLARATION
    if ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR)) {
        statementRule = ruleNode(P_VAR_DECLARATION);
        insertRight(ptr, statementRule);
        VarDeclaration(token, statementRule, buffer, true);
    } 
    // ID
    else if ((*token)->type == T_ID) {
        ASTNode *idFound = checkToken(token, T_ID, NO_KW);
        insertRight(ptr, idFound);
    // P_ID_FOUND
        *token = get_token(buffer);
        statementRule = ruleNode(P_ID_FOUND);
        insertLeft(idFound, statementRule);
        IdFound(token, statementRule, buffer, true);
    }
    // P_IF_STATEMENT
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_IF ) {
        statementRule = ruleNode(P_IF_STATEMENT);
        insertRight(ptr, statementRule);
        IfStatement(token, statementRule, buffer);
    }
    // P_WHILE_STATEMENT
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_WHILE) {
        statementRule = ruleNode(P_WHILE_LOOP);
        insertRight(ptr, statementRule);
        While(token, statementRule, buffer);
    } 
    //P_RETURN_STATEMENT
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_RETURN) {
        statementRule = ruleNode(P_RETURN_STATEMENT);
        insertRight(ptr, statementRule);
        Return(token, statementRule, buffer);
    }
    // More statements
    if ((*token)->type == T_ID || ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR || (*token)->value.keyword == KW_WHILE || (*token)->value.keyword == KW_IF || (*token)->value.keyword == KW_RETURN))) {
        ASTNode *nextStatement = ruleNode(P_STATEMENT);
        insertLeft(statementRule, nextStatement);
        Statement(token, nextStatement, buffer);
    }
}

void OptionalValue(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // | 
    ASTNode *lPipeFound = checkToken(token, T_PIPE, NO_KW);
    freeAST(lPipeFound);
    *token = get_token(buffer);
    // ID
    ASTNode *idFound = checkToken(token,T_ID, NO_KW);
    insertRight(ptr, idFound);
    *token = get_token(buffer);
    // |
    ASTNode *rPipeFound = checkToken(token, T_PIPE, NO_KW);
    freeAST(rPipeFound);
    *token = get_token(buffer);
}
void ElseStatementWhile(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ELSE
    ASTNode *elseFound = checkToken(token, T_KW, KW_ELSE);
    freeAST(elseFound);
    *token = get_token(buffer);
    // P_BLOCK
    ASTNode *BlockRule = ruleNode(P_BLOCK);
    insertRight(ptr, BlockRule);
    Block(token, BlockRule, buffer);
}
void ElseStatement(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ELSE 
    ASTNode *elseFound = checkToken(token, T_KW, KW_ELSE);
    freeAST(elseFound);
    *token = get_token(buffer);
    //IF
    if ((*token)->type == T_KW && (*token)->value.keyword == KW_IF) {
        ASTNode *ifFound = ruleNode(P_IF_STATEMENT);
        insertRight(ptr, ifFound);
        IfStatement(token, ifFound, buffer);
    }
    // P_BLOCK
    else {
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertRight(ptr, BlockRule);
        Block(token, BlockRule, buffer);
    }
    
}

void ExprCommaFound(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // ,
    ASTNode *commaFound = checkToken(token, T_COMMA, NO_KW);
    freeAST(commaFound);
    *token = get_token(buffer);
    // P_EXPRESSION_LIST
    ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
    insertLeft(ptr, expressionListRule);
    ExpressionList(token, expressionListRule, buffer);
}

void While(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // while
    ASTNode *whileFound = checkToken(token, T_KW, KW_WHILE);
    freeAST(whileFound);
    *token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW);
    freeAST(lParenFound);
    *token = get_token(buffer);
    // P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
    freeAST(rParenFound);
    *token = get_token(buffer);
    // P_OPTIONAL_VALUE
    if ((*token)->type == T_PIPE) {
        ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
        insertLeft(expressionRule, OptionalValueRule);
        OptionalValue(token, OptionalValueRule, buffer);

        if ((*token)->type == T_LPAREN) {
    // P_OPTIONAL_STATEMENTS
            ASTNode *optionalStatementsRule = ruleNode(P_OPTIONAL_STATEMENTS);
            insertLeft(OptionalValueRule, optionalStatementsRule);
            OptionalStatements(token, optionalStatementsRule, buffer);
    // P_BLOCK
            ASTNode *BlockRule = ruleNode(P_BLOCK);
            insertLeft(optionalStatementsRule, BlockRule);
            Block(token, BlockRule, buffer);
    // P_ELSE 
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(BlockRule, ElseRule);
                ElseStatementWhile(token, ElseRule, buffer);
            }
        }
    // P_BLOCK
        else {
            ASTNode *BlockRule = ruleNode(P_BLOCK);
            insertLeft(OptionalValueRule, BlockRule);
            Block(token, BlockRule, buffer);
    // P_ELSE
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(BlockRule, ElseRule);
                ElseStatementWhile(token, ElseRule, buffer);
            }
        }
    }
    // P_BLOCK
    else {
    // P_OPTIONAL_STATEMENTS
        if ((*token)->type == T_LPAREN) {
            ASTNode *optionalStatementsRule = ruleNode(P_OPTIONAL_STATEMENTS);
            insertLeft(expressionRule, optionalStatementsRule);
            OptionalStatements(token, optionalStatementsRule, buffer);
    // P_BLOCK
            ASTNode *BlockRule = ruleNode(P_BLOCK);
            insertLeft(optionalStatementsRule, BlockRule);
            Block(token, BlockRule, buffer);
    // P_ELSE
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(BlockRule, ElseRule);
                ElseStatementWhile(token, ElseRule, buffer);
            }
        }
    // P_BLOCK
        else {
            ASTNode *BlockRule = ruleNode(P_BLOCK);
            insertLeft(expressionRule, BlockRule);
            Block(token, BlockRule, buffer);
    // P_ELSE
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(BlockRule, ElseRule);
                ElseStatementWhile(token, ElseRule, buffer);
            }
        }
    }
}

void OptionalStatements(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    // (
    ASTNode *lParenFound = checkToken(token,T_LPAREN,NO_KW);
    freeAST(lParenFound);
    *token = get_token(buffer);
    // single statement
    if ((*token)->type == T_ID || ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR || (*token)->value.keyword == KW_WHILE || (*token)->value.keyword == KW_IF || (*token)->value.keyword == KW_RETURN))) {
        ASTNode *singleStatementRule = ruleNode(P_SINGLE_STATEMENT);
        insertRight(ptr, singleStatementRule);
        SingleStatement(token, singleStatementRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        freeAST(rParenFound);
        *token = get_token(buffer);
    }
    // P_BLOCK
    else {
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertRight(ptr, BlockRule);
        Block(token, BlockRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW);
        freeAST(rParenFound);
        *token = get_token(buffer);
    }
}


void Return(Token **token, ASTNode *ptr, circ_buff_ptr buffer) {
    //RETURN
    ASTNode *returnFound = checkToken(token, T_KW, KW_RETURN);
    freeAST(returnFound);
    *token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
        free_token(*token);
        freeAST(ASTRoot); 
        error_exit(2, "SYNTAX ERROR!\n");
    }
    // ;
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW);
    freeAST(semiColonFound);
    *token = get_token(buffer);
}

void End(Token **token, ASTNode *ptr) {
    ASTNode *eofFound = checkToken(token,T_EOF, NO_KW);
    insertRight(ptr, eofFound);
}
