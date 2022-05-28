#ifndef TOYMC_TESTS_H
#define TOYMC_TESTS_H

#include <check.h>

#include "errors.h"

#define _OK(v) ck_assert_int_eq(v, TM_ERR_OK)
#define _NOK(v) ck_assert_int_ne(v, TM_ERR_OK)

#endif //TOYMC_TESTS_H
