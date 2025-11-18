#pragma once

#include <array>
#include <functional>

#include "parlay.h"

#define PARLAY_V0(n) (parlay::tabulate(n, [&](size_t i__) { \
	(void) i__; return 0.0f; \
}))
#define PARLAY_M0(m, n) (parlay::tabulate(m, [&](size_t j__) { \
	(void) j__; \
	return PARLAY_V0(n); \
}))

class Matrix {
public:
  parlay::sequence<parlay::sequence<float>> d;
	std::array<int, 2> dims;

	Matrix() : dims{0, 0} {}
	Matrix(int x, int y);
	Matrix(std::function<float(size_t)> gen, int x, int y);
};

