/*
file: exp_parser.h
author: Adam Vesely
Implementation of the expression parser.
*/

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

    while(token != NULL && !expressionEnd(isEnd(token), stackIsEmpty(operand_stack), stackIsEmpty(operator_stack))) {
        ASTNode* node = nodeCreate(convertToASTType(token->type), NO_KW);
        if(isOperand(token)) {
            stackPush(operand_stack, (long)node);
        }
        else if(isOperator(token)) {
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

    }
    stackClear(operand_stack);
    stackClear(operator_stack);

    // switch(token->type) {
    //     case T_ID:
    //     case T_INT:
    //     case T_FLOAT:
    //     case T_STR:
    //         parsePrimaryExpression(token, buff);
    //         break;
    //     case T_LPAREN:
    //     case T_RPAREN:
    //         return nodeCreate()
            
    // }

    // ASTNode *left = parsePrimaryExpression();
    // if (left == NULL) {
    //     return NULL;
    // }

    // return parseBinaryExpression(left, 0); // Start with lowest precedence
}

// Function to parse a primary expression (numbers, identifiers, etc.)
ASTNode *parsePrimaryExpression() {

    // ASTNode* node = nodeCreate(convertToASTType(token->type, NO_KW), token);
    // switch (token->type) {
    //     case T_INT:
    //     case T_FLOAT:
    //     case T_STR:
    //     case T_ID:
    //         return nodeCreate(P_EXPRESSION, token); // Create a node for the primary expression

    //     case LPAREN:
    //         {
    //             ASTNode *expr = parseExpression();
    //             token = getNextToken(); // Get the next token after expression
    //             if (token == NULL || token->type != RPAREN) {
    //                 // Handle error: expected closing parenthesis
    //                 return NULL;
    //             }
    //             return expr; // Return the expression inside parentheses
    //         }

    //     default:
    //         // Handle error: unexpected token
    //         return NULL;
    // }
}

// Function to parse unary expressions (e.g., -x, !x)
ASTNode *parseUnaryExpression() {
    // Token *token = getNextToken();
    // if (token == NULL) {
    //     return NULL; // Error: unexpected end of input
    // }

    // if (token->type == PLUS || token->type == MINUS || token->type == BANG) {
    //     ASTNode *operand = parsePrimaryExpression();
    //     if (operand == NULL) {
    //         return NULL; // Error: failed to parse operand
    //     }
    //     return nodeCreate(P_EXPRESSION, token); // Create a unary expression node
    // }

    // // If not a unary operator, return NULL for fallback
    // return NULL;
}

// Function to parse binary expressions with operator precedence
ASTNode *parseBinaryExpression(ASTNode *left, int precedence) {
    // Token *token = getNextToken(); // Get the next token
    // while (token != NULL && getPrecedence(token->type) > precedence) {
    //     int currentPrecedence = getPrecedence(token->type);
    //     ASTNode *right = parseUnaryExpression();
    //     if (right == NULL) {
    //         return NULL; // Error: failed to parse right operand
    //     }

    //     // Create a binary expression node
    //     ASTNode *binaryNode = nodeCreate(P_EXPRESSION, token);
    //     insertLeft(binaryNode, left);
    //     insertRight(binaryNode, right);

    //     left = binaryNode; // Update left for next iteration
    //     token = getNextToken(); // Get the next token
    // }

    // return left; // Return the constructed expression tree
}

void reduce(stack_t* operand_stack, stack_t* operator_stack) {
    ASTNode* root = stackGetTop(operator_stack);
    stackPop(operator_stack);

    ASTNode* child = stackGetTop(operand_stack);
    stackPop(operator_stack);
    insertRight(root, child);

    if(root->type != BANG) {
        child = stackGetTop(operand_stack);
        stackPop(operator_stack);
        insertLeft(root, child);
    }

    stackPush(operand_stack, (long)root);

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
            return true;
    }
    return false;
}

bool expressionEnd(bool end, bool operand_stack_isEmpty, bool operator_stack_isEmpty) {
    return operand_stack_isEmpty && operator_stack_isEmpty && end;
}
