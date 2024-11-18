#include "utils.h"

void printToken(Token *token) {
    if (token == NULL) return;

    // Print based on the token type
    switch (token->type) {
        case T_ID: 
            printf("ID(%s) ", token->value.string_value); 
            break;

        case T_KW: 
            switch (token->value.keyword) {
                case KW_CONST: printf("Keyword(const)"); break;
                case KW_IF: printf("Keyword(if)"); break;
                case KW_ELSE: printf("Keyword(else)"); break;
                case KW_FN: printf("Keyword(fn)"); break;
                case KW_I32: printf("Keyword(i32)"); break;
                case KW_F64: printf("Keyword(f64)"); break;
                case KW_BOOL: printf("Keyword(bool)"); break;
                case KW_NULL: printf("Keyword(null)"); break;
                case KW_PUB: printf("Keyword(pub)"); break;
                case KW_RETURN: printf("Keyword(return)"); break;
                case KW_U8: printf("Keyword(u8)"); break;
                case KW_VAR: printf("Keyword(var)"); break;
                case KW_VOID: printf("Keyword(void)"); break;
                case KW_FOR: printf("Keyword(for)"); break;
                case KW_WHILE: printf("Keyword(while)"); break;
                default: printf("Unknown Keyword"); break;
            }
            break;

        case T_AT_IMPORT: 
            printf("@import"); 
            break;

        case T_AT_AS: 
            printf("@as"); 
            break;

        case T_INT: 
            printf("Int(%lld)", token->value.int_value); 
            break;

        case T_FLOAT: 
            printf("Float(%lf)", token->value.float_value); 
            break;

        case T_STR: 
            printf("String(\"%s\")", token->value.string_value); 
            break;

        case T_ASGN: 
            printf("="); 
            break;

        case T_EQ: 
            printf("=="); 
            break;

        case T_BANG: 
            printf("!"); 
            break;

        case T_NEQ: 
            printf("!="); 
            break;

        case T_LESS: 
            printf("<"); 
            break;

        case T_LEQ: 
            printf("<="); 
            break;

        case T_MORE: 
            printf(">"); 
            break;

        case T_MEQ: 
            printf(">="); 
            break;

        case T_PLUS: 
            printf("+"); 
            break;

        case T_MINUS: 
            printf("-"); 
            break;

        case T_MUL: 
            printf("*"); 
            break;

        case T_DIV: 
            printf("/"); 
            break;

        case T_LPAREN: 
            printf("("); 
            break;

        case T_RPAREN: 
            printf(")"); 
            break;

        case T_LBRACE: 
            printf("{"); 
            break;

        case T_RBRACE: 
            printf("}"); 
            break;

        case T_LBRACKET: 
            printf("["); 
            break;

        case T_RBRACKET: 
            printf("]"); 
            break;

        case T_COMMA: 
            printf(","); 
            break;

        case T_DOT: 
            printf("."); 
            break;

        case T_SEMICOL: 
            printf(";"); 
            break;

        case T_COLON: 
            printf(":"); 
            break;

        case T_PIPE: 
            printf("|"); 
            break;

        case T_QMARK: 
            printf("?"); 
            break;

        case T_EOF: 
            printf("EOF"); 
            break;

        case T_ERROR: 
            printf("Error"); 
            break;

        default: 
            printf("Unknown"); 
            break;
    }
}

void printNodeType(ASTNodeType type){
    switch (type) {
        case P_PROG: 
            printf("P_PROG"); 
            break;

        case P_PROLOG: 
            printf("P_PROLOG"); 
            break;

        case P_END: 
            printf("P_END"); 
            break;

        case P_FUNCTION_DEF: 
            printf("P_FUNCTION_DEF"); 
            break;

        case P_PARAM_LIST: 
            printf("P_PARAM_LIST"); 
            break;

        case P_COMMA_PAR_FOUND: 
            printf("P_COMMA_PAR_FOUND"); 
            break;

        case P_BLOCK: 
            printf("P_BLOCK"); 
            break;

        case P_STATEMENT: 
            printf("P_STATEMENT"); 
            break;

        case P_ID_FOUND: 
            printf("P_ID_FOUND"); 
            break;

        case P_VAR_DECLARATION:
            printf("P_VAR_DECLARATION");
            break;

        case P_IF_STATEMENT:
            printf("P_IF_STATEMENT");
            break;

        case P_IF_FOUND:
            printf("P_IF_FOUND");
            break;

        case P_ELSE_STATEMENT:
            printf("P_ELSE_STATEMENT");
            break;

        case P_OPTIONAL_VALUE:
            printf("P_OPTIONAL_VALUE");
            break;

        case P_WHILE_LOOP:
            printf("P_WHILE_LOOP");
            break;

        case P_RETURN_STATEMENT:
            printf("P_RETURN_STATEMENT");
            break;

        case P_EXPRESSION_LIST:
            printf("P_EXPRESSION_LIST");
            break;

        case P_COMMA_EXPR_FOUND:     
            printf("P_COMMA_EXPR_FOUND");
            break;

        case P_TYPE:
            printf("P_TYPE");
            break;

        case P_FOR_LOOP:
            printf("P_FOR_LOOP");
            break;

        case P_OPTIONAL_STATEMENTS:
            printf("P_OPTIONAL_STATEMENTS");
            break;

        case P_TYPE_COMPLETE:
            printf("P_TYPE_COMPLETE");
            break;

        case P_QUESTION_MARK:
            printf("P_QUESTION_MARK");
            break;

        case P_SINGLE_STATEMENT:
            printf("P_SINGLE_STATEMENT");
            break;

        case P_FUNCTION_TYPE:
            printf("P_FUNCTION_TYPE");
            break;

        case P_EXPRESSION:
            printf("P_EXPRESSION");
            break;  

        case ID:
            printf("ID");
            break;

        
        case AT_IMPORT:
            printf("AT_IMPORT");
            break;

        case TYPE_INT:
            printf("TYPE_INT");
            break;

        case TYPE_F64:
            printf("TYPE_F64");
            break;

        case TYPE_STR:
            printf("TYPE_STR");
            break;

        case ASSGN:
            printf("ASSGN");
            break;

        case EQ:
            printf("EQ");
            break;

        case BANG:
            printf("BANG");
            break;

        case NEQ:
            printf("NEQ");
            break;

        case LESS:
            printf("LESS");
            break;

        case LEQ:   
            printf("LEQ");
            break;
        
        case MORE:
            printf("MORE");
            break;

        case MEQ:   
            printf("MEQ");
            break;

        case PLUS:
            printf("PLUS");
            break;

        case MINUS: 
            printf("MINUS");
            break;

        case MUL:
            printf("MUL");
            break;

        case DIV:
            printf("DIV");
            break;

        case LPAREN:
            printf("LPAREN");
            break;

        case RPAREN:
            printf("RPAREN");
            break;

        case LBRACE:
            printf("LBRACE");
            break;

        case RBRACE:
            printf("RBRACE");
            break;

        case LBRACKET:
            printf("LBRACKET");
            break;
        
        case RBRACKET:  
            printf("RBRACKET");
            break;

        case COMMA:
            printf("COMMA");
            break;

        case DOT:
            printf("DOT");
            break;

        case SEMICOLON:
            printf("SEMICOLON");
            break;

        case COLON:
            printf("COLON");
            break; 

        case PIPE:
            printf("PIPE");
            break;

        case QMARK:
            printf("QMARK");
            break;

        case END_OF_FILE:
            printf("END_OF_FILE");
            break;

        case T_CONST:
            printf("T_CONST");
            break;

        case T_IF:
            printf("T_IF");
            break;
        
        case T_ELSE:
            printf("T_ELSE");
            break;

        case T_FN:
            printf("T_FN");
            break;

        case T__KW_I32:
            printf("T__KW_I32");
            break;

        
        case T_KW_F64:
            printf("T_KW_F64");
            break;

        case T_KW_BOOL:
            printf("T_KW_BOOL");
            break;

        case T_NULL:   
            printf("T_NULL");
            break;

        case T_PUB:
            printf("T_PUB");
            break;

        case T_RETURN:
            printf("T_RETURN");
            break;

        case T_U8:
            printf("T_U8");
            break;

        case T_VAR:
            printf("T_VAR");
            break;

        case T_VOID:
            printf("T_VOID");
            break;

        case T_WHILE:
            printf("T_WHILE");
            break;

        default: 
            printf("Unknown"); 
            break;
    }
}

typedef enum direction { left, right, none } direction_t;

void print_tree(ASTNode *tree);

void print_node(ASTNode *node)
{
    printf("[");
    printNodeType(node->type);
    printf(", ");
    printToken(node->token);
    printf("]");
}

    const char *subtree_prefix = "  |";
    const char *space_prefix = "   ";

char *make_prefix(char *prefix, const char *suffix) {
    char *result = (char *)malloc(strlen(prefix) + strlen(suffix) + 1);
    strcpy(result, prefix);
    result = strcat(result, suffix);
    return result;
}

void print_subtree(ASTNode *tree, char *prefix, direction_t from) {
    if (tree != NULL) {
        char *current_subtree_prefix = make_prefix(prefix, subtree_prefix);
        char *current_space_prefix = make_prefix(prefix, space_prefix);

        if (from == left) {
            printf("%s\n", current_subtree_prefix);
        }

        print_subtree(
            tree->right,
            from == left ? current_subtree_prefix : current_space_prefix, right);

        printf("%s  +-", prefix);
        print_node(tree);
        printf("\n");

        print_subtree(
            tree->left,
            from == right ? current_subtree_prefix : current_space_prefix, left);

        if (from == right) {
            printf("%s\n", current_subtree_prefix);
        }

        free(current_space_prefix);
        free(current_subtree_prefix);
    }
}

void print_tree(ASTNode *tree) {
    printf("Binary tree structure:\n");
    printf("\n");
    if (tree != NULL) {
        print_subtree(tree, "", none);
    } else {
        printf("Tree is empty\n");
    }
    printf("\n");
}