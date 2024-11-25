#include "sem_anal.h"
#include "ast.h"
#include "symtable.h"
#include "error.h"

// INT + INT CONVERSION FOR SOME REASON
void analyse (ASTNode* node) {
    if(node == NULL) {
        return;
    }
    if(node->type == P_EXPRESSION) {
        checkExpr(node->right);
    }
    analyse(node->left);
    analyse(node->right);

}

void checkExpr(ASTNode* node) {
    if(node == NULL) {
        return;
    }
    switch(node->type) {
        case DIV:
            checkDiv(node);
            checkBinTypes(node);
            break;
        case EQ:
        case NEQ:
        case LESS:
        case LEQ:
        case MORE:
        case MEQ:
        case PLUS:
        case MINUS:
        case MUL:
            checkBinTypes(node);
            break;
        case BANG:
            checkUnTypes(node);
            break;
        default:
            break;
    }
    checkExpr(node->left);
    checkExpr(node->right);
}

void checkDiv(ASTNode* node) {    
    if(node->left->type == TYPE_INT && node->right->type == TYPE_INT) {
        node->token->type = T_IDIV;
        node->type = IDIV;
    }
}

void checkBinTypes(ASTNode* node) {
    if(node->left == NULL || node->right == NULL) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(7, "ERROR: Missing operand in binary operation!\n");
        return;
    }
    if(node->left->type == TYPE_INT && node->right->type == TYPE_F64) {
        ASTNode* temp = node->left;
        Token* new_token = (Token*)malloc(sizeof(Token));
        if (new_token == NULL){ 
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "Memory allocation failed"); 
        }
        new_token->type = T_I2F;
        // create node for orelse
        ASTNode* new_node = nodeCreate(I2F, new_token);
        new_node->right = temp;
        node->left = new_node;
    }
    if(node->right->type == TYPE_INT && node->left->type == TYPE_F64) {
        ASTNode* temp = node->right;
        Token* new_token = (Token*)malloc(sizeof(Token));
        if (new_token == NULL){ 
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(99, "Memory allocation failed"); 
        }
        new_token->type = T_I2F;
        // create node for orelse
        ASTNode* new_node = nodeCreate(I2F, new_token);
        new_node->right = temp;
        node->right = new_node;
    }
    if(node->left->type != node->right->type) {
        symtable_dispose(&SymFunctionTree);
        freeAST(ASTRoot);
        error_exit(7, "ERROR: Wrong types in binary operation!\n");
    }
}

void checkUnTypes(ASTNode* node) {
    switch(node->right->type) {
        case TYPE_INT:
        case TYPE_F64:
        case ID:
        case BANG:
            return;
        default:
            symtable_dispose(&SymFunctionTree);
            freeAST(ASTRoot);
            error_exit(7, "ERROR: Wrong type in unary operation!\n");
    }
}