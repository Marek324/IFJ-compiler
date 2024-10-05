#include "../../include/scanner.h"

int main(int argc, char **argv) {
    if (argc!=0){};
    // Test the get_token function
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("Error opening input file\n");
        return 1;
    }
    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        printf("Error opening output file\n");
        return 1;
    }
    
    while(1) {
        Token *token = get_token(input);
        fprintf(output, "%d,", token->type);
        if(token->type == T_F64) {
            fprintf(output, "%f", token->value.float_value);
        } else if(token->type == T_INT) {
            fprintf(output, "%d", token->value.int_value);
        } else if(token->type == T_STR) {
            fprintf(output, "%s", token->value.string_value->str);
        } else if(token->type == T_KW) {
            fprintf(output, "%c", token->value.keyword);
        } 
        fprintf(output, "\n");

        free_token(token);
        if(token->type == T_EOF) {
            break;
        }
    }

    fclose(input);
    fclose(output);

    return 0;
}