#include <check.h>
#include <token.h>
#include <scanner.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include "symtable.h"

////////////////////////////// COMPONENTS ////////////////////////////////////////////////
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

void scanner_teardown(void)
{
    // Restore the original stdin
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
    remove("temp_input.txt"); // Clean up the temporary file
    circ_buff_free(buffer);
}

void checkInterpretError(int errcode){
    printf("\n");
    if (errcode == 50){
        printf("chybně zadané vstupní parametry na příkazovém řádku při spouštění interpretu");
    }
    if (errcode == 51){
        printf(" chyba při analýze");
    }
    if (errcode == 52){
        printf("chyba při sémantických kontrolách vstupního kódu v IFJcode24.");
    }
    if (errcode == 53){
        printf(" běhová chyba interpretace - špatné typy operandů.");
    }
    if (errcode == 54){
        printf("běhová chyba interpretace - přístup k neexistující proměnné (rámec existuje).");
    }
    if (errcode == 55){
        printf("běhová chyba interpretace - rámec neexistuje (např. čtení z prázdného zásobníku rámců).");
    }
    if (errcode == 56){
        printf("běhová chyba interpretace - chybějící hodnota (v proměnné, na datovém zásobníku, nebo v zásobníku volání).");
    }
    if (errcode == 57){
        printf(" běhová chyba interpretace - špatná hodnota operandu (např. dělení nulou, špatná návratová hodnota instrukce EXIT).");
    }
    if (errcode == 58){
        printf(" běhová chyba interpretace - chybná práce s řetězcem");
    }
    if (errcode == 60){
        printf("interní chyba interpretu tj. neovlivněná vstupním programem (např. chyba alokace paměti, chyba při otvírání souboru s řídicím programem atd.).");
    }
    printf("\n");
}

void checkMidError(int errcode){
    printf("\n");
    if (errcode == 1){
        printf("chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).");
    }
    if (errcode == 2){
        printf("chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.).");
    }
    if (errcode == 3){
        printf("sémantická chyba v programu - nedefinovaná funkce či proměnná.");
    }
    if (errcode == 4){
        printf("sémantická chyba v programu - špatný počet/typ parametrů u volání funkce; špatný typ či nepovolené zahození návratové hodnoty z funkce.");
    }
    if (errcode == 5){
        printf("sémantická chyba v programu - redefinice proměnné nebo funkce; přiřazení do nemodifikovatelné proměnné.");
    }
    if (errcode == 6){
        printf(" sémantická chyba v programu - chybějící/přebývající výraz v příkazu návratu z funkce");
    }
    if (errcode == 7){
        printf("sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech; nekompatibilní typ výrazu (např. při přiřazení).");
    }
    if (errcode == 8){
        printf("sémantická chyba odvození typu - typ proměnné není uveden a nelze odvodit od použitého výrazu.");
    }
    if (errcode == 9){
        printf("sémantická chyba nevyužité proměnné v jejím rozsahu platnosti; modifikovatelná proměnná bez možnosti změny po její inicializaci.");
    }
    if (errcode == 10){
        printf("ostatní sémantické chyby.");
    }
    if (errcode == 99){
        printf("interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.).");
    }
    printf("\n");
}


FILE* temp_function(const char* filename){
    printf("Do some magic! Also magic with file: %s\n",filename);
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL; // Return NULL if the file couldn't be opened
    }

    // Return the file pointer
    return file;
}

void tree_to_file_Left_Right(ASTNode *root, FILE* file){
    if (root == NULL)
    {
        return;
    }
    tree_to_file_Left_Right(root->left, file);
    addNodeEXPToFile(root, file);
    tree_to_file_Left_Right(root->right, file);
}

void tree_to_file_Right_Left(ASTNode *root, FILE* file){
    if (root == NULL)
    {
        return;
    }
    tree_to_file_Right_Left(root->right, file);
    addNodeEXPToFile(root, file);
    tree_to_file_Right_Left(root->left, file);
}

void addNodeEXPToFile(ASTNode *node, FILE* file){
    const char *data = ASTNodeTypeToString(node->type);
    fputs(data,file);
    fputs(" ",file);
}

int compare_files_line_by_line(FILE *file1, FILE *file2) {
    char line1[1024], line2[1024];

    // Read and compare the files line by line
    while (fgets(line1, sizeof(line1), file1) != NULL) {
        if (fgets(line2, sizeof(line2), file2) == NULL) {
            return 0;  // file2 ended before file1
        }
        if (strcmp(line1, line2) != 0) {
            return 0;  // lines are different
        }
    }

    // If file2 still has data left, it's different
    if (fgets(line2, sizeof(line2), file2) != NULL) {
        return 0;  // file2 has extra lines
    }

    // If no differences were found
    return 1;  // Files are identical
}
const char* ASTNodeTypeToString(ASTNodeType type) {
    switch (type) {
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

        // Terminal nodes
        case ID: return "ID";
        case AT_IMPORT: return "AT_IMPORT";
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
        default: return "UNKNOWN";
    }
}


////////////////////////////// TESTS SCANNER ////////////////////////////////////////////////

START_TEST(test_KwIdImport)
{
    scanner_setup();
    //Test T_KW
    setup_stdin("if lmao @import");
    Token *token = get_token(buffer);
    ck_assert_int_eq(0,token->type);
    free_token(token);
    //Test T_ID
    token = get_token(buffer);
    ck_assert_int_eq(1,token->type);
    free_token(token);
    //Test T_AT_IMPORT
    token = get_token(buffer);
    ck_assert_int_eq(2,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_DataTypes)
{
    scanner_setup();
    setup_stdin("10 10.0 \"aaa\"");
    //Test Int
    Token *token = get_token(buffer);
    ck_assert_int_eq(3,token->type);
    free_token(token);
    //Test Float
    token = get_token(buffer);
    ck_assert_int_eq(4,token->type);
    free_token(token);
    //Test String
    token = get_token(buffer);
    ck_assert_int_eq(5,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Operations1)
{
    scanner_setup();
    setup_stdin("= == ! !=");
    //Test ASGN
    Token *token = get_token(buffer);
    ck_assert_int_eq(6,token->type);
    free_token(token);
    //Test EQ
    token = get_token(buffer);
    ck_assert_int_eq(7,token->type);
    free_token(token);
    //Test BANG
    token = get_token(buffer);
    ck_assert_int_eq(8,token->type);
    free_token(token);
    //Test NEQ
    token = get_token(buffer);
    ck_assert_int_eq(9,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Operations2)
{
    scanner_setup();
    setup_stdin("< <= > >=");
    //Test LESS
    Token *token = get_token(buffer);
    ck_assert_int_eq(10,token->type);
    free_token(token);
    //Test LEQ
    token = get_token(buffer);
    ck_assert_int_eq(11,token->type);
    free_token(token);
    //Test MORE
    token = get_token(buffer);
    ck_assert_int_eq(12,token->type);
    free_token(token);
    //Test MEQ
    token = get_token(buffer);
    ck_assert_int_eq(13,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Operations3)
{
    scanner_setup();
    setup_stdin("+ - * /");
    //Test PLUS
    Token *token = get_token(buffer);
    ck_assert_int_eq(14,token->type);
    free_token(token);
    //Test MINUS
    token = get_token(buffer);
    ck_assert_int_eq(15,token->type);
    free_token(token);
    //Test MUL
    token = get_token(buffer);
    ck_assert_int_eq(16,token->type);
    free_token(token);
    //Test DIV
    token = get_token(buffer);
    ck_assert_int_eq(17,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Brackets)
{
    scanner_setup();
    setup_stdin("( ) { } [ ]");
    //Test T_LPAREN
    Token *token = get_token(buffer);
    ck_assert_int_eq(18,token->type);
    free_token(token);
    //Test T_RPAREN
    token = get_token(buffer);
    ck_assert_int_eq(19,token->type);
    free_token(token);
    //Test T_LBRACE
    token = get_token(buffer);
    ck_assert_int_eq(20,token->type);
    free_token(token);
    //Test T_RBRACE
    token = get_token(buffer);
    ck_assert_int_eq(21,token->type);
    free_token(token);
    //Test T_LBRACKET
    token = get_token(buffer);
    ck_assert_int_eq(22,token->type);
    free_token(token);
    //Test T_RBRACKET
    token = get_token(buffer);
    ck_assert_int_eq(23,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Punctuation)
{
    scanner_setup();
    setup_stdin(", . ; : | ?");
    //Test T_COMMA
    Token *token = get_token(buffer);
    ck_assert_int_eq(24,token->type);
    free_token(token);
    //Test T_DOT
    token = get_token(buffer);
    ck_assert_int_eq(25,token->type);
    free_token(token);
    //Test T_SEMICOL
    token = get_token(buffer);
    ck_assert_int_eq(26,token->type);
    free_token(token);
    //Test T_COLON
    token = get_token(buffer);
    ck_assert_int_eq(27,token->type);
    free_token(token);
    //Test T_PIPE
    token = get_token(buffer);
    ck_assert_int_eq(28,token->type);
    free_token(token);
    //Test T_QMARK
    token = get_token(buffer);
    ck_assert_int_eq(29,token->type);
    free_token(token);
    //Test T_EOF
    token = get_token(buffer);
    ck_assert_int_eq(31,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Empty)
{
    scanner_setup();
    setup_stdin("");
    //Test Empty-EOF
    Token *token = get_token(buffer);
    ck_assert_int_eq(31,token->type);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Keywords1)
{
    scanner_setup();
    setup_stdin("const if else fn i32 f64");
    //Test KW_CONST
    Token *token = get_token(buffer);
    ck_assert_int_eq(0,token->value.keyword);
    free_token(token);
    //Test KW_IF
    token = get_token(buffer);
    ck_assert_int_eq(1,token->value.keyword);
    free_token(token);
    //Test KW_ELSE
    token = get_token(buffer);
    ck_assert_int_eq(2,token->value.keyword);
    free_token(token);
    //Test KW_FN
    token = get_token(buffer);
    ck_assert_int_eq(3,token->value.keyword);
    free_token(token);
    //Test KW_I32
    token = get_token(buffer);
    ck_assert_int_eq(4,token->value.keyword);
    free_token(token);
    //Test KW_F64
    token = get_token(buffer);
    ck_assert_int_eq(5,token->value.keyword);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_Keywords2)
{
    scanner_setup();
    setup_stdin("pub return u8 var void while");
    //Test KW_PUB
    Token *token = get_token(buffer);
    ck_assert_int_eq(7,token->value.keyword);
    free_token(token);
    //Test KW_RETURN
    token = get_token(buffer);
    ck_assert_int_eq(8,token->value.keyword);
    free_token(token);
    //Test KW_U8
    token = get_token(buffer);
    ck_assert_int_eq(9,token->value.keyword);
    free_token(token);
    //Test KW_VAR
    token = get_token(buffer);
    ck_assert_int_eq(10,token->value.keyword);
    free_token(token);
    //Test KW_VOID
    token = get_token(buffer);
    ck_assert_int_eq(11,token->value.keyword);
    free_token(token);
    //Test KW_WHILE
    token = get_token(buffer);
    ck_assert_int_eq(12,token->value.keyword);
    free_token(token);

    scanner_teardown();
}
END_TEST

START_TEST(test_NULL)
{
    scanner_setup();
    setup_stdin("null");
    //Test KW_NULL
    Token *token = get_token(buffer);
    ck_assert_int_eq(6,token->value.keyword);
    free_token(token);

    scanner_teardown();
}
END_TEST
////////////////////////////// TESTS EXP PARSER ////////////////////////////////////////////////

//Test for singular token
START_TEST(test_EXP_EMPTY)
{
    scanner_setup(); 
    setup_stdin(" ");
    
    Token *token = get_token(buffer);
    ASTNode *root = parseExpression(token, buffer);

    ck_assert_int_eq( END_OF_FILE , root->type);
    ck_assert_mem_eq( token, root->Token);

    free_token(token);
    freeAST(root);

    scanner_teardown();
}
END_TEST

//Test for longer stuff
START_TEST(test_EXP_Prolog)
{
    scanner_setup(); 
    setup_stdin("const ifj = @import(\"ifj24.zig\");"); // This is the input you are testing
    
    Token *token = get_token(buffer);
    ASTNode *root = parseExpression(token, buffer); // Create the EXP tree

    FILE *output_file = fopen("test_EXP_Prolog.out", "r"); // This is a file with the expected output, needs to be created (add space at the end (if needed))
    FILE *output_file_EXP = fopen("tempTreeLR.txt", "w+"); // File with the output from our prog
    tree_to_file_Left_Right(root, output_file_EXP); // Writes tree to file


    ck_assert_int_eq(1 , compare_files_line_by_line(output_file,output_file_EXP));

    fclose(output_file); // Start cleaning up and closing all the stuff
    fclose(output_file_our);
    remove("tempTreeLR.txt");
    free_token(token);
    freeAST(root);

    scanner_teardown();
}
////////////////////////////// TESTS PARSER ////////////////////////////////////////////////
START_TEST(test_Parse_Prolog)
{
    scanner_setup(); 
    setup_stdin("const ifj = @import(\"ifj24.zig\");"); // This is the input you are testing
    
    Token *token = get_token(buffer);
    ASTNode *root = parseExpression(token, buffer); // Create the EXP tree

    ////// IMPORTANT NEED TO RUN PARSER AS IS :D (will add it later)

    FILE *output_file = fopen("test_Parse_Prolog.out", "r"); // This is a file with the expected output, needs to be created (add space at the end (if needed))
    FILE *output_file_EXP = fopen("tempTreeRL.txt", "w+"); // File with the output from our prog
    tree_to_file_Left_Right(root, output_file_EXP); // Writes tree to file


    ck_assert_int_eq(1 , compare_files_line_by_line(output_file,output_file_EXP));

    fclose(output_file); // Start cleaning up and closing all the stuff
    fclose(output_file_our);
    remove("tempTreeRL.txt");
    free_token(token);
    freeAST(root);

    scanner_teardown();
}
////////////////////////////// TESTS SYMTABLE ////////////////////////////////////////////////
START_TEST(test_SYM_Empty)
{
    
}
END_TEST
////////////////////////////// TESTS TOTAL/FINAL ////////////////////////////////////////////////

START_TEST(test_TL_hello)
{

    //temp_function("hello.out"); // temp function represents our compiler, hello.out is to be replaced with hello.zig
    int res = system("main < hello.zig > hello.code");

    if(res != 0){
        checkMidError(res);
    }

    int result = system("ic24int hello.code < hello.in > output_file_our.out"); // change stuff with paths if we want it in different folders (hello.code >> dir/hello.code)
    
    if(result != 0){
        checkInterpretError(result);
    }

    FILE *output_file = fopen("hello.out", "r");
    FILE *output_file_our = fopen("output_file_our.out", "r");

    ck_assert_int_eq(1 , compare_files_line_by_line(output_file,output_file_our));

    fclose(output_file);
    fclose(output_file_our);


}
END_TEST


////////////////////////////// END TESTS ////////////////////////////////////////////////

char* suiteToRun = "all";


 Suite * ScannerSuite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ScannerTests");

    /* Core test case */
    tc_core = tcase_create("Core1");

    tcase_add_test(tc_core, test_KwIdImport);
    tcase_add_test(tc_core, test_DataTypes);
    tcase_add_test(tc_core, test_Operations1);
    tcase_add_test(tc_core, test_Operations2);
    tcase_add_test(tc_core, test_Operations3);
    tcase_add_test(tc_core, test_Brackets);
    tcase_add_test(tc_core, test_Punctuation);
    tcase_add_test(tc_core, test_Empty);
    tcase_add_test(tc_core, test_Keywords1);
    tcase_add_test(tc_core, test_Keywords2);
    tcase_add_test(tc_core, test_NULL);


    suite_add_tcase(s, tc_core);

    return s;
}

Suite * EXParseSuite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("EXPParserTests");

    /* Core test case */
    tc_core = tcase_create("Core3");

    tcase_add_test(tc_core, test_EXP_EMPTY);
    tcase_add_test(tc_core, test_EXP_Prolog);

    suite_add_tcase(s, tc_core);

    return s;
}

Suite * ParseSuite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("ParserTests");

    /* Core test case */
    tc_core = tcase_create("Core4");

    tcase_add_test(tc_core, test_Parse_Prolog);

    suite_add_tcase(s, tc_core);

    return s;
}

Suite * SymSuite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("SymTableTests");

    /* Core test case */
    tc_core = tcase_create("Core6");

    tcase_add_test(tc_core, test_SYM_Empty);


    suite_add_tcase(s, tc_core);

    return s;
}

Suite * TotalSuite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("TotalTests");

    /* Core test case */
    tc_core = tcase_create("Core7");

    tcase_add_test(tc_core, test_TL_hello);


    suite_add_tcase(s, tc_core);

    return s;
}

 int main(int argc, char** argv)
 {
    int number_failed;
    Suite *scanSuit,*expParseSuit,*ParseSuit, *totalSuit,*SymSuit;
    SRunner *scanRunner,*expParseRunner,*ParseRunner, *totalRunner,*SymRunner;
    if (argc == 1){
        suiteToRun = "all";
    }else{
        suiteToRun =  argv[1];
    }


    if (strcmp(suiteToRun, "Scanner") == 0 || strcmp(suiteToRun, "all") == 0) {
        printf("\n");
        scanSuit = ScannerSuite();
        scanRunner = srunner_create(scanSuit);

        srunner_run_all(scanRunner, CK_VERBOSE);
        number_failed = srunner_ntests_failed(scanRunner);
        srunner_free(scanRunner);
        printf("\n");
    }

    if (strcmp(suiteToRun, "EXParser") == 0 || strcmp(suiteToRun, "all") == 0) {
        printf("\n");
        expParseSuit = EXParseSuite();
        expParseRunner = srunner_create(expParseSuit);

        srunner_run_all(expParseRunner, CK_VERBOSE);
        number_failed = srunner_ntests_failed(expParseRunner);
        srunner_free(expParseRunner);
        printf("\n");
    }

    if (strcmp(suiteToRun, "Parser") == 0 || strcmp(suiteToRun, "all") == 0) {
        printf("\n");
        ParseSuit = ParseSuite();
        ParseRunner = srunner_create(ParseSuit);

        srunner_run_all(ParseRunner, CK_VERBOSE);
        number_failed = srunner_ntests_failed(ParseRunner);
        srunner_free(ParseRunner);
        printf("\n");
    }

    if (strcmp(suiteToRun, "Sym") == 0 || strcmp(suiteToRun, "all") == 0) {
        printf("\n");
        SymSuit = SymSuite();
        SymRunner = srunner_create(SymSuit);

        srunner_run_all(SymRunner, CK_VERBOSE);
        number_failed = srunner_ntests_failed(SymRunner);
        srunner_free(SymRunner);
        printf("\n");
    }

    if (strcmp(suiteToRun, "Total") == 0 || strcmp(suiteToRun, "all") == 0) {
        printf("\n");
        totalSuit = TotalSuite();
        totalRunner = srunner_create(totalSuit);

        srunner_run_all(totalRunner, CK_VERBOSE);
        number_failed = srunner_ntests_failed(totalRunner);
        srunner_free(totalRunner);
        printf("\n");
    }

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }