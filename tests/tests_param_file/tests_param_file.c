#include <check.h>
#include "param_file.h"
#include "tests_param_file.h"

/* Booleans */
#define BOOLEAN_VALUE 1
#define BOOLEAN_OTHER_VALUE 0
tm_parf_t* obj_boolean;

#define _OK(v) ck_assert_int_eq(v, 0)
#define _NOK(v) ck_assert_int_ne(v, 0)

void setup_boolean() {
    obj_boolean = tm_parf_boolean_new(BOOLEAN_VALUE);
}

void teardown_boolean() {
    _OK(tm_parf_delete(obj_boolean));
}

START_TEST(test_parf_boolean_create) {
    int v = -1;
    _OK(tm_parf_boolean_value(obj_boolean, &v));
    ck_assert_int_eq(v, BOOLEAN_VALUE);
}
END_TEST

START_TEST(test_parf_boolean_set) {
    int v = -1;
    _OK(tm_parf_boolean_set(obj_boolean, BOOLEAN_OTHER_VALUE));
    _OK(tm_parf_boolean_value(obj_boolean, &v));
    ck_assert_int_eq(v, BOOLEAN_OTHER_VALUE);
}
END_TEST

/* Integers */
#define INT_VALUE 15
#define INT_OTHER_VALUE 42
tm_parf_t* obj_integer;

void setup_integer() {
    obj_integer = tm_parf_integer_new(INT_VALUE);
}

void teardown_integer() {
    _OK(tm_parf_delete(obj_integer));
}

START_TEST(test_parf_integer_create) {
    int v = -1;
    _OK(tm_parf_integer_value(obj_integer, &v));
    ck_assert_int_eq(v, INT_VALUE);
}
END_TEST

START_TEST(test_parf_integer_set) {
    int v = -1;
    _OK(tm_parf_integer_set(obj_integer, INT_OTHER_VALUE));
    _OK(tm_parf_integer_value(obj_integer, &v));
    ck_assert_int_eq(v, INT_OTHER_VALUE);
}
END_TEST

/* Reals */
#define REAL_VALUE 3.14
#define REAL_OTHER_VALUE 42.42
tm_parf_t* obj_real;

void setup_real() {
    obj_real = tm_parf_real_new(REAL_VALUE);
}

void teardown_real() {
    _OK(tm_parf_delete(obj_real));
}

START_TEST(test_parf_real_create) {
   double v = -1;
   _OK(tm_parf_real_value(obj_real, &v));
   ck_assert_double_eq(v, REAL_VALUE);
}
END_TEST

START_TEST(test_parf_real_set) {
    double v = -1;
    _OK(tm_parf_real_set(obj_real, REAL_OTHER_VALUE));
    _OK(tm_parf_real_value(obj_real, &v));
    ck_assert_double_eq(v, REAL_OTHER_VALUE);
}
END_TEST

/* strings */
#define STRING_VALUE "x"
#define STRING_OTHER_VALUE "y"
tm_parf_t* obj_string;

void setup_string() {
    obj_string = tm_parf_string_new(STRING_VALUE);
}

void teardown_string() {
    _OK(tm_parf_delete(obj_string));
}

START_TEST(test_parf_string_create) {
    char* v = NULL;
    _OK(tm_parf_string_value(obj_string, &v));
    ck_assert_str_eq(v, STRING_VALUE);
}
END_TEST

START_TEST(test_parf_string_length) {
    unsigned int l = 0;
    _OK(tm_parf_string_length(obj_string, &l));
    ck_assert_uint_eq(l, strlen(STRING_VALUE));
}
END_TEST

START_TEST(test_parf_string_set) {
    char* v = NULL;
    _OK(tm_parf_string_set(obj_string, STRING_OTHER_VALUE));
    _OK(tm_parf_string_value(obj_string, &v));
    ck_assert_str_eq(v, STRING_OTHER_VALUE);
}
END_TEST


/* lists */
tm_parf_t* obj_list;

void setup_list() {
    obj_list = tm_parf_list_new();
}

void teardown_list() {
    _OK(tm_parf_delete(obj_list));
}

START_TEST(test_parf_list_append) {
    tm_parf_t* val = tm_parf_integer_new(INT_VALUE);
    tm_parf_t* val2 = tm_parf_integer_new(INT_VALUE);

    // first append
    ck_assert_uint_eq(obj_list->val_size, 0);
    _OK(tm_parf_list_append(obj_list, val));
    ck_assert_uint_eq(obj_list->val_size, 1);

    ck_assert_ptr_eq(obj_list->val_obj_or_list, val);
    ck_assert_ptr_eq(obj_list->last, val);

    // second append
    _OK(tm_parf_list_append(obj_list, val2));
    ck_assert_uint_eq(obj_list->val_size, 2);

    ck_assert_ptr_eq(obj_list->val_obj_or_list, val);
    ck_assert_ptr_eq(obj_list->val_obj_or_list->next, val2);
    ck_assert_ptr_eq(obj_list->last, val2);
}
END_TEST

START_TEST(test_parf_list_get) {
    tm_parf_t* val = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_list_append(obj_list, val));
    tm_parf_t* val2 = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_list_append(obj_list, val2));
    tm_parf_t* val3 = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_list_append(obj_list, val3));

    ck_assert_uint_eq(obj_list->val_size, 3);

    // get first
    tm_parf_t* elm;
    _OK(tm_parf_list_get(obj_list, 0, &elm));
    ck_assert_ptr_eq(elm, val);

    // get second
    _OK(tm_parf_list_get(obj_list, 1, &elm));
    ck_assert_ptr_eq(elm, val2);

    // get last
    _OK(tm_parf_list_get(obj_list, -1, &elm));
    ck_assert_ptr_eq(elm, val3);

    // get one before last
    _OK(tm_parf_list_get(obj_list, -2, &elm));
    ck_assert_ptr_eq(elm, val2);

    // fail because too large
    _NOK(tm_parf_list_get(obj_list, 5, &elm));
    _NOK(tm_parf_list_get(obj_list, -5, &elm));
}
END_TEST

START_TEST(test_parf_list_length) {
    unsigned int l = 0;
    _OK(tm_parf_list_length(obj_list, &l));
    ck_assert_uint_eq(l, 0);

    tm_parf_t* val = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_list_append(obj_list, val));
    _OK(tm_parf_list_length(obj_list, &l));
    ck_assert_uint_eq(l, 1);
}
END_TEST


START_TEST(test_parf_list_iterate) {

    tm_parf_t* tab[] = {
            tm_parf_integer_new(INT_VALUE),
            tm_parf_integer_new(INT_VALUE),
            tm_parf_integer_new(INT_VALUE)
    };

    for(int i=0; i < 3; i++)
        _OK(tm_parf_list_append(obj_list, tab[i]));

    tm_parf_t* obj;
    tm_parf_iterator * it = tm_parf_iterator_new(obj_list);
    int i = 0;

    while(tm_parf_iterator_has_next(it)) {
        _OK(tm_parf_iterator_next(it, &obj));
        ck_assert_ptr_eq(obj, tab[i]);
        i++;
    }

    ck_assert_int_eq(i, 3);

    tm_parf_iterator_delete(it);
}
END_TEST

/* objects */
tm_parf_t* obj_object;

void setup_object() {
    obj_object = tm_parf_object_new();
}

void teardown_object() {
    _OK(tm_parf_delete(obj_object));
}

START_TEST(test_parf_object_set) {
    tm_parf_t* val = tm_parf_integer_new(INT_VALUE);
    tm_parf_t* val2 = tm_parf_integer_new(INT_VALUE);
    tm_parf_t* val3 = tm_parf_integer_new(INT_VALUE);
    tm_parf_t* val4 = tm_parf_integer_new(INT_VALUE);
    tm_parf_t* val5 = tm_parf_integer_new(INT_VALUE);

    // first set
    ck_assert_uint_eq(obj_object->val_size, 0);
    _OK(tm_parf_object_set(obj_object, "key1", val));
    ck_assert_uint_eq(obj_object->val_size, 1);

    ck_assert_ptr_eq(obj_object->val_obj_or_list, val);
    ck_assert_ptr_eq(obj_object->last, val);

    // second set
    _OK(tm_parf_object_set(obj_object, "key2", val2));
    ck_assert_uint_eq(obj_object->val_size, 2);

    ck_assert_ptr_eq(obj_object->val_obj_or_list, val);
    ck_assert_ptr_eq(obj_object->val_obj_or_list->next, val2);
    ck_assert_ptr_eq(obj_object->last, val2);

    // reset a key that already exists (in last position)
    _OK(tm_parf_object_set(obj_object, "key2", val3));
    ck_assert_uint_eq(obj_object->val_size, 2);
    ck_assert_ptr_eq(obj_object->val_obj_or_list->next, val3);
    ck_assert_ptr_eq(obj_object->last, val3);

    // reset a key that already exist (in first position)
    _OK(tm_parf_object_set(obj_object, "key1", val4));
    ck_assert_uint_eq(obj_object->val_size, 2);
    ck_assert_ptr_eq(obj_object->val_obj_or_list, val4);
    ck_assert_ptr_eq(obj_object->val_obj_or_list->next, val3);

    // add yet another object
    _OK(tm_parf_object_set(obj_object, "key5", val5));
    ck_assert_uint_eq(obj_object->val_size, 3);
    ck_assert_ptr_eq(obj_object->val_obj_or_list->next->next, val5);
    ck_assert_ptr_eq(obj_object->last, val5);
}
END_TEST

START_TEST(test_parf_object_get) {
    tm_parf_t* val = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_object_set(obj_object, "key1", val));
    tm_parf_t* val2 = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_object_set(obj_object, "key2", val2));
    tm_parf_t* val3 = tm_parf_integer_new(INT_VALUE);
    _OK(tm_parf_object_set(obj_object, "key3", val3));

    ck_assert_uint_eq(obj_object->val_size, 3);

    // get first
    tm_parf_t* elm;
    _OK(tm_parf_object_get(obj_object, "key1", &elm));
    ck_assert_ptr_eq(elm, val);

    // get second
    _OK(tm_parf_object_get(obj_object, "key2", &elm));
    ck_assert_ptr_eq(elm, val2);

    // get last
    _OK(tm_parf_object_get(obj_object, "key3", &elm));
    ck_assert_ptr_eq(elm, val3);

    // get non-existing
    _NOK(tm_parf_object_get(obj_object, "whatever", &elm));
}
END_TEST

START_TEST(test_parf_object_iterate) {

    tm_parf_t* tab[] = {
            tm_parf_integer_new(INT_VALUE),
            tm_parf_integer_new(INT_VALUE),
            tm_parf_integer_new(INT_VALUE)
    };

    char* key[] = {
            "ab",
            "xe",
            "qxyz"
    };

    for(int i=0; i < 3; i++)
        _OK(tm_parf_object_set(obj_object, key[i], tab[i]));

    tm_parf_t* obj;
    tm_parf_iterator * it = tm_parf_iterator_new(obj_object);
    int i = 0;

    while(tm_parf_iterator_next(it, &obj) == 0) { // another (less safe?) form
        ck_assert_str_eq(obj->key, key[i]);
        ck_assert_ptr_eq(obj, tab[i]);
        i++;
    }

    ck_assert_int_eq(i, 3);
    tm_parf_iterator_delete(it);
}
END_TEST

void add_tests_param_file_cases(Suite* s) {
    // boolean
    TCase* tc_boolean = tcase_create("booleans");
    tcase_add_checked_fixture(tc_boolean, setup_boolean, teardown_boolean);
    tcase_add_test(tc_boolean, test_parf_boolean_create);
    tcase_add_test(tc_boolean, test_parf_boolean_set);

    suite_add_tcase(s, tc_boolean);

    // integers
    TCase* tc_integer = tcase_create("integers");
    tcase_add_checked_fixture(tc_integer, setup_integer, teardown_integer);
    tcase_add_test(tc_integer, test_parf_integer_create);
    tcase_add_test(tc_integer, test_parf_integer_set);

    suite_add_tcase(s, tc_integer);

    // reals
    TCase* tc_real = tcase_create("reals");
    tcase_add_checked_fixture(tc_real, setup_real, teardown_real);
    tcase_add_test(tc_real, test_parf_real_create);
    tcase_add_test(tc_real, test_parf_real_set);

    suite_add_tcase(s, tc_real);

    // strings
    TCase* tc_string = tcase_create("strings");
    tcase_add_checked_fixture(tc_string, setup_string, teardown_string);
    tcase_add_test(tc_string, test_parf_string_create);
    tcase_add_test(tc_string, test_parf_string_length);
    tcase_add_test(tc_string, test_parf_string_set);

    suite_add_tcase(s, tc_string);

    // lists
    TCase* tc_list = tcase_create("lists");
    tcase_add_checked_fixture(tc_list, setup_list, teardown_list);
    tcase_add_test(tc_list, test_parf_list_append);
    tcase_add_test(tc_list, test_parf_list_get);
    tcase_add_test(tc_list, test_parf_list_length);
    tcase_add_test(tc_list, test_parf_list_iterate);

    suite_add_tcase(s, tc_list);

    // objects
    TCase* tc_object = tcase_create("objects");
    tcase_add_checked_fixture(tc_object, setup_object, teardown_object);
    tcase_add_test(tc_object, test_parf_object_set);
    tcase_add_test(tc_object, test_parf_object_get);
    tcase_add_test(tc_object, test_parf_object_iterate);

    suite_add_tcase(s, tc_object);
}
