#include "test.h"

#include "nurbs.h"

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
      snprintf(line, 256, "point (%d,%d) expects (%.20g, %.20g, %.20g), got (%.20g, %.20g, %.20g)", \
        i, j, gx, gy, gz, p.data[0], p.data[1], p.data[2]); \
      ASSERT_BLAME(std::abs(p.data[0] - gx) < F32_TOL, line); \
      ASSERT_BLAME(std::abs(p.data[1] - gy) < F32_TOL, line); \
      ASSERT_BLAME(std::abs(p.data[2] - gz) < F32_TOL, line); \
    } \
  } \
  fclose(gold); \
} while (0)

TESTCASE(test_surf_init) {
  NurbSurf surf_0;
  ASSERT(surf_0.degree == 3);
  ASSERT(surf_0.point_res[0] == 16);
  ASSERT(surf_0.point_res[1] == 16);
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
  ASSERT(surf_1.point_res[0] == 11);
  ASSERT(surf_1.point_res[1] == 11);
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
  for (int i = 0; i < 21; i++) {
    for (int j = 0; j < 21; j++) {
      float u = (float) i / 20.0f;
      float v = (float) j / 20.0f;
      fv3_t n = surf.get_normal(u, v);
      fgets(line, 256, norm_gold);
      float x_, y_, z_;
      float gx, gy, gz;
      sscanf(line, "%g,%g,%g,%g,%g,%g", &x_, &y_, &z_, &gx, &gy, &gz);
      snprintf(line, 256, "normal (%d,%d) expects (%f, %f, %f), got (%f, %f, %f)", 
        i, j, gx, gy, gz, n.data[0], n.data[1], n.data[2]);
      ASSERT_BLAME(std::abs(n.data[0] - gx) < F32_TOL, line);
      ASSERT_BLAME(std::abs(n.data[1] - gy) < F32_TOL, line);
      ASSERT_BLAME(std::abs(n.data[2] - gz) < F32_TOL, line);
    }
  }
  fclose(norm_gold);

  return 0;
}

#define POST_INSERT_CMP(surf) do { \
  for (int u = 0; u < 100; u++) { \
    for (int v = 0; v < 100; v++) { \
      float fu = (float) u / 99.0f; \
      float fv = (float) v / 99.0f; \
      fv3_t p_test = surf.get_point(fu, fv); \
      fv3_t p_gold = gold.get_point(fu, fv); \
      char msg[256]; \
      snprintf(msg, 256, "point(%.6g, %.6g) expects (%.20g, %.20g, %.20g), got (%.20g, %.20g, %.20g)", \
        fu, fv, \
        p_gold.data[0], p_gold.data[1], p_gold.data[2], \
        p_test.data[0], p_test.data[1], p_test.data[2]); \
      ASSERT_BLAME(std::abs(p_test.data[0] - p_gold.data[0]) < F32_TOL, msg); \
      ASSERT_BLAME(std::abs(p_test.data[1] - p_gold.data[1]) < F32_TOL, msg); \
      ASSERT_BLAME(std::abs(p_test.data[2] - p_gold.data[2]) < F32_TOL, msg); \
    } \
  } \
} while (0)
TESTCASE(test_surf_2) {
  NurbSurf gold(4);
  SETUP_SURF_0(gold);

  int n_insert = 17;

  NurbSurf s0(4);
  SETUP_SURF_0(s0);
  for (int i = 0; i < n_insert; i++) {
    float t = (float) (i + 1) / (float) (n_insert + 1);
    s0.insert_knot(0, t);
  }
  ASSERT(s0.point_res[0] == 4 + n_insert);
  ASSERT(s0.point_res[1] == 4);
  POST_INSERT_CMP(s0);

  NurbSurf s1(4);
  SETUP_SURF_0(s1);
  for (int i = 0; i < n_insert; i++) {
    float t = (float) (i + 1) / (float) (n_insert + 1);
    s1.insert_knot(1, t);
  }
  ASSERT(s1.point_res[0] == 4);
  ASSERT(s1.point_res[1] == 4 + n_insert);
  POST_INSERT_CMP(s1);

  for (int i = 0; i < n_insert; i++) {
    float t = (float) (i + 1) / (float) (n_insert + 1);
    s0.insert_knot(1, t);
  }
  ASSERT(s0.point_res[1] == 4 + n_insert);
  POST_INSERT_CMP(s0);

  for (int i = 0; i < n_insert; i++) {
    float t = (float) (i + 1) / (float) (n_insert + 1);
    s1.insert_knot(0, t);
  }
  ASSERT(s1.point_res[0] == 4 + n_insert);
  POST_INSERT_CMP(s1);

  return 0;
}
