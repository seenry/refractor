#include "test.h"

#include "portprio/linalg.h"

TESTCASE(portprio_linalg_0) {
	Matrix mat_0(5, 3);
	ASSERT(mat_0.dims[0] == 3);
	ASSERT(mat_0.dims[1] == 5);
	ASSERT(mat_0.dims[0] == mat_0.d.size());
	ASSERT(mat_0.dims[1] == mat_0.d[0].size());

	Matrix mat_1(
		[](size_t i) { return 1.0f / (1.0f + static_cast<float>(i)); },
		3, 2);
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

