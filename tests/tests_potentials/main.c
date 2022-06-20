#include <stdlib.h>
#include <stdio.h>

#include "../tests.h"
#include "potentials.h"


START_TEST(test_compute_LJ_3p_with_PBC) {
    /**
     * +-----+-----+
     * |2    |2    |
     * |1   3|1   3|
     * +-----+-----+
     * |2    |2    |
     * |1   3|1   3|
     * +-----+-----+
     *
     * BOX = (1, 1)
     * P1 = (.2, .2)
     * P2 = (.2, .8)
     * P3 = (.8, .2)
     *
     * distances::
     *
     *     | 1  | 2  | 3
     * ----+----+----+----
     * 1   |  1 | r1 | r1
     * 2   |  - | 1  | r2
     * 3   |  - | -  | 1
     * with r1 = 0.4, r2 = sqrt(0.4²+0.6²)
     */

}
END_TEST

int main(int argc, char* argv[]) {
    Suite* s = suite_create("tests: potentials");

    // LJ
    TCase* tc_LJ = tcase_create("LJ");
    tcase_add_test(tc_LJ, test_compute_LJ_3p_with_PBC);

    suite_add_tcase(s, tc_LJ);

    // run suite
    SRunner *sr = srunner_create(s) ;
    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    // exit
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
