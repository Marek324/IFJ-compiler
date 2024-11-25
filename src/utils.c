#include "utils.h"

const char* getNodeName(ASTNode *node) {
    switch (node->type) {
        case P_PROG: return "P_PROG";
        case P_PROLOG: return "P_PROLOG";
        case P_END: return "P_END";
        case P_FUNCTION_DEF: return "P_FUNCTION_DEF";
        case P_PARAM_LIST: return "P_PARAM_LIST";
        case P_COMMA_PAR_FOUND: return "P_COMMA_PAR_FOUND";
        case P_BLOCK: return "P_BLOCK";
        case P_STATEMENT: return "P_STATEMENT";
        case P_ID_FOUND: return "P_ID_FOUND";
        case P_VAR_DECLARATION: return "P_VAR_DECLARATION";
        case P_IF_STATEMENT: return "P_IF_STATEMENT";
        case P_IF_FOUND: return "P_IF_FOUND";
        case P_ELSE_STATEMENT: return "P_ELSE_STATEMENT";
        case P_OPTIONAL_VALUE: return "P_OPTIONAL_VALUE";
        case P_WHILE_LOOP: return "P_WHILE_LOOP";
        case P_CONTINUE: return "P_CONTINUE";
        case P_BREAK: return "P_BREAK";
        case P_RETURN_STATEMENT: return "P_RETURN_STATEMENT";
        case P_EXPRESSION_LIST: return "P_EXPRESSION_LIST";
        case P_COMMA_EXPR_FOUND: return "P_COMMA_EXPR_FOUND";
        case P_TYPE: return "P_TYPE";
        case P_FOR_LOOP: return "P_FOR_LOOP";
        case P_OPTIONAL_STATEMENTS: return "P_OPTIONAL_STATEMENTS";
        case P_TYPE_COMPLETE: return "P_TYPE_COMPLETE";
        case P_QUESTION_MARK: return "P_QUESTION_MARK";
        case P_SINGLE_STATEMENT: return "P_SINGLE_STATEMENT";
        case P_FUNCTION_TYPE: return "P_FUNCTION_TYPE";
        case P_EXPRESSION: return "P_EXPRESSION";
        case P_ASGN_FOUND: return "P_ASGN_FOUND";
        case ID: return node->token->value.string_value;
        case AT_IMPORT: return "AT_IMPORT";
        case AT_AS: return "AT_AS";
        case TYPE_INT: return "TYPE_INT";
        case TYPE_F64: return "TYPE_F64";
        case TYPE_STR: return "TYPE_STR";
        case ASSGN: return "ASSGN";
        case EQ: return "EQ";
        case BANG: return "BANG";
        case NEQ: return "NEQ";
        case LESS: return "LESS";
        case LEQ: return "LEQ";
        case MORE: return "MORE";
        case MEQ: return "MEQ";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case COMMA: return "COMMA";
        case DOT: return "DOT";
        case SEMICOLON: return "SEMICOLON";
        case COLON: return "COLON";
        case PIPE: return "PIPE";
        case QMARK: return "QMARK";
        case END_OF_FILE: return "END_OF_FILE";
        case T_CONST: return "T_CONST";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_FN: return "T_FN";
        case T__KW_I32: return "T__KW_I32";
        case T_KW_F64: return "T_KW_F64";
        case T_KW_BOOL: return "T_KW_BOOL";
        case T_NULL: return "T_NULL";
        case T_PUB: return "T_PUB";
        case T_RETURN: return "T_RETURN";
        case T_U8: return "T_U8";
        case T_VAR: return "T_VAR";
        case T_VOID: return "T_VOID";
        case T_WHILE: return "T_WHILE";
        case T_UNREACHABLE: return "T_UNREACHABLE";
        case T_ORELSE: return "T_ORELSE";
        case T_AND: return "T_AND";
        case T_OR: return "T_OR";
        case T_TRUE: return "T_TRUE";
        case T_FALSE: return "T_FALSE";
        case I2F: return "I2F";
        case F2I: return "F2I";
        default: return "UNKNOWN";
    }
}

// Recursive function to write the tree in D2 format
void write_d2(ASTNode *node, FILE *file) {
    if (node == NULL) return;

    if (node != NULL) {
        // Include token type, attributes, line, and column
        fprintf(file, "Node%p: \"%s\"\n", 
                (void*)node, 
                getNodeName(node));
    }

    // Write the left child
    if (node->left != NULL) {
        fprintf(file, "Node%p -> Node%p: left\n", (void*)node, (void*)node->left);
        write_d2(node->left, file);
    }

    // Write the right child
    if (node->right != NULL) {
        fprintf(file, "Node%p -> Node%p: right\n", (void*)node, (void*)node->right);
        write_d2(node->right, file);
    }
}

// Function to generate D2 file from AST
void generate_d2_file(ASTNode *root, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "direction: right\ngraph {\n"); // Start of D2 graph
    write_d2(root, file);
    fprintf(file, "}\n"); // End of D2 graph

    fclose(file);
}