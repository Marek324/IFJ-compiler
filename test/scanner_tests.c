#include "scanner_tests.h"

circ_buff_ptr buffer = NULL;
int saved_stdin;

void scanner_setup(void)
{
    buffer = circ_buff_init();
    saved_stdin = dup(STDIN_FILENO); // Save the original stdin
}

void setup_stdin(const char *input_data) {
    FILE *input_file = fopen("temp_input.txt", "w");
    fputs(input_data, input_file);
    fclose(input_file);
    freopen("temp_input.txt", "r", stdin); // Redirect stdin to the file
}

void teardown(void)
{
    // Restore the original stdin
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
    remove("temp_input.txt"); // Clean up the temporary file
    circ_buff_free(buffer);
}

TestSuite(scanner_tests, .init = scanner_setup, .fini = teardown);

Test(scanner_tests, test_float_with_positive_exponent)
{
    setup_stdin("12.0e+3");

    Token expected_tokens[] = {
        TOKEN(T_FLOAT, 12000.0),
    };
    
    for (int i = 0; i < N; i++){
        Token *token = get_token(buffer);

        assert_token_eq(token, &expected_tokens[i]);

        free_token(token);
    }
}

Test(scanner_tests, test_float_with_negative_exponent)
{
    setup_stdin("12.0e-3");

    Token expected_tokens[] = {
        TOKEN(T_FLOAT, 0.012),
    };
    
    for (int i = 0; i < N; i++){
        Token *token = get_token(buffer);

        assert_token_eq(token, &expected_tokens[i]);

        free_token(token);
    }
}

Test(scanner_tests, test_float_with_positive_exponent_no_plus)
{
    setup_stdin("12.0e3");

    Token expected_tokens[] = {
        TOKEN(T_FLOAT, 12000.0),
    };
    
    for (int i = 0; i < N; i++){
        Token *token = get_token(buffer);

        assert_token_eq(token, &expected_tokens[i]);

        free_token(token);
    }
}

Test(scanner_tests, test_float_with_negative_exponent_no_minus)
{
    setup_stdin("12.0e3");

    Token expected_tokens[] = {
        TOKEN(T_FLOAT, 12000.0),
    };
    
    for (int i = 0; i < N; i++){
        Token *token = get_token(buffer);

        assert_token_eq(token, &expected_tokens[i]);

        free_token(token);
    }
}
