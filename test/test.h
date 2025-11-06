#pragma once
#include <stdio.h>
#include <stdint.h>

#define TESTCASE(label) \
uint8_t label(char* err_str_)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        snprintf(err_str_, 256, "Assert: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

TESTCASE(test_0);
