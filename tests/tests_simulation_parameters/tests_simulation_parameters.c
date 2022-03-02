#include "tests_simulation_parameters.h"

START_TEST(test_read) {

}
END_TEST

void add_tests_simulation_parameters_cases(Suite* s) {
    // read
    TCase* tc_lexer = tcase_create("read");
    tcase_add_test(tc_lexer, test_read);

    suite_add_tcase(s, tc_lexer);
}