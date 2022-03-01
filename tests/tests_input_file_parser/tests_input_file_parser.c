#include <check.h>
#include "input_file_parser.h"
#include "tests_input_file_parser.h"

#define _OK(v) ck_assert_int_eq(v, 0)
#define _NOK(v) ck_assert_int_ne(v, 0)

int tm_infi_lexer(tm_infi_token *tk, char *input, int position);
tm_infi_t* tm_infi_parse_string(tm_infi_token* tk, char* input, tm_infi_error* error);
tm_infi_t* tm_infi_parse_number(tm_infi_token* tk, char* input, tm_infi_error* error);
tm_infi_t* tm_infi_parse_boolean(tm_infi_token* tk, char* input, tm_infi_error* error);
tm_infi_t* tm_infi_parse_list(tm_infi_token* tk, char* input, tm_infi_error* error);

tm_infi_t *parse_string(tm_infi_token *t, char *input) {
    tm_infi_error e;
    _OK(tm_infi_lexer(t, input, 0));
    return tm_infi_parse_string(t, input, &e);
}

tm_infi_t *parse_number(tm_infi_token *t, char *input) {
    tm_infi_error e;
    _OK(tm_infi_lexer(t, input, 0));
    return tm_infi_parse_number(t, input, &e);
}

tm_infi_t *parse_boolean(tm_infi_token *t, char *input) {
    tm_infi_error e;
    _OK(tm_infi_lexer(t, input, 0));
    return tm_infi_parse_boolean(t, input, &e);
}

tm_infi_t *parse_list(tm_infi_token *t, char *input) {
    tm_infi_error e;
    _OK(tm_infi_lexer(t, input, 0));
    return tm_infi_parse_list(t, input, &e);
}

START_TEST(test_lexer) {
    char* str = "ab[-9]";
    int l = strlen(str);
    tm_infi_token t;

    tm_infi_token_type tab[] = {
            TM_TK_CHAR, TM_TK_CHAR, TM_TK_LBRACKET, TM_TK_DASH, TM_TK_DIGIT, TM_TK_RBRACKET, TM_TK_EOS};

    for(int i=0; i <= l; i++) {
        tm_infi_lexer(&t, str, i);
        ck_assert_int_eq(t.type, tab[i]);
    }
}
END_TEST

START_TEST(test_parser_string) {
    char tmp[100];
    char *val, *found;
    tm_infi_token t;

    // first test
    val = "xyz";
    sprintf(tmp, "\"%s\"", val);

    tm_infi_t* obj = parse_string(&t, tmp);
    _OK(tm_infi_string_value(obj, &found));
    ck_assert_str_eq(found, val);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_delete(obj);

    // second test (with quote in it)
    val = "a \\\"lapin\\\" is a rabbit, in french";
    sprintf(tmp, "\"%s\"", val);
    tm_infi_lexer(&t, tmp, 0);

    obj = parse_string(&t, tmp);
    _OK(tm_infi_string_value(obj, &found));
    ck_assert_str_eq(found, val);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_delete(obj);

    // error
    val = "\"a non-finished one";
    ck_assert_ptr_null(parse_string(&t, val));
}
END_TEST

START_TEST(test_parser_number) {
    char tmp[100];
    int val_int = 42, val_int_found;
    double val_real = 3.1415, val_real_found;

    tm_infi_token t;

    // integer
    sprintf(tmp, "%d", val_int);
    tm_infi_t* obj = parse_number(&t, tmp);
    _OK(tm_infi_integer_value(obj, &val_int_found));
    ck_assert_int_eq(val_int, val_int_found);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_delete(obj);

    // real
    sprintf(tmp, "%f", val_real);

    obj = parse_number(&t, tmp);
    _OK(tm_infi_real_value(obj, &val_real_found));
    ck_assert_double_eq(val_real, val_real_found);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_delete(obj);

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
        tm_infi_delete(obj);
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
    tm_infi_token t;
    int found;

    // true
    tm_infi_t* obj = parse_boolean(&t, "true");
    _OK(tm_infi_boolean_value(obj, &found));
    ck_assert_int_eq(found, 1);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_delete(obj);

    // false
    obj = parse_boolean(&t, "false");
    _OK(tm_infi_boolean_value(obj, &found));
    ck_assert_int_eq(found, 0);
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_delete(obj);

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
    tm_infi_token t;

    sprintf(tmp, "[%d, %d, %d]", value[0], value[1], value[2]);

    tm_infi_t* obj_list = parse_list(&t, tmp);
    tm_infi_t* obj;
    ck_assert_int_eq(t.type, TM_TK_EOS);

    tm_infi_iterator * it = tm_infi_iterator_new(obj_list);
    int i = 0, val;

    while(tm_infi_iterator_has_next(it)) {
        _OK(tm_infi_iterator_next(it, &obj));
        _OK(tm_infi_integer_value(obj, &val));
        ck_assert_int_eq(val, value[i]);
        i++;
    }

    ck_assert_int_eq(i, 3);

    tm_infi_delete(obj_list);
    tm_infi_iterator_delete(it);

    // non-working stuffs
    char* wrong_examples[] = {
            "[",
            "[2",
            "[2,",
            "[2,x",
            "[2,3"
    };

    int sz = sizeof(wrong_examples) / sizeof(*wrong_examples);
    for(i=0; i < sz; i++) {
        ck_assert_ptr_null(parse_list(&t, wrong_examples[i]));
    }
}
END_TEST

void add_tests_input_file_parser_cases(Suite* s) {
    // lexer
    TCase* tc_lexer = tcase_create("lexer");
    tcase_add_test(tc_lexer, test_lexer);

    suite_add_tcase(s, tc_lexer);

    // parser
    TCase* tc_parser = tcase_create("parser");
    tcase_add_test(tc_parser, test_parser_string);
    tcase_add_test(tc_parser, test_parser_number);
    tcase_add_test(tc_parser, test_parser_boolean);
    tcase_add_test(tc_parser, test_parser_list);

    suite_add_tcase(s, tc_parser);
}
