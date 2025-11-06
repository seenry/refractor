#include "test.h"

#include "surface.h"

#define compare_with_file(curve, filename) do { \
  FILE* gold = fopen(filename, "r"); \
  char line[256]; \
  for (int i = 0; i < 100; i++) { \
    float t = (float) i / 99.0f; \
    fv3_t p = curve.get_point(t); \
    fgets(line, 256, gold); \
    float gx, gy, gz; \
    sscanf(line, "%g,%g,%g", &gx, &gy, &gz); \
    snprintf(line, 256, "point %d = (%f, %f, %f), expects (%f, %f, %f)", \
      i, p.data[0], p.data[1], p.data[2], gx, gy, gz); \
    ASSERT_BLAME(fabsf(p.data[0] - gx) < FLOAT_TOL, line); \
    ASSERT_BLAME(fabsf(p.data[1] - gy) < FLOAT_TOL, line); \
    ASSERT_BLAME(fabsf(p.data[2] - gz) < FLOAT_TOL, line); \
  } \
} while (0)

TESTCASE(test_curve_0) {
  NurbCurve curve(5);
  ASSERT(curve.degree == 3);
  ASSERT(curve.point_res == 5);
  ASSERT(curve.ctrl.size() == 5);
  ASSERT(curve.weight.size() == 5);
  ASSERT(curve.knot.size() == 9);
  for (int i = 0; i < curve.point_res; i++) {
    ASSERT(curve.weight[i] == 1.0f);
  }
  curve.ctrl[0] = fv3_t(-4.0f, -4.0f, 0.0f);
  curve.ctrl[1] = fv3_t(-4.0f, 4.0f, 0.0f);
  curve.ctrl[2] = fv3_t(4.0f, -4.0f, 0.0f);
  curve.ctrl[3] = fv3_t(4.0f, 4.0f, 0.0f);
  curve.ctrl[4] = fv3_t(-2.0f, -2.0f, 0.0f);
  curve.knot = {
      0.0f, 0.0f, 0.0f, 0.0f,
      0.75f,
      1.0f, 1.0f, 1.0f, 1.0f
  };

  compare_with_file(curve, "./test/curve_0.csv");
  
  return 0;
}

TESTCASE(test_curve_1) {
  NurbCurve curve(5);
  ASSERT(curve.degree == 3);
  ASSERT(curve.point_res == 5);
  ASSERT(curve.ctrl.size() == 5);
  ASSERT(curve.weight.size() == 5);
  ASSERT(curve.knot.size() == 9);
  curve.ctrl[0] = fv3_t(-4.0f, -4.0f, -4.0f);
  curve.ctrl[1] = fv3_t(4.0f, -4.0f, -4.0f);
  curve.ctrl[2] = fv3_t(-4.0f, 4.0f, 4.0f);
  curve.ctrl[3] = fv3_t(4.0f, 4.0f, -4.0f);
  curve.ctrl[4] = fv3_t(-4.0f, -4.0f, 4.0f);
  curve.knot = {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.5f,
    1.0f, 1.0f, 1.0f, 1.0f
  };
  curve.weight[0] = 0.5f;
  curve.weight[1] = 2.0f;
  curve.weight[2] = 5.0f;
  curve.weight[3] = 0.5f;
  curve.weight[4] = 0.2f;

  compare_with_file(curve, "./test/curve_1.csv");

  return 0;
}
