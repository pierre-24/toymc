#include <check.h>

START_TEST(test_infi_object_create) {
    ck_assert(1);
}
END_TEST


void add_test_cases(Suite* s) {
    TCase* tc = tcase_create("input files");

    // add tests
    tcase_add_test(tc, test_infi_object_create);

    suite_add_tcase(s, tc);
}
