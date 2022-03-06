#include <check.h>
#include "param_file_parser.h"

#define _OK(v) ck_assert_int_eq(v, 0)
#define _NOK(v) ck_assert_int_ne(v, 0)

int tm_parf_lexer(tm_parf_token *tk, char *input, int shift);
tm_parf_t* tm_parf_parse_string(tm_parf_token* tk, char* input, tm_parf_error* error);
tm_parf_t* tm_parf_parse_number(tm_parf_token* tk, char* input, tm_parf_error* error);
tm_parf_t* tm_parf_parse_boolean(tm_parf_token* tk, char* input, tm_parf_error* error);
tm_parf_t* tm_parf_parse_list(tm_parf_token* tk, char* input, tm_parf_error* error);

tm_parf_error e;

tm_parf_t *parse_string(tm_parf_token *t, char *input) {
    _OK(tm_parf_token_init(t, input));
    return tm_parf_parse_string(t, input, &e);
}

tm_parf_t *parse_number(tm_parf_token *t, char *input) {
    _OK(tm_parf_token_init(t, input));
    return tm_parf_parse_number(t, input, &e);
}

tm_parf_t *parse_boolean(tm_parf_token *t, char *input) {
    _OK(tm_parf_token_init(t, input));
    return tm_parf_parse_boolean(t, input, &e);
}

tm_parf_t *parse_list(tm_parf_token *t, char *input) {
    _OK(tm_parf_token_init(t, input));
    return tm_parf_parse_list(t, input, &e);
}

START_TEST(test_lexer) {
    char* str = "ab[-9]";
    int l = strlen(str);
    tm_parf_token t;
    _OK(tm_parf_token_init(&t, str));

    tm_parf_token_type tab[] = {
            TM_TK_CHAR, TM_TK_CHAR, TM_TK_LBRACKET, TM_TK_DASH, TM_TK_DIGIT, TM_TK_RBRACKET, TM_TK_EOS};

    for(int i=0; i <= l; i++) {
        ck_assert_int_eq(t.type, tab[i]);
        tm_parf_lexer(&t, str, 1);
    }
}
END_TEST

START_TEST(test_lexer_line) {
    char* str = "a\nb1\ncde\nf2";
    int l = strlen(str);
    tm_parf_token t;
    _OK(tm_parf_token_init(&t, str));
    int line = 1, pos_in_line = 0;

    for(int i=0; i < l; i++) {
        ck_assert_int_eq(t.line, line);
        ck_assert_int_eq(t.pos_in_line, pos_in_line);

        tm_parf_lexer(&t, str, 1);
        if(*(t.value) == '\n') {
            line += 1;
            pos_in_line = 0;
        } else {
            pos_in_line += 1;
        }
    }
}
END_TEST

START_TEST(test_parser_string) {
    char tmp[100];
    char *val, *found;
    tm_parf_token t;

    // first test
    val = "xyz";
    sprintf(tmp, "\"%s\"", val);

    tm_parf_t* obj = parse_string(&t, tmp);
    _OK(tm_parf_string_value(obj, &found));
    ck_assert_str_eq(found, val);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_parf_delete(obj);

    // second test (with quote in it)
    val = "a \\\"lapin\\\" is a rabbit, in french";
    sprintf(tmp, "\"%s\"", val);
    tm_parf_lexer(&t, tmp, 0);

    obj = parse_string(&t, tmp);
    _OK(tm_parf_string_value(obj, &found));
    ck_assert_str_eq(found, val);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_parf_delete(obj);

    // error
    val = "\"a non-finished one";
    ck_assert_ptr_null(parse_string(&t, val));
}
END_TEST

START_TEST(test_parser_number) {
    char tmp[100];
    int val_int = 42;
    long val_int_found;
    double val_real = 3.1415, val_real_found;

    tm_parf_token t;

    // integer
    sprintf(tmp, "%d", val_int);
    tm_parf_t* obj = parse_number(&t, tmp);
    _OK(tm_parf_integer_value(obj, &val_int_found));
    ck_assert_int_eq(val_int, val_int_found);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_parf_delete(obj);

    // real
    sprintf(tmp, "%f", val_real);

    obj = parse_number(&t, tmp);
    _OK(tm_parf_real_value(obj, &val_real_found));
    ck_assert_double_eq(val_real, val_real_found);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_parf_delete(obj);

    // other working stuffs
    char* examples[] = {
            ".25",
            "-5",
            "+3",
            "1e-3",
            ".5e+6",
    };

    int sz = sizeof(examples) / sizeof(*examples);
    for(int i=0; i < sz; i++) {
        obj = parse_number(&t, examples[i]);
        ck_assert_int_eq(t.type, TM_TK_EOS);
        tm_parf_delete(obj);
    }

    // non-working stuffs
    char* wrong_examples[] = {
            "xy",
            ".",
            "+e3",
            "+.e-5"
    };

    sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(int i=0; i < sz; i++) {
        ck_assert_ptr_null(parse_number(&t, wrong_examples[i]));
    }
}
END_TEST

START_TEST(test_parser_boolean) {
    tm_parf_token t;
    int found;
    tm_parf_t* obj;

    char* true[] = {"true", "yes", "on"};
    char* false[] = {"false", "no", "off"};

    for(int i=0; i < 3; i++) {
        obj = parse_boolean(&t, true[i]);
        _OK(tm_parf_boolean_value(obj, &found));
        ck_assert_int_eq(found, 1);
        ck_assert_int_eq(t.type, TM_TK_EOS);
        tm_parf_delete(obj);

        obj = parse_boolean(&t, false[i]);
        _OK(tm_parf_boolean_value(obj, &found));
        ck_assert_int_eq(found, 0);
        ck_assert_int_eq(t.type, TM_TK_EOS);
        tm_parf_delete(obj);
    }

    // non-working stuffs
    char* wrong_examples[] = {
            "xy",
            "tru",
            "folse",
            "+true"
    };

    int sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(int i=0; i < sz; i++) {
        ck_assert_ptr_null(parse_boolean(&t, wrong_examples[i]));
    }
}
END_TEST

START_TEST(test_parser_list) {
    char tmp[100];
    int value[] = {
            3, 42, -6
    };
    tm_parf_token t;

    sprintf(tmp, "[%d %d %d]", value[0], value[1], value[2]);

    tm_parf_t* obj_list = parse_list(&t, tmp);
    tm_parf_t* obj;
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_parf_iterator * it = tm_parf_iterator_new(obj_list);
    int i = 0;
    long val;

    while(tm_parf_iterator_has_next(it)) {
        _OK(tm_parf_iterator_next(it, &obj));
        _OK(tm_parf_integer_value(obj, &val));
        ck_assert_int_eq(val, value[i]);
        i++;
    }

    ck_assert_int_eq(i, 3);

    tm_parf_delete(obj_list);
    tm_parf_iterator_delete(it);

    // non-working stuffs
    char* wrong_examples[] = {
            "[",
            "[2",
            "[2 [1 2]",
            "[2 [1 2",
            "[2 x",
            "[2 3"
    };

    int sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(i=0; i < sz; i++) {
        ck_assert_ptr_null(parse_list(&t, wrong_examples[i]));
    }
}
END_TEST

START_TEST(test_parser_object) {
    char tmp[100];
    char* key[] = {
            "1key", "_key2", "key-3"
    };
    int value[] = {
            3, 42, -6
    };

    sprintf(tmp, "%s %d %s %d # comment\n%s %d", key[0], value[0], key[1], value[1], key[2], value[2]);

    tm_parf_t* obj_object = tm_parf_loads(tmp, &e);
    tm_parf_t* elmt;

    tm_parf_iterator * it = tm_parf_iterator_new(obj_object);
    int i = 0;
    long val;

    while(tm_parf_iterator_has_next(it)) {
        _OK(tm_parf_iterator_next(it, &elmt));
        _OK(tm_parf_integer_value(elmt, &val));
        ck_assert_int_eq(val, value[i]);
        ck_assert_str_eq(elmt->key, key[i]);
        i++;
    }

    ck_assert_int_eq(i, 3);

    tm_parf_delete(obj_object);
    tm_parf_iterator_delete(it);

    // non-working stuffs
    char* wrong_examples[] = {
            "a", // missing value
            "a x", // unexpected token CHAR
            "a [2", // missing RBRACKET
            "a \"x", // missing QUOTE
            "a 2 b2" // that does not work since b2 is considered as a literal name, missing value
    };

    int sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(i=0; i < sz; i++) {
        ck_assert_ptr_null(tm_parf_loads(wrong_examples[i], &e));
    }
}
END_TEST


int main(int argc, char* argv[]) {
    Suite* s = suite_create("tests: param_file_parser");

    // lexer
    TCase* tc_lexer = tcase_create("lexer");
    tcase_add_test(tc_lexer, test_lexer);
    tcase_add_test(tc_lexer, test_lexer_line);

    suite_add_tcase(s, tc_lexer);

    // parser
    TCase* tc_parser = tcase_create("parser");
    tcase_add_test(tc_parser, test_parser_string);
    tcase_add_test(tc_parser, test_parser_number);
    tcase_add_test(tc_parser, test_parser_boolean);
    tcase_add_test(tc_parser, test_parser_list);
    tcase_add_test(tc_parser, test_parser_object);

    suite_add_tcase(s, tc_parser);

    // run suite
    SRunner *sr = srunner_create(s) ;
    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    // exit
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
