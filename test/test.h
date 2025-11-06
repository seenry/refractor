#pragma once
#include <stdio.h>
#include <stdint.h>
#include <cmath>

#define TESTCASE(label) \
uint8_t label(char* err_str_)

#define FLOAT_TOL ( 1e-6f )

#define ASSERT(cond) do { \
    if (!(cond)) { \
        snprintf(err_str_, 256, "Assert: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

#define ASSERT_BLAME(cond, msg) do { \
    if (!(cond)) { \
        snprintf(err_str_, 256, "Assert: %s (%s:%d)--%s\n", #cond, __FILE__, __LINE__, msg); \
        return 1; \
    } \
} while (0)

TESTCASE(test_curve_0);
TESTCASE(test_curve_1);
