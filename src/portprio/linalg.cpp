#include "portprio/linalg.h"

Matrix::Matrix(int y, int x) {
	dims[0] = y;
	dims[1] = x;

	d = PARLAY_M0(y, x);
}

Matrix::Matrix(std::function<float(size_t)> gen, int y, int x) {
	dims[0] = y;
	dims[1] = x;
	d = parlay::tabulate(y, [&](size_t j) {
		(void) j;
		return parlay::tabulate(x, gen);
	});
}

Matrix Matrix::transpose(Matrix& m) {
	Matrix result(m.dims[1], m.dims[0]);
	parlay::parallel_for(0, m.dims[0], [&](size_t i) {
		for (size_t j = 0; j < m.dims[1]; j++) {
			result.d[j][i] = m.d[i][j];
		}
	});
	return result;
}

Matrix Matrix::mult(Matrix& a, Matrix& b) {
	Matrix result(a.dims[0], b.dims[1]);
	parlay::parallel_for(0, a.dims[0], [&](size_t i) {
		for (size_t j = 0; j < b.dims[1]; j++) {
			float sum = 0.0f;
			for (size_t k = 0; k < a.dims[1]; k++) {
				sum += a.d[i][k] * b.d[k][j];
			}
			result.d[i][j] = sum;
		}
	});
	return result;
}
