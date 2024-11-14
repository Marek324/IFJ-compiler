/*
file: exp_parser.h
author: Adam Vesely
Implementation of the expression parser.
*/
// TODO ERROR HANDLING AND EXTENSIONS
#include <stdbool.h>

#include "../include/exp_parser.h"
#include "../include/scanner.h"
#include "../include/stack.h"

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
    { H, U, H, H,   H,   H, H, H, U, H, U, H }, // ID "i"
    { L, L, L, L,   L,   L, L, L, L, U, L, U }  // END "$"
};

// Function to parse an expression
ASTNode *parseExpression(Token* token, circ_buff_ptr buff) {

    if (token == NULL) {
        return NULL; // Error: unexpected end of input
    }
    stack_t* operand_stack = stackInit();
    stack_t* operator_stack = stackInit();
    // number of left parenthesis on the stack
    int* paren_depth = malloc(sizeof(int));
    if(paren_depth == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for paren_depth!\n");
        return;
    }
    *paren_depth = 0;

    while(token != NULL && !expressionEnd(isEnd(token), stackIsEmpty(operator_stack))) {
        ASTNode* node = nodeCreate(convertToASTType(token->type), NO_KW);
        if(isOperand(token)) {
            stackPush(operand_stack, (long)node);
        }
        else if(isOperator(token)) {
            if(token->type == T_LPAREN) {
                *paren_depth++;
            }
            if(token->type == T_RPAREN) {
                if(*paren_depth != 0) {
                    reduceParen(operand_stack, operator_stack);
                    (*paren_depth)--;
                }
                else {
                    break;
                }
                continue;
            }
            if(stackIsEmpty(operator_stack)) {
                stackPush(operator_stack, (long)node);
                continue;
            }
            ASTNode* top = (ASTNode*)stackGetTop(operator_stack);
            switch(precedence_table[getIndex(top)][getIndex(token)]) {
                case L:
                    stackPush(operator_stack, (long)node);
                    break;
                case H:
                case E:
                    reduce(operand_stack, operator_stack);
                    stackPush(operator_stack, (long)node);
                    break;
                case U:
                default:
                    return NULL;
            }
        }
        else {
            return NULL;
        }
        token = get_token(buff);
    }

    ASTNode* root = reduceAll(operand_stack, operator_stack);
    stackClear(operand_stack);
    stackClear(operator_stack);
    free(paren_depth);

    return root;
}

void reduce(stack_t* operand_stack, stack_t* operator_stack) {
    ASTNode* root = (ASTNode*)stackGetTop(operator_stack);
    stackPop(operator_stack);

    ASTNode* child = (ASTNode*)stackGetTop(operand_stack);
    stackPop(operator_stack);
    insertRight(root, child);

    if(root->type != BANG) {
        child = (ASTNode*)stackGetTop(operand_stack);
        stackPop(operator_stack);
        insertLeft(root, child);
    }

    stackPush(operand_stack, (long)root);

}

void reduceParen(stack_t* operand_stack, stack_t* operator_stack) {
    ASTNode* curr = (ASTNode*)stackGetTop(operator_stack);
    while(curr->type != T_LPAREN && !stackIsEmpty(operator_stack)) {
        reduce(operand_stack, operator_stack);
        curr = (ASTNode*)stackGetTop(operator_stack);
    }
    stackPop(operator_stack);
}

ASTNode* reduceAll(stack_t* operand_stack, stack_t* operator_stack) {
    // while (), a = ();
    if(stackIsEmpty(operator_stack) && stackIsEmpty(operand_stack)) {
        return NULL;
    }
    while(!stackIsEmpty(operator_stack)) {
        reduce(operand_stack, operator_stack);
    }
    ASTNode* root = (ASTNode*)stackGetTop(operand_stack);
    stackPop(operand_stack);
    return root;
}

PREC_TABLE_INDEX getIndex(Token* token) {
    switch (token->type) {
        case T_BANG:
            return NEGATE;
        case T_MUL:
        case T_DIV:
            return MUL_DIV;
        case T_PLUS:
        case T_MINUS:
            return ADD_SUB;
        case T_EQ:
        case t_NEQ:
        case T_LESS:
        case T_LEQ:
        case T_MORE:
        case T_MEQ:
            return RELATION;
        case T_LPAREN:
            return L_PAR;
        case T_RPAREN:
            return R_PAR;
        case T_ID:
        case T_INT:
        case T_FLOAT:
        case T_STR:
            return ID;
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
            return true;
    }
    return false;
}

bool isOperand(Token* token) {
    switch(token->type) {
        case T_ID:
        case T_INT:
        case T_FLOAT:
        case T_STR:
            return true;
    }
    return false;
}

bool isEnd(Token* token) {
    switch(token->type) {
        case T_RPAREN:
        case T_SEMICOL:
        case T_COMMA:
            return true;
    }
    return false;
}

bool expressionEnd(bool end, bool operator_stack_isEmpty) {
    return end && operator_stack_isEmpty;
}
