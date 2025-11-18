#include "portprio/linalg.h"

Matrix::Matrix(int x, int y) {
	dims[0] = y;
	dims[1] = x;

	d = PARLAY_M0(y, x);
}

Matrix::Matrix(std::function<float(size_t)> gen, int x, int y) {
	dims[0] = y;
	dims[1] = x;
	d = parlay::tabulate(y, [&](size_t j) {
		(void) j;
		return parlay::tabulate(x, gen);
	});
}

