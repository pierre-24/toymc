#include <check.h>
#include "input_file.h"

/* Booleans */
#define BOOLEAN_VALUE 1
#define BOOLEAN_OTHER_VALUE 0
tm_infi_t* obj_boolean;

void setup_boolean() {
    obj_boolean = tm_infi_boolean_new(BOOLEAN_VALUE);
}

void teardown_boolean() {
    tm_infi_delete(obj_boolean);
}

START_TEST(test_infi_boolean_create) {
    int v = -1;
    tm_infi_boolean_value(obj_boolean, &v);
    ck_assert_int_eq(v, BOOLEAN_VALUE);
}
END_TEST

START_TEST(test_infi_boolean_set) {
    int v = -1;
    tm_infi_boolean_set(obj_boolean, BOOLEAN_OTHER_VALUE);
    tm_infi_boolean_value(obj_boolean, &v);
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
    tm_infi_delete(obj_integer);
}

START_TEST(test_infi_integer_create) {
    int v = -1;
    tm_infi_integer_value(obj_integer, &v);
    ck_assert_int_eq(v, INT_VALUE);
}
END_TEST

START_TEST(test_infi_integer_set) {
    int v = -1;
    tm_infi_integer_set(obj_integer, INT_OTHER_VALUE);
    tm_infi_integer_value(obj_integer, &v);
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
    tm_infi_delete(obj_real);
}

START_TEST(test_infi_real_create) {
   double v = -1;
   tm_infi_real_value(obj_real, &v);
   ck_assert_double_eq(v, REAL_VALUE);
}
END_TEST

START_TEST(test_infi_real_set) {
    double v = -1;
    tm_infi_real_set(obj_real, REAL_OTHER_VALUE);
    tm_infi_real_value(obj_real, &v);
    ck_assert_double_eq(v, REAL_OTHER_VALUE);
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
    // tcase_add_test(tc_real, test_infi_real_set);

    suite_add_tcase(s, tc_real);
}
