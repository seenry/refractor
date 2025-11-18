#pragma once
#include <stdio.h>
#include <stdint.h>
#include <cmath>

#define TESTCASE(label) \
uint8_t label(char* err_str_)
#define TEST_LIST \
    X(test_0)
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
