#include <stdlib.h>
#include "simulation_parameters.h"
#include "tests.h"

tm_simulation_parameters* sp;

START_TEST(test_read) {
    sp = tm_simulation_parameters_new();
    ck_assert_ptr_nonnull(sp);

    FILE* f = fopen("test_dummy_input.inp", "r");
    ck_assert_ptr_nonnull(f);

    _OK(tm_simulation_parameters_read(sp, f));

    fclose(f);
    _OK(tm_simulation_parameters_delete(sp));
}
END_TEST

int main(int argc, char* argv[]) {
    Suite* s = suite_create("tests: simulation_parameters");

    // read
    TCase* tc_lexer = tcase_create("read");
    tcase_add_test(tc_lexer, test_read);

    suite_add_tcase(s, tc_lexer);

    // run suite
    SRunner *sr = srunner_create(s) ;
    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    // exit
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
