#include <check.h>
#include <token.h>
#include <scanner.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
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
////////////////////////////// TESTS TOTAL/FINAL ////////////////////////////////////////////////

START_TEST(test_hello)
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

Suite * TotalSuite()
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("TotalTests");

    /* Core test case */
    tc_core = tcase_create("Core5");

    tcase_add_test(tc_core, test_hello);


    suite_add_tcase(s, tc_core);

    return s;
}

 int main(int argc, char** argv)
 {
    int number_failed;
    Suite *scanSuit, *totalSuit;
    SRunner *scanRunner, *totalRunner;
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