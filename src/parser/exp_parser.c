/*
file: exp_parser.h
author: Adam Vesely
Implementation of the expression parser.
*/

#include "../include/exp_parser.h"
#include "../include/scanner.h"

#define P_TABLE_SIZE 12

int precedence_table[P_TABLE_SIZE][P_TABLE_SIZE] = 
{ // .? ! */ +- orelse  r and or (  )  i  $
    {L, U, U, U,   U,   U, U, U, L, H, L, H}, // UNREACHABLE ".?"
    {U, L, H, H,   H,   H, H, H, L, H, H, H}, // NEGATE "!"
    {U, L, H, H,   H,   H, H, H, L, H, L, H}, // MUL_DIV "* /"
    {U, L, L, H,   H,   H, H, H, L, H, L, H}, // ADD_SUB "+ -"
    {U, L, L, L,   H,   H, H, H, L, H, L, H}, // ORELSE "orelse"
    {U, L, L, L,   L,   U, H, H, L, H, L, H}, // RELATION "r"
    {U, L, L, L,   L,   L, H, H, L, H, L, H}, // AND "and"
    {U, L, L, L,   L,   L, L, H, L, H, L, H}, // OR "or"
    {L, L, L, L,   L,   L, L, L, L, E, L, U}, // L_PAR "("
    {H, H, H, H,   H,   H, H, H, U, H, U, H}, // R_PAR ")"
    {H, U, H, H,   H,   H, H, H, U, H, U, H}, // ID "i"
    {L, L, L, L,   L,   L, L, L, L, U, L, U} // END "$"
};

// Function to parse an expression
ASTNode *parseExpression(Token* token, circ_buff_ptr buff) {

    ASTNode *left = parsePrimaryExpression();
    if (left == NULL) {
        return NULL; // Error: failed to parse primary expression
    }

    return parseBinaryExpression(left, 0); // Start with lowest precedence
}

// Function to parse a primary expression (numbers, identifiers, etc.)
ASTNode *parsePrimaryExpression() {
    Token *token = getNextToken(); // Assuming you have a function to get the next token
    if (token == NULL) {
        return NULL; // Error: unexpected end of input
    }

    switch (token->type) {
        case T_INT:
        case T_F64:
        case T_ID:
            return nodeCreate(P_EXPRESSION, token); // Create a node for the primary expression

        case LPAREN:
            {
                ASTNode *expr = parseExpression();
                token = getNextToken(); // Get the next token after expression
                if (token == NULL || token->type != RPAREN) {
                    // Handle error: expected closing parenthesis
                    return NULL;
                }
                return expr; // Return the expression inside parentheses
            }

        default:
            // Handle error: unexpected token
            return NULL;
    }
}

// Function to parse unary expressions (e.g., -x, !x)
ASTNode *parseUnaryExpression() {
    Token *token = getNextToken();
    if (token == NULL) {
        return NULL; // Error: unexpected end of input
    }

    if (token->type == PLUS || token->type == MINUS || token->type == BANG) {
        ASTNode *operand = parsePrimaryExpression();
        if (operand == NULL) {
            return NULL; // Error: failed to parse operand
        }
        return nodeCreate(P_EXPRESSION, token); // Create a unary expression node
    }

    // If not a unary operator, return NULL for fallback
    return NULL;
}

// Function to parse binary expressions with operator precedence
ASTNode *parseBinaryExpression(ASTNode *left, int precedence) {
    Token *token = getNextToken(); // Get the next token
    while (token != NULL && getPrecedence(token->type) > precedence) {
        int currentPrecedence = getPrecedence(token->type);
        ASTNode *right = parseUnaryExpression();
        if (right == NULL) {
            return NULL; // Error: failed to parse right operand
        }

        // Create a binary expression node
        ASTNode *binaryNode = nodeCreate(P_EXPRESSION, token);
        insertLeft(binaryNode, left);
        insertRight(binaryNode, right);

        left = binaryNode; // Update left for next iteration
        token = getNextToken(); // Get the next token
    }

    return left; // Return the constructed expression tree
}

// Function to get the precedence of operators
int getPrecedence(TokenType type) {
    switch (type) {
        case PLUS:
        case MINUS:
            return 1; // Lowest precedence
        case MUL:
        case DIV:
            return 2; // Higher precedence
        case BANG: // Unary operator
            return 3;
        default:
            return 0; // No precedence
    }
}
