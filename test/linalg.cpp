#include "test.h"

#include "portprio/linalg.h"

TESTCASE(portprio_linalg_0) {
	Matrix mat_0(3, 5);
	ASSERT(mat_0.dims[0] == 3);
	ASSERT(mat_0.dims[1] == 5);
	ASSERT(mat_0.dims[0] == mat_0.d.size());
	ASSERT(mat_0.dims[1] == mat_0.d[0].size());

	Matrix mat_1(
		[](size_t i) { return 1.0f / (1.0f + static_cast<float>(i)); },
		2, 3);
	ASSERT(mat_1.dims[0] == 2);
	ASSERT(mat_1.dims[1] == 3);
	ASSERT(mat_1.dims[0] == mat_1.d.size());
	ASSERT(mat_1.dims[1] == mat_1.d[0].size());
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			ASSERT(F_EQ(mat_1.d[i][j], 1.0f / (1.0f + static_cast<float>(j))));
		}
	}

	return 0;
}

TESTCASE(portprio_linalg_1) {
	Matrix a(2, 3);
	a.d[0][0] = 1.0f; a.d[0][1] = 2.0f; a.d[0][2] = 3.0f;
	a.d[1][0] = 4.0f; a.d[1][1] = 5.0f; a.d[1][2] = 6.0f;

	Matrix aT = Matrix::transpose(a);
	ASSERT(aT.dims[0] == 3);
	ASSERT(aT.dims[1] == 2);
	ASSERT(F_EQ(aT.d[0][0], 1.0f)); ASSERT(F_EQ(aT.d[0][1], 4.0f));
	ASSERT(F_EQ(aT.d[1][0], 2.0f)); ASSERT(F_EQ(aT.d[1][1], 5.0f));
	ASSERT(F_EQ(aT.d[2][0], 3.0f)); ASSERT(F_EQ(aT.d[2][1], 6.0f));

	ASSERT(1==1);
	return 0;
}

TESTCASE(portprio_linalg_2) {
	Matrix a(4, 5);
	Matrix b(5, 2);
	a.d[0][0] = 1.0f; a.d[0][1] = 2.0f; a.d[0][2] = 3.0f; a.d[0][3] = 4.0f; a.d[0][4] = 5.0f;
	a.d[1][0] = 6.0f; a.d[1][1] = 7.0f; a.d[1][2] = 8.0f; a.d[1][3] = 9.0f; a.d[1][4] = 10.0f;
	a.d[2][0] = 11.0f; a.d[2][1] = 12.0f; a.d[2][2] = 13.0f; a.d[2][3] = 14.0f; a.d[2][4] = 15.0f;
	a.d[3][0] = 16.0f; a.d[3][1] = 17.0f; a.d[3][2] = 18.0f; a.d[3][3] = 19.0f; a.d[3][4] = 20.0f;

	b.d[0][0] = 1.0f; b.d[0][1] = 2.0f;
	b.d[1][0] = 3.0f; b.d[1][1] = 4.0f;
	b.d[2][0] = 5.0f; b.d[2][1] = 6.0f;
	b.d[3][0] = 7.0f; b.d[3][1] = 8.0f;
	b.d[4][0] = 9.0f; b.d[4][1] = 10.0f;

	Matrix c = Matrix::mult(a, b);
	std::cout << "c.d[0][0] = " << c.d[0][0] << std::endl;
	ASSERT(c.dims[0] == 4);
	ASSERT(c.dims[1] == 2);
	ASSERT(F_EQ(c.d[0][0], 95.0f));  ASSERT(F_EQ(c.d[0][1], 110.0f));
	ASSERT(F_EQ(c.d[1][0], 220.0f)); ASSERT(F_EQ(c.d[1][1], 260.0f));
	ASSERT(F_EQ(c.d[2][0], 345.0f)); ASSERT(F_EQ(c.d[2][1], 410.0f));
	ASSERT(F_EQ(c.d[3][0], 470.0f)); ASSERT(F_EQ(c.d[3][1], 560.0f));

	return 0;
}
