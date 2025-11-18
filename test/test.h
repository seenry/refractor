#pragma once
#include <stdio.h>
#include <stdint.h>
#include <cmath>

#define TESTCASE(label) \
uint8_t label(char* err_str_)
#define TEST_LIST \
    X(portprio_linalg_0) \
    X(portprio_linalg_1) \
    X(portprio_linalg_2) \
    X(evo2014_chr_0_0) \
    X(evo2014_pop_0_0) \
    X(evo2014_pop_0_1)
#define X(name) TESTCASE(name);
TEST_LIST
#undef X

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

#define F_EQ(a, b) (std::abs((a) - (b)) < 1e-6f)
