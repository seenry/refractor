#pragma once
#include <stdio.h>
#include <stdint.h>
#include <cmath>

#define TESTCASE(label) \
uint8_t label(char* err_str_)

#define F32_TOL ( 1e-5f )

#define ASSERT(cond) do { \
    if (!(cond)) { \
        snprintf(err_str_, 256, "(%s:%d) :: ASSERT( %s )\n", __FILE__, __LINE__, #cond); \
        return 1; \
    } \
} while (0)

#define ASSERT_BLAME(cond, msg) do { \
    if (!(cond)) { \
        snprintf(err_str_, 256, "(%s:%d) :: ASSERT( %s )\n  %s\n\n", __FILE__, __LINE__, #cond , msg); \
        return 1; \
    } \
} while (0)

TESTCASE(test_surf_init);
TESTCASE(test_surf_0);
TESTCASE(test_surf_1);
TESTCASE(test_tesselation_0);
