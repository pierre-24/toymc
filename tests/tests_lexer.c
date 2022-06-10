#include "tests.h"
#include "lexer.h"

START_TEST(test_lexer) {
        char* str = "ab[-9]";
        int l = strlen(str);
        tm_parf_token t;
        _OK(tm_lexer_token_init(&t, str));

        tm_parf_token_type tab[] = {
            TM_TK_CHAR, TM_TK_CHAR, TM_TK_LBRACKET, TM_TK_DASH, TM_TK_DIGIT, TM_TK_RBRACKET, TM_TK_EOS};

        for(int i=0; i <= l; i++) {
            ck_assert_int_eq(t.type, tab[i]);
            tm_lexer_advance(&t, str, 1);
        }
}
END_TEST

START_TEST(test_lexer_line) {
    char* str = "a\nb1\ncde\nf2";
    int l = strlen(str);
    tm_parf_token t;
    _OK(tm_lexer_token_init(&t, str));
    int line = 1, pos_in_line = 0;

    for(int i=0; i < l; i++) {
        ck_assert_int_eq(t.line, line);
        ck_assert_int_eq(t.pos_in_line, pos_in_line);

        tm_lexer_advance(&t, str, 1);
        if(*(t.value) == '\n') {
            line += 1;
            pos_in_line = 0;
        } else {
            pos_in_line += 1;
        }
    }
}
END_TEST

int main(int argc, char* argv[]) {
    Suite* s = suite_create("tests: param_lexer");

    // add tests
    TCase* tc_lexer = tcase_create("lexer");
    tcase_add_test(tc_lexer, test_lexer);
    tcase_add_test(tc_lexer, test_lexer_line);

    suite_add_tcase(s, tc_lexer);

    // run suite
    SRunner *sr = srunner_create(s) ;
    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    // exit
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}