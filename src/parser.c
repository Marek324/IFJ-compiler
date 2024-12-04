#include "parser.h"
#include "exp_parser.h"
#include "sem_anal.h"

void Free(Token **token) {
    symtable_dispose(&SymFunctionTree);
    free_token(*token);
    freeAST(ASTRoot);
}
ASTNode *checkToken(Token **token, TokenType wantedType, KeyWordType wantedKeyWord, const char *error) {
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
    Free(token);
    error_exit(2, "%s\n", error); 
    return NULL;
}

ASTNode *ruleNode(ASTNodeType rule) {
   ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(99, "Malloc failed\n");
    }
    node->type = rule;
    node->left = NULL;
    node->right = NULL;
    node->token = NULL;
    return node;
}
void Parse(c_buff_ptr buffer) {
    Token *token = NULL;
    ASTNode *progFound = ruleNode(P_PROG);
    ASTRoot = progFound;
    Prog(&token, progFound, buffer);
}
void Prog(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // P_PROLOG
    ASTNode *prolog = ruleNode(P_PROLOG); // save string and id
    insertRight(ptr, prolog);
    *token = get_token(buffer);
    Prolog(token, prolog, buffer);
    // P_FUNCTION_DEF
    if ((*token)->type != T_EOF){
        ASTNode *function_def = ruleNode(P_FUNCTION_DEF); 
        insertLeft(prolog, function_def);
        FunctionDef(token, function_def, buffer);
    // P_END
        ASTNode *end = ruleNode(P_END); 
        insertLeft(function_def, end);
        End(token, end);
    }
    else {
        ASTNode *end = ruleNode(P_END); 
        insertLeft(prolog, end);
        End(token, end);
    }

}
void Prolog(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // CONST
    ASTNode *constFound = checkToken(token, T_KW, KW_CONST,"SYNTAX ERROR: Prolog expected CONST");
    freeAST(constFound);
    // ID 
    *token = get_token(buffer);
    ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: Prolog expected ID");
    if (strcmp(idFound->token->value.string_value, "ifj") == 0 ) {
        freeAST(idFound);
    }
    else {
        Free(token);
        error_exit(2,"SYNTAX ERROR: WRONG PROLOG ID");
    }
    // =
    *token = get_token(buffer);
    ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW, "SYNTAX ERROR: Prolog expected =");
    freeAST(asgnFound);
    // @import
    *token = get_token(buffer);
    ASTNode *importFound = checkToken(token, T_AT_IMPORT, NO_KW, "SYNTAX ERROR: Prolog expected @import");
    freeAST(importFound);
    // (
    *token = get_token(buffer);
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: Prolog expected (");
    freeAST(lParenFound);
    // P_EXPRESSION
    ASTNode *expression = ruleNode(P_EXPRESSION);
    insertLeft(ptr, expression);
    *token = get_token(buffer);
    Expression(token, expression, buffer);
    if (expression->right == NULL) {
        Free(token);
        error_exit(2, "SYNTAX ERROR: Prolog (Expression)\n"); 
    }
    if (strcmp(expression->right->token->value.string_value, "ifj24.zig") == 0 ) {
        freeAST(expression);
        expression = NULL;
    }
    else {
        Free(token);
        error_exit(2,"SYNTAX ERROR: HAS TO BE ifj24.zig");
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: Prolog expected )");
    freeAST(rParenFound);
    // ;
    *token = get_token(buffer);
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: Prolog expected ;");
    freeAST(semiColonFound);
    *token = get_token(buffer);
}
void Expression(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    ASTNode *expressionFound = parseExpression(token, buffer);
    if (expressionFound != NULL) {
        insertRight(ptr, expressionFound);
    }
}

void FunctionDef(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // PUB
    ASTNode *pubFound = checkToken(token, T_KW, KW_PUB, "SYNTAX ERROR: FuncDef expected PUB");
    freeAST(pubFound);
    // FN
    *token = get_token(buffer);
    ASTNode *fnFound = checkToken(token, T_KW, KW_FN, "SYNTAX ERROR: FuncDef expected FN");
    freeAST(fnFound);
    // ID
    *token = get_token(buffer);
    ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: FuncDef expected ID");
    insertRight(ptr, idFound);
    // insert function into symtable
    symtable_insert(&SymFunctionTree, idFound->token->value.string_value, T_FUN_SYM);
    // (
    *token = get_token(buffer);
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: FuncDef expected (");
    insertLeft(idFound, lParenFound);
    // P_PARAM_LIST
    *token = get_token(buffer);
    ASTNode *rParenFound = NULL;
    if((*token)->type == T_ID) {
        ASTNode *paramList = ruleNode(P_PARAM_LIST);
        insertLeft(lParenFound, paramList);
        ParamList(token, paramList, buffer);
    // insert function info into symtable
        getFunctionParamInfo(SymFunctionTree, idFound->token->value.string_value, paramList, DEFAULT_INDEX_SIZE, DEFAULT_FUNCTION_PARAM_SIZE);
    // )
        rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: FuncDef expected )");
        insertLeft(paramList, rParenFound);
        *token = get_token(buffer);
    } else {
    // )
    rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: FuncDef expected )");
    insertLeft(lParenFound, rParenFound);
    *token = get_token(buffer);
    }
    // P_FUNCTION_TYPE
    ASTNode *function_type = ruleNode(P_FUNCTION_TYPE);
    insertLeft(rParenFound, function_type);
    FunctionType(token, function_type, buffer);
    // insert function type into symtable
    getFunctionType(SymFunctionTree, idFound->token->value.string_value, function_type);
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
void ParamList(Token **token, ASTNode *ptr, c_buff_ptr buffer){
    // ID
    ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: ParamList expected ID");
    insertRight(ptr, idFound);
    // :
    *token = get_token(buffer);
    ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: ParamList expected :");
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
void CommaParFound(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // ,
    ASTNode *commaFound = checkToken(token, T_COMMA, NO_KW, "SYNTAX ERROR: CommaParFound expected ,");
    freeAST(commaFound);
    // P_PARAM_LIST
    *token = get_token(buffer);
    if((*token)->type == T_ID) {
    ASTNode *paramList = ruleNode(P_PARAM_LIST);
    insertRight(ptr,paramList);
    ParamList(token, paramList, buffer);
    }
}
void TypeComplete(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
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
void QMark(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    (void) ptr;
    // ?
    ASTNode *qmarkFound = checkToken(token, T_QMARK, NO_KW, "SYNTAX ERROR: QMark expected ?");
    freeAST(qmarkFound);
    *token = get_token(buffer);
}
void Type(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // [
    if ((*token)->type == T_LBRACKET) {
        ASTNode *lBracketFound = checkToken(token, T_LBRACKET, NO_KW, "SYNTAX ERROR: Type expected [");
        freeAST(lBracketFound);
        *token = get_token(buffer);
    // ]
        ASTNode *rBracketFound = checkToken(token, T_RBRACKET, NO_KW,"SYNTAX ERROR: Type expected ]" );
        freeAST(rBracketFound);
        *token = get_token(buffer);
    // u8
        ASTNode *u8Found = checkToken(token, T_KW, KW_U8, "SYNTAX ERROR: Type expected u8");
        insertRight(ptr, u8Found);
    }
    else {
        if ((*token)->type == T_KW && (*token)->value.keyword != KW_VOID && ((*token)->value.keyword == KW_I32 || (*token)->value.keyword == KW_F64 || (*token)->value.keyword == KW_BOOL)) {
            if ((*token)->value.keyword == KW_I32) {
                ASTNode *i32Found = checkToken(token, T_KW, KW_I32, "SYNTAX ERROR: Type expected i32");
                insertRight(ptr, i32Found);
            }
            if ((*token)->value.keyword == KW_F64) {
                ASTNode *f64Found = checkToken(token, T_KW, KW_F64, "SYNTAX ERROR: Type expected f64");
                insertRight(ptr, f64Found);
            }
            if ((*token)->value.keyword == KW_BOOL) {
                ASTNode *boolFound = checkToken(token, T_KW, KW_BOOL, "SYNTAX ERROR: Type expected bool");
                insertRight(ptr, boolFound);
            }
        }
        else {
            Free(token);
            error_exit(2, "SYNTAX ERROR: Type unknown type\n"); 
        }
    }
    *token = get_token(buffer);
}
void FunctionType(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // P_TYPE
    if((*token)->type == T_QMARK || (*token)->type == T_LBRACKET || ((*token)->type == T_KW && ((*token)->value.keyword == KW_I32 || (*token)->value.keyword == KW_F64 || (*token)->value.keyword == KW_BOOL) )) {
        ASTNode *type_complete = ruleNode(P_TYPE_COMPLETE);
        insertRight(ptr, type_complete);
        TypeComplete(token, type_complete, buffer);
    }
    // VOID
    else if ((*token)->value.keyword == KW_VOID) {
                ASTNode *voidFound = checkToken(token, T_KW, KW_VOID, "SYNTAX ERROR: FuncType expected void");
                insertRight(ptr, voidFound);
                *token = get_token(buffer);
            }
    else {
        Free(token);
        error_exit(2, "SYNTAX ERROR: Unknown func type\n"); 
    }
}
void Block(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // {
    ASTNode *lBraceFound = checkToken(token, T_LBRACE, NO_KW, "SYNTAX ERROR: Block expected {");
    freeAST(lBraceFound);
    *token = get_token(buffer);
    //P_STATEMENT
    ASTNode *statement = ruleNode(P_STATEMENT);
    insertRight(ptr, statement);
    Statement(token, statement, buffer);
    // }
    ASTNode *rBraceFound = checkToken(token, T_RBRACE, NO_KW, "SYNTAX ERROR: Block expected }");
    freeAST(rBraceFound);
    *token = get_token(buffer);
}
void Statement(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    ASTNode *statementRule = NULL;
    // P_VAR_DECLARATION
    if ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR)) {
        statementRule = ruleNode(P_VAR_DECLARATION);
        insertRight(ptr, statementRule);
        VarDeclaration(token, statementRule, buffer, true);
    } 
    // ID
    else if ((*token)->type == T_ID) {
        ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: Statement expected ID");
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
    //P_BREAK
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_BREAK) {
        statementRule = ruleNode(P_BREAK);
        insertRight(ptr, statementRule);
        Break(token, statementRule, buffer);
    }
    //P_CONTINUE
    else if ((*token)->type == T_KW &&  (*token)->value.keyword == KW_CONTINUE) {
        statementRule = ruleNode(P_CONTINUE);
        insertRight(ptr, statementRule);
        Continue(token, statementRule, buffer);
    }
    //P_FOR_LOOP
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_FOR) {
        statementRule = ruleNode(P_FOR_LOOP);
        insertRight(ptr, statementRule);
        For(token, statementRule, buffer);
    }
    
    // More statements
    if ((*token)->type == T_ID || ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR || (*token)->value.keyword == KW_WHILE || (*token)->value.keyword == KW_IF || (*token)->value.keyword == KW_RETURN || (*token)->value.keyword == KW_BREAK || (*token)->value.keyword == KW_CONTINUE || (*token)->value.keyword == KW_FOR))) {
        ASTNode *nextStatement = ruleNode(P_STATEMENT);
        insertLeft(statementRule, nextStatement);
        Statement(token, nextStatement, buffer);
    }
}
void Continue(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    (void) ptr;
    // continue
    ASTNode *continueFound = checkToken(token, T_KW, KW_CONTINUE, "SYNTAX ERROR: Continue expected continue");
    freeAST(continueFound);
    *token = get_token(buffer);
    //:
    if ((*token)->type == T_COLON) {
        ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: Continue expected :");
        freeAST(colonFound);
        *token = get_token(buffer);
    // ID 
        ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: Continue expected ID");
        insertRight(ptr, idFound);
        *token = get_token(buffer);
    }
    // ;
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: Continue expected ;");
    freeAST(semiColonFound);
    *token = get_token(buffer);
}
void Break(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    (void) ptr;
    // break
    ASTNode *breakFound = checkToken(token, T_KW, KW_BREAK, "SYNTAX ERROR: Break expected break");
    freeAST(breakFound);
    *token = get_token(buffer);
    //:
    if ((*token)->type == T_COLON) {
        ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: Break expected :");
        freeAST(colonFound);
        *token = get_token(buffer);
    // ID 
        ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: Break expected ID");
        insertRight(ptr, idFound);
        *token = get_token(buffer);
    }
    // ;
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: Break expected ;");
    freeAST(semiColonFound);
    *token = get_token(buffer);
}
void VarDeclaration(Token **token, ASTNode *ptr, c_buff_ptr buffer, bool semic) {
     // T_KW
    if ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR)) {
        ASTNode *node = NULL;
    // CONST
        if ((*token)->value.keyword == KW_CONST ) {
            node = checkToken(token, T_KW, KW_CONST, "SYNTAX ERROR: VarDec expected CONST");
            insertRight(ptr, node);
        }
    // VAR
        else {
            node = checkToken(token, T_KW, KW_VAR, "SYNTAX ERROR: VarDec expected VAR");
            insertRight(ptr, node);
        }
        *token = get_token(buffer);
    // ID
        ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: VarDec expected ID");
        insertLeft(node, idFound);
        *token = get_token(buffer);
        ASTNode* asgnFound = NULL;
        // if we decide to include the datatype during variable declaration
        if((*token)->type == T_COLON) {
            // :
            ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: VarDec expected :");
            freeAST(colonFound);
            *token = get_token(buffer);

            // P_TYPE_COMPLETE
            ASTNode *type_complete = ruleNode(P_TYPE_COMPLETE);
            insertLeft(idFound, type_complete);
            TypeComplete(token, type_complete, buffer);

            // =
            asgnFound = checkToken(token, T_ASGN, NO_KW, "SYNTAX ERROR: VarDec expected =");
            insertLeft(type_complete, asgnFound);
            *token = get_token(buffer);
        }
        else { 
            // =
            asgnFound = checkToken(token, T_ASGN, NO_KW, "SYNTAX ERROR: VarDec expected =");
            insertLeft(idFound, asgnFound);
            *token = get_token(buffer);
        }
    // P_ASGN_FOUND
        ASTNode *asgnFoundRule = ruleNode(P_ASGN_FOUND);
        insertRight(asgnFound, asgnFoundRule);
        AsgnFound(token, asgnFoundRule, buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: VarDec expected ;");
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    }
    else {
        Free(token);
        error_exit(2, "SYNTAX ERROR: NOT CONST OR VAR\n");
    }
}
void IdFound(Token **token, ASTNode *ptr, c_buff_ptr buffer, bool semic) {
    if ((*token)->type == T_ASGN) {
    // =
        ASTNode *asgnFound = checkToken(token, T_ASGN, NO_KW, "SYNTAX ERROR: IdFound expected =");
        freeAST(asgnFound);
        *token = get_token(buffer);
    // P_ASGN_FOUND
        ASTNode *asgnFoundRule = ruleNode(P_ASGN_FOUND);
        insertRight(ptr, asgnFoundRule);
        AsgnFound(token, asgnFoundRule, buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: IdFound expected ;");
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    }
    else if ((*token)->type == T_LPAREN) {
    // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: IdFound expected (");
        insertRight(ptr, lParenFound);
        *token = get_token(buffer);
    // P_EXPRESSION_LIST
        ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
        insertLeft(lParenFound, expressionListRule);
        ExpressionList(token, expressionListRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: IdFound expected )");
        insertLeft(expressionListRule, rParenFound);
        *token = get_token(buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: IdFound expected ;");
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    } 
    else if ((*token)->type == T_DOT) {
    // . 
        ASTNode *dotFound = checkToken(token, T_DOT, NO_KW, "SYNTAX ERROR: IdFound expected .");
        freeAST(dotFound);
        *token = get_token(buffer);
    // ID 
        ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: IdFound expected ID");
        insertRight(ptr, idFound);
        *token = get_token(buffer);
    // (
        ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: IdFound expected (");
        insertLeft(idFound, lParenFound);
        *token = get_token(buffer);
    // P_EXPRESSION_LIST
        ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
        insertLeft(lParenFound, expressionListRule);
        ExpressionList(token, expressionListRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: IdFound expected )");
        insertLeft(expressionListRule, rParenFound);
        *token = get_token(buffer);
    // ;
        if (semic) { 
            ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: IdFound expected ;");
            freeAST(semiColonFound);
            *token = get_token(buffer);
        }
    }
    // :
    else if ((*token)->type == T_COLON) {
        ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: IdFound expected :");
        freeAST(colonFound);
        *token = get_token(buffer);
    // P_WHILE_STATEMENT
        ASTNode* whileStatementRule = ruleNode(P_WHILE_LOOP);
        insertRight(ptr, whileStatementRule);
        While(token, whileStatementRule, buffer);
    }
}
void AsgnFound(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // P_EXPRESSION
    ASTNode *expressionFound = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionFound);
    Expression(token, expressionFound, buffer);
    if (expressionFound->right == NULL) {
        Free(token);
        error_exit(2, "SYNTAX ERROR assigning wrong expression!\n"); 
    }
}

void ExpressionList(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
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
void IfStatement(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    //IF
    ASTNode *ifFound = checkToken(token, T_KW, KW_IF, "SYNTAX ERROR: IfStatement expected IF");
    freeAST(ifFound);
    *token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: IfStatement expected (");
    freeAST(lParenFound);
    *token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
        Free(token);
        error_exit(2, "SYNTAX ERROR wrong if condition!\n");
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: IfStatement expected )");
    freeAST(rParenFound);
    *token = get_token(buffer);
    // P_IF_FOUND
    ASTNode *ifFoundRule = ruleNode(P_IF_FOUND);
    insertLeft(expressionRule, ifFoundRule);
    IfFound(token, ifFoundRule, buffer);
}
void IfFound(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    if ((*token)->type == T_PIPE) {
    //P_OPTIONAL_VALUE
        ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
        insertRight(ptr, OptionalValueRule);
        OptionalValue(token, OptionalValueRule, buffer);
    // P_BLOCK
        ASTNode *blockRule = ruleNode(P_BLOCK);
        insertLeft(OptionalValueRule, blockRule);
        Block(token, blockRule, buffer);
    //P_ELSE_STATEMENT
        if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
            ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
            insertLeft(blockRule, ElseRule);
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
        Free(token);
        error_exit(2, "SYNTAX ERROR: after if condition\n");
    }
}
void SingleStatement(Token **token, ASTNode *ptr, c_buff_ptr buffer, bool semic) {
    ASTNode *statementRule = NULL;
    // P_VAR_DECLARATION
    if ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR)) {
        statementRule = ruleNode(P_VAR_DECLARATION);
        insertRight(ptr, statementRule);
        VarDeclaration(token, statementRule, buffer, semic);
    } 
    // ID
    else if ((*token)->type == T_ID) {
        ASTNode *idFound = checkToken(token, T_ID, NO_KW, "SYNTAX ERROR: SingleStatement expected ID");
        insertRight(ptr, idFound);
    // P_ID_FOUND
        *token = get_token(buffer);
        statementRule = ruleNode(P_ID_FOUND);
        insertLeft(idFound, statementRule);
        IdFound(token, statementRule, buffer, semic);
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
    //P_CONTINUE
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_CONTINUE) {
        statementRule = ruleNode(P_CONTINUE);
        insertRight(ptr, statementRule);
        Continue(token, statementRule, buffer);
    }
    //P_BREAK
    else if ((*token)->type == T_KW &&  (*token)->value.keyword == KW_BREAK) {
        statementRule = ruleNode(P_BREAK);
        insertRight(ptr, statementRule);
        Break(token, statementRule, buffer);
    }
    //P_FOR_LOOP
    else if ((*token)->type == T_KW && (*token)->value.keyword == KW_FOR) {
        statementRule = ruleNode(P_FOR_LOOP);
        insertRight(ptr, statementRule);
        For(token, statementRule, buffer);
    }
    else {
        Free(token);
        error_exit(2, "SYNTAX ERROR not a statement!\n");
    }
    
}
void OptionalValue(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // | 
    ASTNode *lPipeFound = checkToken(token, T_PIPE, NO_KW, "SYNTAX ERROR: OptionalValue expected |");
    freeAST(lPipeFound);
    *token = get_token(buffer);
    // ID
    ASTNode *idFound = checkToken(token,T_ID, NO_KW, "SYNTAX ERROR: OptionalValue expected ID");
    insertRight(ptr, idFound);
    *token = get_token(buffer);
    // |
    ASTNode *rPipeFound = checkToken(token, T_PIPE, NO_KW, "SYNTAX ERROR: OptionalValue expected |");
    freeAST(rPipeFound);
    *token = get_token(buffer);
}
void ElseStatement(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // ELSE 
    ASTNode *elseFound = checkToken(token, T_KW, KW_ELSE, "SYNTAX ERROR: ElseStatement expected ELSE");
    freeAST(elseFound);
    *token = get_token(buffer);
    if ((*token)->type == T_KW && (*token)->value.keyword == KW_IF) {
        ASTNode *ifStatementRule = ruleNode(P_IF_STATEMENT);
        insertRight(ptr, ifStatementRule);
        IfStatement(token, ifStatementRule, buffer);
    }
    else {
    // P_BLOCK  
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertRight(ptr, BlockRule);
        Block(token, BlockRule, buffer);    
    }
}
void ExprCommaFound(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // ,
    ASTNode *commaFound = checkToken(token, T_COMMA, NO_KW, "SYNTAX ERROR: ExprCommaFound expected ,");
    freeAST(commaFound);
    *token = get_token(buffer);
    // P_EXPRESSION_LIST
    ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
    insertLeft(ptr, expressionListRule);
    ExpressionList(token, expressionListRule, buffer);
}
void While(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // while
    ASTNode *whileFound = checkToken(token, T_KW, KW_WHILE, "SYNTAX ERROR: While expected while");
    freeAST(whileFound);
    *token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: While expected (");
    freeAST(lParenFound);
    *token = get_token(buffer);
    // P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
            Free(token);
            error_exit(2, "SYNTAX ERROR wrong while condition!\n"); 
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: While expected )");
    freeAST(rParenFound);
    *token = get_token(buffer);
    // P_OPTIONAL_VALUE
    if ((*token)->type == T_PIPE) {
        ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
        insertLeft(expressionRule, OptionalValueRule);
        OptionalValue(token, OptionalValueRule, buffer);
    //:
        if ((*token)->type == T_COLON) {
            ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: While expected :");
            freeAST(colonFound);
            *token = get_token(buffer);
    // P_OPTIONAL_STATEMENTS
            ASTNode *optionalStatementsRule = ruleNode(P_OPTIONAL_STATEMENTS);
            insertLeft(OptionalValueRule, optionalStatementsRule);
            OptionalStatements(token, optionalStatementsRule, buffer);
    // P_BLOCK
            ASTNode *blockRule = ruleNode(P_BLOCK);
            insertLeft(optionalStatementsRule, blockRule);
            Block(token, blockRule, buffer);
    // P_ELSE 
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(blockRule, ElseRule);
                ElseStatement(token, ElseRule, buffer);
            }
        }
        else {
    // P_BLOCK
            ASTNode *blockRule = ruleNode(P_BLOCK);
            insertLeft(OptionalValueRule, blockRule);
            Block(token, blockRule, buffer);
        
    // P_ELSE
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(blockRule, ElseRule);
                ElseStatement(token, ElseRule, buffer);
            }
        } 
    }
    else {
    // : 
        if ((*token)->type == T_COLON) {
            ASTNode *colonFound = checkToken(token, T_COLON, NO_KW, "SYNTAX ERROR: While expected :");
            freeAST(colonFound);
            *token = get_token(buffer);
    // P_OPTIONAL_STATEMENTS
            ASTNode *optionalStatementsRule = ruleNode(P_OPTIONAL_STATEMENTS);
            insertLeft(expressionRule, optionalStatementsRule);
            OptionalStatements(token, optionalStatementsRule, buffer);
    // P_BLOCK
            ASTNode *blockRule = ruleNode(P_BLOCK);
            insertLeft(optionalStatementsRule, blockRule);
            Block(token, blockRule, buffer);
    // P_ELSE
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(blockRule, ElseRule);
                ElseStatement(token, ElseRule, buffer);
            }
        }
        else {
    // P_BLOCK
            ASTNode *blockRule = ruleNode(P_BLOCK);
            insertLeft(expressionRule, blockRule);
            Block(token, blockRule, buffer);
    // P_ELSE
            if((*token)->type == T_KW && (*token)->value.keyword == KW_ELSE) {
                ASTNode *ElseRule = ruleNode(P_ELSE_STATEMENT);
                insertLeft(blockRule, ElseRule);
                ElseStatement(token, ElseRule, buffer);
            }
        }
    }
}
void OptionalStatements(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // (
    ASTNode *lParenFound = checkToken(token,T_LPAREN,NO_KW, "SYNTAX ERROR: OptionalStatements expected (");
    freeAST(lParenFound);
    *token = get_token(buffer);
    // single statement
    if ((*token)->type == T_ID || ((*token)->type == T_KW && ((*token)->value.keyword == KW_CONST || (*token)->value.keyword == KW_VAR || (*token)->value.keyword == KW_WHILE || (*token)->value.keyword == KW_IF || (*token)->value.keyword == KW_RETURN || (*token)->value.keyword == KW_BREAK || (*token)->value.keyword == KW_CONTINUE || (*token)->value.keyword == KW_FOR))) {
        ASTNode *singleStatementRule = ruleNode(P_SINGLE_STATEMENT);
        insertRight(ptr, singleStatementRule);
        SingleStatement(token, singleStatementRule, buffer, false);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: OptionalStatements expected )");
        freeAST(rParenFound);
        *token = get_token(buffer);
    }
    // P_BLOCK
    else {
        ASTNode *BlockRule = ruleNode(P_BLOCK);
        insertRight(ptr, BlockRule);
        Block(token, BlockRule, buffer);
    // )
        ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: OptionalStatements expected )");
        freeAST(rParenFound);
        *token = get_token(buffer);
    }
}
void Return(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    //RETURN
    ASTNode *returnFound = checkToken(token, T_KW, KW_RETURN, "SYNTAX ERROR: Return expected return");
    freeAST(returnFound);
    *token = get_token(buffer);
    //P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    // ;
    ASTNode *semiColonFound = checkToken(token, T_SEMICOL, NO_KW, "SYNTAX ERROR: Return expected ;");
    freeAST(semiColonFound);
    *token = get_token(buffer);
}
void For(Token **token, ASTNode *ptr, c_buff_ptr buffer) {
    // FOR
    ASTNode *forFound = checkToken(token, T_KW, KW_FOR, "SYNTAX ERROR: For expected for");
    freeAST(forFound);
    *token = get_token(buffer);
    // (
    ASTNode *lParenFound = checkToken(token, T_LPAREN, NO_KW, "SYNTAX ERROR: For expected (");
    freeAST(lParenFound);
    *token = get_token(buffer);
    // P_EXPRESSION
    ASTNode *expressionRule = ruleNode(P_EXPRESSION);
    insertRight(ptr, expressionRule);
    Expression(token, expressionRule, buffer);
    if (expressionRule->right == NULL) {
            Free(token);
            error_exit(2, "SYNTAX ERROR wrong for condition!\n"); 
    }
    // )
    ASTNode *rParenFound = checkToken(token, T_RPAREN, NO_KW, "SYNTAX ERROR: For expected )");
    freeAST(rParenFound);
    *token = get_token(buffer);
    // P_OPTIONAL_VALUE
    ASTNode *OptionalValueRule = ruleNode(P_OPTIONAL_VALUE);
    insertLeft(expressionRule, OptionalValueRule);
    OptionalValue(token, OptionalValueRule, buffer);
    // P_BLOCK
    ASTNode *BlockRule = ruleNode(P_BLOCK);
    insertLeft(OptionalValueRule, BlockRule);
    Block(token, BlockRule, buffer);
}
void End(Token **token, ASTNode *ptr) {
    ASTNode *eofFound = checkToken(token,T_EOF, NO_KW, "SYNTAX ERROR: END WHERE IS END OF FILE ?!");
    insertRight(ptr, eofFound);
}