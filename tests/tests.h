#ifndef TOYMC_TESTS_H
#define TOYMC_TESTS_H

#include <check.h>

#define _OK(v) ck_assert_int_eq(v, 0)
#define _NOK(v) ck_assert_int_ne(v, 0)

#endif //TOYMC_TESTS_H
