#include "symtable_utils.h"
#include <stdio.h>
#include <string.h>

typedef enum direction { left, right, none } direction_t;

void print_sym_subtree(symtable_node_ptr tree, char *prefix, direction_t from);
void print_symtable(symtable_node_ptr tree) {
    printf("Binary tree structure:\n");
    printf("\n");
    if (tree != NULL) {
        print_sym_subtree(tree, "", none);
    } else {
        printf("Tree is empty\n");
    }
    printf("\n");
}

void printReturnType(ret_type type) {
    switch (type) {
        case T_INT_RET:
            printf("int");
            break;

        case T_FLOAT_RET:
            printf("float");
            break;

        case T_BOOL_RET:
            printf("bool");
            break;

        case T_STR_RET:
            printf("str");
            break;

        case T_NULL_RET:
            printf("null");
            break;

        default:
            printf("Unknown");
            break;
    }
}

void printEntryType(symtable_entry_type type) {
    switch (type) {
        case T_VAR_SYM:
            printf("var");
            break;

        case T_FUN_SYM:
            printf("fun");
            break;

        default:
            printf("Unknown");
            break;
    }
}

void print_sym_node(symtable_node_ptr node)
{
    printf("[");
    printEntryType(node->entry->entry_type);
    printf(", {");
    printf("%d, ", node->entry->type);
    printf("%d, ", node->entry->isNullable);
    printf("%d, ", node->entry->isUsed);
    printf("%d, ", node->entry->isChanged);
    if(node->entry->entry_type == T_FUN_SYM){
        printf("%d {", node->entry->param_count);
        for (int i = 0; i < node->entry->param_count; i++) {
            printf("%d, ", node->entry->param_nullable[i]);
            printf("%d, ", node->entry->param_types[i]);
        }
        printf("}, ");
        printf("%d", node->entry->returnsValue);
    } else {
        printf("%d, ", node->entry->isConst);
        printf("%d, ", node->entry->hasExplicitType);
    }
    printf("}]");
}

    const char *subtree_prefix_sym = "  |";
    const char *space_prefix_syn = "   ";

char *make_prefix_syn(char *prefix, const char *suffix) {
    char *result = (char *)malloc(strlen(prefix) + strlen(suffix) + 1);
    strcpy(result, prefix);
    result = strcat(result, suffix);
    return result;
}

void print_sym_subtree(symtable_node_ptr tree, char *prefix, direction_t from) {
    if (tree != NULL) {
        char *current_subtree_prefix = make_prefix_syn(prefix, subtree_prefix_sym);
        char *current_space_prefix = make_prefix_syn(prefix, space_prefix_syn);

        if (from == left) {
            printf("%s\n", current_subtree_prefix);
        }

        print_sym_subtree(
            tree->right,
            from == left ? current_subtree_prefix : current_space_prefix, right);

        printf("%s  +-", prefix);
        print_sym_node(tree);
        printf("\n");

        print_sym_subtree(
            tree->left,
            from == right ? current_subtree_prefix : current_space_prefix, left);

        if (from == right) {
            printf("%s\n", current_subtree_prefix);
        }

        free(current_space_prefix);
        free(current_subtree_prefix);
    }
}
