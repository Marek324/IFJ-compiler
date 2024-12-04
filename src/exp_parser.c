/*
file: exp_parser.h
author: Adam Vesely
Implementation of the expression parser.
*/
// TODO ERROR HANDLING AND EXTENSIONS, IMPLICIT CONVERSIONS
#include <stdbool.h>

#include "exp_parser.h"
#include "scanner.h"
#include "stack.h"
#include "parser.h"
#include "symtable.h"
#include "ast.h"

#define P_TABLE_SIZE 12

// we compare the operator on the stack vs the current operator from the token
ASOC precedence_table[P_TABLE_SIZE][P_TABLE_SIZE] = 
{ //  .? ! */ +- orelse  r and or (  )  i  $
    { L, U, U, U,   U,   U, U, U, L, H, L, H }, // UNREACHABLE ".?"
    { U, L, H, H,   H,   H, H, H, L, H, H, H }, // NEGATE "!"
    { U, L, H, H,   H,   H, H, H, L, H, L, H }, // MUL_DIV "* /"
    { U, L, L, H,   H,   H, H, H, L, H, L, H }, // ADD_SUB "+ -"
    { U, L, L, L,   H,   H, H, H, L, H, L, H }, // ORELSE "orelse"
    { U, L, L, L,   L,   U, H, H, L, H, L, H }, // RELATION "r"
    { U, L, L, L,   L,   L, H, H, L, H, L, H }, // AND "and"
    { U, L, L, L,   L,   L, L, H, L, H, L, H }, // OR "or"
    { L, L, L, L,   L,   L, L, L, L, E, L, U }, // L_PAR "("
    { H, H, H, H,   H,   H, H, H, U, H, U, H }, // R_PAR ")"
    { H, U, H, H,   H,   H, H, H, U, H, U, H }, // LIT_ID "i"
    { L, L, L, L,   L,   L, L, L, L, U, L, U }  // END "$"
};

// Function to parse an expression
ASTNode *parseExpression(Token **token, circ_buff_ptr buff) {

    if (*token == NULL) {
        return NULL; // Error: unexpected end of input
    }

    if(*token == T_AT_IMPORT) {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(4, "ERROR: @import!\n");
    }

    int* paren_depth = malloc(sizeof(int));
    if (paren_depth == NULL) {
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(99, "ERROR: Unable to allocate memory for paren_depth!\n");
        return NULL;
    }
    *paren_depth = 0;
    stack_t* operand_stack = stackInit();
    stack_t* operator_stack = stackInit();
    TokenType temp_type = (*token)->type;

    while (*token != NULL && !expressionEnd(isEnd(*token), paren_depth)) {
        ASTNode* node = NULL;
        // for keywords
        if((*token)->type == T_KW) {
            node = nodeCreate(convertToASTType(T_KW, (*token)->value.keyword), *token);
        }
        else {
            node = nodeCreate(convertToASTType((*token)->type, NO_KW), *token);    
        }
        // for @as(i32, id) ------------------------------------------------------------------------
        if((*token)->type == T_AT_AS) {
            // get token after AT_AS (@as) [should be LPAREN]
            *token = get_token(buff);
            if((*token)->type != T_LPAREN) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                fprintf(stderr, "SYNTAX ERROR: Missing LPAREN after \"@as\"\n");
                exit(2);
            }
            free_token(*token);
            // get token after LPAREN (should be i32)
            *token = get_token(buff);
            if((*token)->value.keyword != KW_I32) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                fprintf(stderr, "SYNTAX ERROR: Missing i32 after \"(\"\n");
                exit(2);
            }
            free_token(*token);
            // get token after i32 (should be comma)
            *token = get_token(buff);
            if((*token)->type != T_COMMA) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                fprintf(stderr, "SYNTAX ERROR: Missing COMMA after \"i32\"\n");
                exit(2);
            }
            free_token(*token);
            ASTNode* expressionList = ruleNode(P_EXPRESSION_LIST);
            insertLeft(node, expressionList);
            // get token after comma (should be expression)
            *token = get_token(buff);
            ExpressionList(token, expressionList, buff);
            stackPush(operand_stack, (long)node);
            // get token after expression (should be RPAREN)
            if((*token)->type != T_RPAREN) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Missing RPAREN after expression\n");
            }
            free_token(*token);
            // get token after RPAREN (should be semicolon)
            *token = get_token(buff);
            if((*token)->type != T_SEMICOL) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Missing SEMICOLON after RPAREN\n");
            }
            continue;
        } 
        // for function calls ----------------------------------------------------------------------
        if((temp_type == T_ID) && ((*token)->type == T_LPAREN || (*token)->type == T_DOT)) {
            if((*token)->type == T_LPAREN) {
                freeAST(node);
                ASTNode* id_node = (ASTNode*)stackGetTop(operand_stack);
                if(id_node == NULL || id_node->type != ID) {
                    freeAST(ASTRoot);
                    freeAll(paren_depth, operand_stack, operator_stack);
                    symtable_dispose(&SymFunctionTree);
                    error_exit(2, "ERROR: Unexpected LPAREN in expression!\n");
                    return NULL;
                }
                // get token after LPAREN (should be expression)
                *token = get_token(buff);
                ASTNode *expressionListRule = ruleNode(P_EXPRESSION_LIST);
                insertLeft(id_node, expressionListRule);
                ExpressionList(token, expressionListRule, buff);
            }
            if((*token)->type == T_DOT) {
                freeAST(node);
                // get token after dot (should be ID or question mark)
                *token = get_token(buff);
                node = nodeCreate(convertToASTType((*token)->type, NO_KW), *token);
                if(node->type != ID && node->type != QMARK) {
                    freeAll(paren_depth, operand_stack, operator_stack);
                    symtable_dispose(&SymFunctionTree);
                    error_exit(2, "ERROR: Unexpected DOT in expression!\n");
                    return NULL;
                }
                if(node->type == QMARK) {
                    // frees QMARK node
                    freeAST(node);
                    // create a new token for orelse
                    Token* new_token = (Token*)malloc(sizeof(Token));
                    if (new_token == NULL){ 
                        freeAll(paren_depth, operand_stack, operator_stack);
                        freeAST(ASTRoot);
                        symtable_dispose(&SymFunctionTree);
                        error_exit(99, "Memory allocation failed"); 
                    }
                    new_token->type = T_KW;
                    new_token->value.keyword = KW_ORELSE;
                    // create node for orelse
                    node = nodeCreate(convertToASTType(T_KW, new_token->value.keyword), new_token);
                    if(stackIsEmpty(operand_stack)) {
                        freeAll(paren_depth, operand_stack, operator_stack);
                        freeAST(ASTRoot);
                        symtable_dispose(&SymFunctionTree);
                        error_exit(2, "ERROR: Unexpected QMARK in expression!\n");
                    }
                    ASTNode* id_node = (ASTNode*)stackGetTop(operand_stack);
                    insertLeft(node, id_node);
                    stackPop(operand_stack);

                    // create a new token for unreachable
                    new_token = (Token *)malloc(sizeof(Token));
                    if (new_token == NULL){ 
                        freeAll(paren_depth, operand_stack, operator_stack);
                        freeAST(ASTRoot);
                        symtable_dispose(&SymFunctionTree);
                        error_exit(99, "Memory allocation failed"); 
                    }
                    new_token->type = T_KW;
                    new_token->value.keyword = KW_UNREACHABLE;
                    // create node for unreachable
                    ASTNode* unreachable_node = nodeCreate(convertToASTType(T_KW, new_token->value.keyword), new_token);
                    insertRight(node, unreachable_node);
                    stackPush(operand_stack, (long)node);
                    // gets token after QMARK
                    *token = get_token(buff);
                    continue;
                }
                else if(node->type == ID) {
                    ASTNode* id_node = (ASTNode*)stackGetTop(operand_stack);
                    insertRight(id_node, node);
                    // get token after ID (should be LPAREN)
                    *token = get_token(buff);
                    continue;
                }
                else {
                    freeAll(paren_depth, operand_stack, operator_stack);
                    freeAST(ASTRoot);
                    symtable_dispose(&SymFunctionTree);
                    error_exit(2, "ERROR: Wrong token after DOT!\n");
                }
                
            }
            if((*token)->type != T_RPAREN) {
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "ERROR: Unexpected token in expression!\n");
                return NULL;
            }
            // RPAREN
            free(*token);
            *token = get_token(buff);
            continue;
        }
        temp_type = (*token)->type;
        // -------------Function calls ----------------------------------------------------------------
        // ============================================================================================
        // -------------Ternary operator --------------------------------------------------------------
        if(node->type == T_IF) {
            ASTNode* expression1 = ruleNode(P_EXPRESSION);
            insertLeft(node, expression1);
            stackPush(operand_stack, (long)node);
            // gets token after "if" -> should be LPAREN
            *token = get_token(buff);
            if((*token)->type != T_LPAREN) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Ternary operator! Missing LPAREN after \"if\"\n");
            }
            free_token(*token);
            // gets token after LPAREN -> should be expression
            *token = get_token(buff);
            ASTNode* expressionFound = parseExpression(token, buff);
            if(expressionFound == NULL) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Ternary operator! Missing expression after LPAREN\n");
            }
            insertRight(expression1, expressionFound);

            if((*token)->type != T_RPAREN) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Ternary operator! Missing RPAREN after expression\n");
            }
            free_token(*token);

            ASTNode* expression2 = ruleNode(P_EXPRESSION);
            insertLeft(expression1, expression2);

            // gets token after RPAREN -> should be expression
            *token = get_token(buff);
            expressionFound = parseExpression(token, buff);
            if(expressionFound == NULL) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Ternary operator! Missing expression after RPAREN\n");
            }
            insertRight(expression2, expressionFound);
            
            if((*token)->value.keyword != KW_ELSE) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Ternary operator! Missing ELSE after expression\n");
            }
            free_token(*token);

            expression1 = ruleNode(P_EXPRESSION);
            insertLeft(expression2, expression1);

            // gets token after ELSE -> should be expression
            *token = get_token(buff);
            expressionFound = parseExpression(token, buff);
            if(expressionFound == NULL) {
                free_token(*token);
                freeAll(paren_depth, operand_stack, operator_stack);
                freeAST(ASTRoot);
                symtable_dispose(&SymFunctionTree);
                error_exit(2, "SYNTAX ERROR: Ternary operator! Missing expression after ELSE\n");
            }

            insertRight(expression1, expressionFound);
            continue;
        }
        // -------------Ternary operator --------------------------------------------------------------

        if (isOperand(*token)) {
            stackPush(operand_stack, (long)node);
        }
        else if (isOperator(*token)) {
            if ((*token)->type == T_LPAREN) {
                (*paren_depth)++;
            }
            if ((*token)->type == T_RPAREN) {
                if (*paren_depth != 0) {
                    freeAST(node);
                    reduceParen(paren_depth, operand_stack, operator_stack);
                    (*paren_depth)--;
                }
                else {
                    break;
                }
                *token = get_token(buff); // Update token
                continue;
            }
            if (stackIsEmpty(operator_stack)) {
                stackPush(operator_stack, (long)node);
                *token = get_token(buff); // Update token
                continue;
            }
            ASTNode* top = (ASTNode*)stackGetTop(operator_stack);
            switch (precedence_table[getIndex(top)][getIndex(node)]) {
                case L:
                    stackPush(operator_stack, (long)node);
                    break;
                case H:
                case E:
                    reduce(paren_depth, operand_stack, operator_stack);
                    stackPush(operator_stack, (long)node);
                    break;
                case U:
                default:
                    freeAll(paren_depth, operand_stack, operator_stack);
                    if(node != NULL) {
                        freeAST(node);
                    }
                    return NULL;
            }
        }
        else {
            freeAll(paren_depth,operand_stack, operator_stack);
            return NULL;
        }
        *token = get_token(buff); // Update token
    }
    ASTNode* root = reduceAll(paren_depth, operand_stack, operator_stack);
    if(!stackIsEmpty(operand_stack) || !stackIsEmpty(operator_stack)) {
        freeAll(paren_depth, operand_stack, operator_stack);
        freeAST(ASTRoot);
        symtable_dispose(&SymFunctionTree);
        error_exit(2, "ERROR: Unable to reduce!\n");
    }
    freeAll(paren_depth, operand_stack, operator_stack);
    return root;
}

void reduce(int* paren_depth, stack_t* operand_stack, stack_t* operator_stack) {
    if(!stackIsEmpty(operator_stack)) {
        ASTNode* root = (ASTNode*)stackGetTop(operator_stack);
        stackPop(operator_stack);
        if(!stackIsEmpty(operand_stack)) {
            ASTNode* child = (ASTNode*)stackGetTop(operand_stack);
            stackPop(operand_stack);
            insertRight(root, child);
            if(root->type != BANG) {
                if(stackIsEmpty(operand_stack)) {
                    freeAll(paren_depth, operand_stack, operator_stack);
                    freeAST(ASTRoot);
                    symtable_dispose(&SymFunctionTree);
                    error_exit(2, "ERROR: Not enough operands!\n");
                }
                child = (ASTNode*)stackGetTop(operand_stack);
                stackPop(operand_stack);
                insertLeft(root, child);
            }

            stackPush(operand_stack, (long)root);
        }
        else {
            freeAll(paren_depth, operand_stack, operator_stack);
            freeAST(ASTRoot);
            symtable_dispose(&SymFunctionTree);
            error_exit(2, "ERROR: Not enough operands!\n");
        }
    }
}

void reduceParen(int* paren_depth, stack_t* operand_stack, stack_t* operator_stack) {
    if(!stackIsEmpty(operator_stack)) {
        ASTNode* curr = (ASTNode*)stackGetTop(operator_stack);
        while(curr->type != LPAREN && !stackIsEmpty(operator_stack)) {
            reduce(paren_depth, operand_stack, operator_stack);
            curr = (ASTNode*)stackGetTop(operator_stack);
        }
        // LPAREN
        freeAST(curr);
        stackPop(operator_stack);
    }
}

ASTNode* reduceAll(int* paren_depth, stack_t* operand_stack, stack_t* operator_stack) {
    // while (), a = ()...;
    if(stackIsEmpty(operator_stack) && stackIsEmpty(operand_stack)) {
        return NULL;
    }
    while(!stackIsEmpty(operator_stack)) {
        reduce(paren_depth, operand_stack, operator_stack);
    }
    ASTNode* root = (ASTNode*)stackGetTop(operand_stack);
    stackPop(operand_stack);

    return root;
}

PREC_TABLE_INDEX getIndex(ASTNode* node) {
    switch (node->type) {
        case BANG:
            return NEGATE;
        case MUL:
        case DIV:
            return MUL_DIV;
        case PLUS:
        case MINUS:
            return ADD_SUB;
        case EQ:
        case NEQ:
        case LESS:
        case LEQ:
        case MORE:
        case MEQ:
            return RELATION;
        case LPAREN:
            return L_PAR;
        case RPAREN:
            return R_PAR;
        case ID:
        case TYPE_INT:
        case TYPE_F64:
        case TYPE_STR:
            return LIT_ID;
        case T_ORELSE:
            return ORELSE ;
        case T_AND:
            return AND;
        case T_OR:
            return OR;
        default:
            return END;
    }   
}

bool isOperator(Token* token) {
    switch(token->type) {
        case T_EQ:
        case T_BANG:
        case T_NEQ:
        case T_LESS:
        case T_LEQ:
        case T_MORE:
        case T_MEQ:
        case T_PLUS:
        case T_MINUS:
        case T_MUL:
        case T_DIV:
        case T_LPAREN:
        case T_RPAREN:
            return true;
        case T_KW:
            switch(token->value.keyword) {
                case KW_UNREACHABLE:
                case KW_AND:
                case KW_OR:
                case KW_ORELSE:
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

bool isOperand(Token* token) {
    switch(token->type) {
        case T_ID:
        case T_INT:
        case T_FLOAT:
        case T_STR:
        case T_AT_AS:
            return true;
        case T_KW:
            switch(token->value.keyword) {
                case KW_TRUE:
                case KW_FALSE:
                case KW_NULL:
                case KW_UNREACHABLE:
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

bool isEnd(Token* token) {
    switch(token->type) {
        case T_RPAREN:
        case T_SEMICOL:
        case T_COMMA:
            return true;
        case T_KW:
            if(token->value.keyword == KW_ELSE) {
                return true;
            }
            else {
                return false;
            }
        default:
            return false;
    }
}

bool expressionEnd(bool end, int* paren_depth) {
    return end && (*paren_depth == 0);
}

void freeAll(int* paren_depth, stack_t* operand_stack, stack_t* operator_stack) {
    if(operand_stack != NULL) {
        stackClear(operand_stack);
    }
    if(operator_stack != NULL) {
        stackClear(operator_stack);
    }
    if(paren_depth != NULL) {
        free(paren_depth);    
    }    
}
