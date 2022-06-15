#include <stdlib.h>
#include <stdio.h>


#include "xyz_parser.h"
#include "../tests.h"
#include "files.h"

int tm_xyz_parse_real(tm_parf_token* tk, char* input, double *out);
int tm_xyz_parse_positive_int(tm_parf_token* tk, char* input, long *out);
int tm_xyz_parse_atom_type(tm_parf_token* tk, char* input, char **out);


START_TEST(test_parser_positive_int) {
    char tmp[100];
    int val_int = 42;
    long val_int_found;
    int sz;

    tm_parf_token t;

    sprintf(tmp, "%d", val_int);
    _OK(tm_lexer_token_init(&t, tmp));

    _OK(tm_xyz_parse_positive_int(&t, tmp, &val_int_found));
    ck_assert_int_eq(val_int, val_int_found);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    // non-working stuffs
    char* wrong_examples[] = {
            "xy",
            ".3",
            "-3"
    };

    sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(int i=0; i < sz; i++) {
        _OK(tm_lexer_token_init(&t, wrong_examples[i]));
        _NOK(tm_xyz_parse_positive_int(&t, wrong_examples[i], &val_int_found));
    }
}
END_TEST


START_TEST(test_parser_real) {
    char tmp[100];
    double val_real = 3.1415, val_real_found;
    int sz;

    tm_parf_token t;

    sprintf(tmp, "%f", val_real);
    _OK(tm_lexer_token_init(&t, tmp));

    _OK(tm_xyz_parse_real(&t, tmp, &val_real_found));
    ck_assert_double_eq(val_real, val_real_found);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    // non-working stuffs
    char* wrong_examples[] = {
            "xy",
            ".",
            "+e3",
            "+.e-5"
    };

    sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(int i=0; i < sz; i++) {
        _OK(tm_lexer_token_init(&t, wrong_examples[i]));
        _NOK(tm_xyz_parse_real(&t, wrong_examples[i], &val_real_found));
    }
}
END_TEST

START_TEST(test_parser_atom_type) {
    char* correct_input[] = {
            "C",
            "Al",
            "O2",
    };

    tm_parf_token t;
    int sz;
    char* found;

    sz = sizeof(correct_input) / sizeof(*correct_input);
    for(int i=0; i < sz; i++) {
        _OK(tm_lexer_token_init(&t, correct_input[i]));
        _OK(tm_xyz_parse_atom_type(&t, correct_input[i], &found));
        ck_assert_str_eq(correct_input[i], found);
        ck_assert_int_eq(t.type, TM_TK_EOS);
        free(found);
    }

    // non-working stuffs
    char* wrong_examples[] = {
            "25",
            "2Z",
            "!C",
    };

    sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(int i=0; i < sz; i++) {
        _OK(tm_lexer_token_init(&t, wrong_examples[i]));
        _NOK(tm_xyz_parse_atom_type(&t, wrong_examples[i], &found));
    }

} END_TEST

START_TEST(test_read_file) {
    // open and read file
    FILE* f = fopen("test_dummy_geom.xyz", "r");
    ck_assert_ptr_nonnull(f);

    char* buffer;
    tm_read_file(f, &buffer);

    ck_assert_ptr_nonnull(buffer);
    fclose(f);

    // get geometry
    tm_geometry * g = tm_xyz_loads(buffer);
    ck_assert_ptr_nonnull(g);

    ck_assert_int_eq(g->N, 3);

    ck_assert_str_eq(g->type_vals[0], "O");
    ck_assert_str_eq(g->type_vals[1], "H");

    ck_assert_int_eq(g->types[0], 0);
    ck_assert_int_eq(g->types[1], 1);
    ck_assert_int_eq(g->types[2], 1);

    _OK(tm_geometry_delete(g));
    free(buffer);
}
END_TEST

START_TEST(test_read_errors) {
    tm_geometry * g = tm_xyz_loads("1"); // no title
    ck_assert_ptr_null(g);

    g = tm_xyz_loads("1\nX"); // no coordinates
    ck_assert_ptr_null(g);

    g = tm_xyz_loads("1\nX\nC .1"); // not enough coordinates
    ck_assert_ptr_null(g);

    g = tm_xyz_loads("2\nX\nC .1 .1 .1"); // less than expected
    ck_assert_ptr_null(g);

    g = tm_xyz_loads("1\nX\nC .1 .1 .1\nC .1 .1 .1"); // more than expected
    ck_assert_ptr_null(g);
}
END_TEST

int main(int argc, char* argv[]) {
    Suite* s = suite_create("tests: xyz_parser");

    // parser
    TCase* tc_parser = tcase_create("parser");
    tcase_add_test(tc_parser, test_parser_positive_int);
    tcase_add_test(tc_parser, test_parser_real);
    tcase_add_test(tc_parser, test_parser_atom_type);

    suite_add_tcase(s, tc_parser);

    // read
    TCase* tc_read = tcase_create("read");
    tcase_add_test(tc_read, test_read_file);
    tcase_add_test(tc_read, test_read_errors);

    suite_add_tcase(s, tc_read);

    // run suite
    SRunner *sr = srunner_create(s) ;
    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    // exit
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
