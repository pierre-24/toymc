#include <check.h>
#include "input_file.h"

/* Booleans */
#define BOOLEAN_VALUE 1
#define BOOLEAN_OTHER_VALUE 0
tm_infi_t* obj_boolean;

#define _OK(v) ck_assert_int_eq(v, 0)
#define _NOK(v) ck_assert_int_ne(v, 0)

void setup_boolean() {
    obj_boolean = tm_infi_boolean_new(BOOLEAN_VALUE);
}

void teardown_boolean() {
    _OK(tm_infi_delete(obj_boolean));
}

START_TEST(test_infi_boolean_create) {
    int v = -1;
    _OK(tm_infi_boolean_value(obj_boolean, &v));
    ck_assert_int_eq(v, BOOLEAN_VALUE);
}
END_TEST

START_TEST(test_infi_boolean_set) {
    int v = -1;
    _OK(tm_infi_boolean_set(obj_boolean, BOOLEAN_OTHER_VALUE));
    _OK(tm_infi_boolean_value(obj_boolean, &v));
    ck_assert_int_eq(v, BOOLEAN_OTHER_VALUE);
}
END_TEST

/* Integers */
#define INT_VALUE 15
#define INT_OTHER_VALUE 42
tm_infi_t* obj_integer;

void setup_integer() {
    obj_integer = tm_infi_integer_new(INT_VALUE);
}

void teardown_integer() {
    _OK(tm_infi_delete(obj_integer));
}

START_TEST(test_infi_integer_create) {
    int v = -1;
    _OK(tm_infi_integer_value(obj_integer, &v));
    ck_assert_int_eq(v, INT_VALUE);
}
END_TEST

START_TEST(test_infi_integer_set) {
    int v = -1;
    _OK(tm_infi_integer_set(obj_integer, INT_OTHER_VALUE));
    _OK(tm_infi_integer_value(obj_integer, &v));
    ck_assert_int_eq(v, INT_OTHER_VALUE);
}
END_TEST

/* Reals */
#define REAL_VALUE 3.14
#define REAL_OTHER_VALUE 42.42
tm_infi_t* obj_real;

void setup_real() {
    obj_real = tm_infi_real_new(REAL_VALUE);
}

void teardown_real() {
    _OK(tm_infi_delete(obj_real));
}

START_TEST(test_infi_real_create) {
   double v = -1;
   _OK(tm_infi_real_value(obj_real, &v));
   ck_assert_double_eq(v, REAL_VALUE);
}
END_TEST

START_TEST(test_infi_real_set) {
    double v = -1;
    _OK(tm_infi_real_set(obj_real, REAL_OTHER_VALUE));
    _OK(tm_infi_real_value(obj_real, &v));
    ck_assert_double_eq(v, REAL_OTHER_VALUE);
}
END_TEST

/* strings */
#define STRING_VALUE "x"
#define STRING_OTHER_VALUE "y"
tm_infi_t* obj_string;

void setup_string() {
    obj_string = tm_infi_string_new(STRING_VALUE);
}

void teardown_string() {
    _OK(tm_infi_delete(obj_string));
}

START_TEST(test_infi_string_create) {
    char* v = NULL;
    _OK(tm_infi_string_value(obj_string, &v));
    ck_assert_str_eq(v, STRING_VALUE);
}
END_TEST

START_TEST(test_infi_string_length) {
    unsigned int l = 0;
    _OK(tm_infi_string_length(obj_string, &l));
    ck_assert_uint_eq(l, strlen(STRING_VALUE));
}
END_TEST

START_TEST(test_infi_string_set) {
    char* v = NULL;
    _OK(tm_infi_string_set(obj_string, STRING_OTHER_VALUE));
    _OK(tm_infi_string_value(obj_string, &v));
    ck_assert_str_eq(v, STRING_OTHER_VALUE);
}
END_TEST


/* arrays */
tm_infi_t* obj_array;

void setup_array() {
    obj_array = tm_infi_array_new();
}

void teardown_array() {
    _OK(tm_infi_delete(obj_array));
}

START_TEST(test_infi_array_append) {
    tm_infi_t* val = tm_infi_integer_new(INT_VALUE);
    tm_infi_t* val2 = tm_infi_integer_new(INT_VALUE);

    // first append
    ck_assert_uint_eq(obj_array->val_size, 0);
    _OK(tm_infi_array_append(obj_array, val));
    ck_assert_uint_eq(obj_array->val_size, 1);

    ck_assert_ptr_eq(obj_array->val_obj_or_list, val);
    ck_assert_ptr_eq(obj_array->last, val);

    // second append
    _OK(tm_infi_array_append(obj_array, val2));
    ck_assert_uint_eq(obj_array->val_size, 2);

    ck_assert_ptr_eq(obj_array->val_obj_or_list, val);
    ck_assert_ptr_eq(obj_array->val_obj_or_list->next, val2);
    ck_assert_ptr_eq(obj_array->last, val2);
}
END_TEST

START_TEST(test_infi_array_get) {
    tm_infi_t* val = tm_infi_integer_new(INT_VALUE);
    _OK(tm_infi_array_append(obj_array, val));
    tm_infi_t* val2 = tm_infi_integer_new(INT_VALUE);
    _OK(tm_infi_array_append(obj_array, val2));
    tm_infi_t* val3 = tm_infi_integer_new(INT_VALUE);
    _OK(tm_infi_array_append(obj_array, val3));

    ck_assert_uint_eq(obj_array->val_size, 3);

    // get first
    tm_infi_t* elm;
    _OK(tm_infi_array_get(obj_array, 0, &elm));
    ck_assert_ptr_eq(elm, val);

    // get second
    _OK(tm_infi_array_get(obj_array, 1, &elm));
    ck_assert_ptr_eq(elm, val2);

    // get last
    _OK(tm_infi_array_get(obj_array, -1, &elm));
    ck_assert_ptr_eq(elm, val3);

    // get one before last
    _OK(tm_infi_array_get(obj_array, -2, &elm));
    ck_assert_ptr_eq(elm, val2);

    // fail because too large
    _NOK(tm_infi_array_get(obj_array, 5, &elm));
    _NOK(tm_infi_array_get(obj_array, -5, &elm));
}
END_TEST

START_TEST(test_infi_array_length) {
    unsigned int l = 0;
    _OK(tm_infi_array_length(obj_array, &l));
    ck_assert_uint_eq(l, 0);

    tm_infi_t* val = tm_infi_integer_new(INT_VALUE);
    _OK(tm_infi_array_append(obj_array, val));
    _OK(tm_infi_array_length(obj_array, &l));
    ck_assert_uint_eq(l, 1);
}
END_TEST

void add_test_cases(Suite* s) {
    // boolean
    TCase* tc_boolean = tcase_create("booleans");
    tcase_add_checked_fixture(tc_boolean, setup_boolean, teardown_boolean);
    tcase_add_test(tc_boolean, test_infi_boolean_create);
    tcase_add_test(tc_boolean, test_infi_boolean_set);

    suite_add_tcase(s, tc_boolean);

    // integers
    TCase* tc_integer = tcase_create("integers");
    tcase_add_checked_fixture(tc_integer, setup_integer, teardown_integer);
    tcase_add_test(tc_integer, test_infi_integer_create);
    tcase_add_test(tc_integer, test_infi_integer_set);

    suite_add_tcase(s, tc_integer);

    // reals
    TCase* tc_real = tcase_create("reals");
    tcase_add_checked_fixture(tc_real, setup_real, teardown_real);
    tcase_add_test(tc_real, test_infi_real_create);
    tcase_add_test(tc_real, test_infi_real_set);

    suite_add_tcase(s, tc_real);

    // strings
    TCase* tc_string = tcase_create("strings");
    tcase_add_checked_fixture(tc_string, setup_string, teardown_string);
    tcase_add_test(tc_string, test_infi_string_create);
    tcase_add_test(tc_string, test_infi_string_length);
    tcase_add_test(tc_string, test_infi_string_set);

    suite_add_tcase(s, tc_string);

    // arrays
    TCase* tc_array = tcase_create("arrays");
    tcase_add_checked_fixture(tc_array, setup_array, teardown_array);
    tcase_add_test(tc_array, test_infi_array_append);
    tcase_add_test(tc_array, test_infi_array_get);
    tcase_add_test(tc_array, test_infi_array_length);

    suite_add_tcase(s, tc_array);
}
