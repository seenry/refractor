#include "test.h"

#include "surface.h"

#define compare_with_file(surf, filename) do { \
  FILE* gold = fopen(filename, "r"); \
  char line[256]; \
  for (int i = 0; i < 100; i++) { \
    for (int j = 0; j < 100; j++) { \
      float u = (float) i / 99.0f; \
      float v = (float) j / 99.0f; \
      fv3_t p = surf.get_point(u, v); \
      fgets(line, 256, gold); \
      float gx, gy, gz; \
      sscanf(line, "%g,%g,%g", &gx, &gy, &gz); \
      snprintf(line, 256, "point (%d,%d) expects (%f, %f, %f), got (%f, %f, %f)", \
        i, j, gx, gy, gz, p.data[0], p.data[1], p.data[2]); \
      ASSERT_BLAME(std::abs(p.data[0] - gx) < FLOAT_TOL, line); \
      ASSERT_BLAME(std::abs(p.data[1] - gy) < FLOAT_TOL, line); \
      ASSERT_BLAME(std::abs(p.data[2] - gz) < FLOAT_TOL, line); \
    } \
  } \
  fclose(gold); \
} while (0)

TESTCASE(test_surf_init) {
  NurbSurf surf_0;
  ASSERT(surf_0.degree == 3);
  ASSERT(surf_0.point_res == 16);
  ASSERT(surf_0.ctrl.size() == 16);
  ASSERT(surf_0.weight.size() == 16);
  for (int i = 0; i < 16; i++) {
    ASSERT(surf_0.ctrl[i].size() == 16);
    ASSERT(surf_0.weight[i].size() == 16);
  }
  ASSERT(surf_0.knot[0].size() == 20);
  ASSERT(surf_0.knot[1].size() == 20);

  NurbSurf surf_1(11);
  ASSERT(surf_1.degree == 3);
  ASSERT(surf_1.point_res == 11);
  ASSERT(surf_1.ctrl.size() == 11);
  ASSERT(surf_1.weight.size() == 11);
  for (int i = 0; i < 11; i++) {
    ASSERT(surf_1.ctrl[i].size() == 11);
    ASSERT(surf_1.weight[i].size() == 11);
  }
  ASSERT(surf_1.knot[0].size() == 15);
  ASSERT(surf_1.knot[1].size() == 15);

  return 0;
}

#define SETUP_SURF_0(var_name) \
  (var_name).ctrl[0][0] = fv3_t(0.0f, 0.0f, 0.0f); (var_name).ctrl[0][1] = fv3_t(0.0f, 1.0f, 0.0f); (var_name).ctrl[0][2] = fv3_t(0.0f, 2.0f, 0.0f); (var_name).ctrl[0][3] = fv3_t(0.0f, 3.0f, 0.0f); \
  (var_name).ctrl[1][0] = fv3_t(1.0f, 0.0f, 0.0f); (var_name).ctrl[1][1] = fv3_t(1.0f, 1.0f, 0.2f); (var_name).ctrl[1][2] = fv3_t(1.0f, 2.0f, 0.1f); (var_name).ctrl[1][3] = fv3_t(1.0f, 3.0f, 0.0f); \
  (var_name).ctrl[2][0] = fv3_t(2.0f, 0.0f, 0.0f); (var_name).ctrl[2][1] = fv3_t(2.0f, 1.0f, -0.5f); (var_name).ctrl[2][2] = fv3_t(2.0f, 2.0f, 0.3f); (var_name).ctrl[2][3] = fv3_t(2.0f, 3.0f, 0.0f); \
  (var_name).ctrl[3][0] = fv3_t(3.0f, 0.0f, 0.0f); (var_name).ctrl[3][1] = fv3_t(3.0f, 1.0f, 0.0f); (var_name).ctrl[3][2] = fv3_t(3.0f, 2.0f, 0.0f); (var_name).ctrl[3][3] = fv3_t(3.0f, 3.0f, 0.0f); \
  (var_name).weight[1][1] = 2.0f; (var_name).weight[1][2] = 2.0f; \
  (var_name).weight[2][1] = 2.0f; (var_name).weight[2][2] = 2.0f; \
  (var_name).knot[0][0] = 0.0f; (var_name).knot[0][1] = 0.0f; (var_name).knot[0][2] = 0.0f; (var_name).knot[0][3] = 0.0f; \
  (var_name).knot[0][4] = 1.0f; (var_name).knot[0][5] = 1.0f; (var_name).knot[0][6] = 1.0f; (var_name).knot[0][7] = 1.0f; \
  (var_name).knot[1][0] = 0.0f; (var_name).knot[1][1] = 0.0f; (var_name).knot[1][2] = 0.0f; (var_name).knot[1][3] = 0.0f; \
  (var_name).knot[1][4] = 1.0f; (var_name).knot[1][5] = 1.0f; (var_name).knot[1][6] = 1.0f; (var_name).knot[1][7] = 1.0f;

TESTCASE(test_surf_0) {
  NurbSurf surf(4);
  SETUP_SURF_0(surf);

  compare_with_file(surf, "test/surf_0.txt");

  return 0;
}

TESTCASE(test_surf_1) {
  NurbSurf surf(4);
  SETUP_SURF_0(surf);

  FILE* norm_gold = fopen("test/norm_0.txt", "r");
  char line[256];
  for (int i = 0; i < 17; i++) {
    for (int j = 0; j < 17; j++) {
      float u = (float) i / 16.0f;
      float v = (float) j / 16.0f;
      fv3_t n = surf.get_normal(u, v);
      fgets(line, 256, norm_gold);
      float gx, gy, gz;
      sscanf(line, "%g,%g,%g", &gx, &gy, &gz);
      snprintf(line, 256, "normal (%d,%d) expects (%f, %f, %f), got (%f, %f, %f)", 
        i, j, gx, gy, gz, n.data[0], n.data[1], n.data[2]);
      ASSERT_BLAME(std::abs(n.data[0] - gx) < FLOAT_TOL, line);
      ASSERT_BLAME(std::abs(n.data[1] - gy) < FLOAT_TOL, line);
      ASSERT_BLAME(std::abs(n.data[2] - gz) < FLOAT_TOL, line);
    }
  }
  fclose(norm_gold);

  return 0;
}
