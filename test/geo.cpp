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
  ASSERT(surf_0.tesselation.domain[0][0] == 0.0f);
  ASSERT(surf_0.tesselation.domain[0][1] == 1.0f);
  ASSERT(surf_0.tesselation.domain[1][0] == 0.0f);
  ASSERT(surf_0.tesselation.domain[1][1] == 1.0f);

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
  ASSERT(surf_1.tesselation.domain[0][0] == 0.0f);
  ASSERT(surf_1.tesselation.domain[0][1] == 1.0f);
  ASSERT(surf_1.tesselation.domain[1][0] == 0.0f);
  ASSERT(surf_1.tesselation.domain[1][1] == 1.0f);

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
  (var_name).knot[1][4] = 1.0f; (var_name).knot[1][5] = 1.0f; (var_name).knot[1][6] = 1.0f; (var_name).knot[1][7] = 1.0f; \
  (var_name).tesselation.add_reference(var_name);

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

TESTCASE(test_tesselation_0) {
  NurbSurf surf(4);
  SETUP_SURF_0(surf);

  ASSERT(1 == 1);
  printf("\n");

  Tile &t = surf.tesselation;
  ASSERT(t.children.size() == 0);
  surf.refine_mesh();
  // printf("final bbox: (%.3g, %.3g, %.3g) to (%.3g, %.3g, %.3g)\n",
  //   t.bbox[0].data[0], t.bbox[0].data[1], t.bbox[0].data[2],
  //   t.bbox[1].data[0], t.bbox[1].data[1], t.bbox[1].data[2]);
  ASSERT(std::abs(t.bbox[0].data[2] + 0.0896f) < 1e-4f);
  ASSERT(std::abs(t.bbox[1].data[2] - 0.0689f) < 1e-4f);

  // FILE* tess_file = fopen("test/tessellation.txt", "w");
  std::vector<Tile*> stack;
  stack.push_back(&surf.tesselation);
  int n_tiles = 0;
  while (stack.size() > 0) {
    Tile* current = stack.back();
    stack.pop_back();
    if (current->children.size() == 0) {
      // for (int i = 0; i < 2; i++) {
      //   fprintf(tess_file, "%.6f,%.6f,%.6f\n", 
      //     current->corners[i * 2 + i].data[0],
      //     current->corners[i * 2 + i].data[1],
      //     current->corners[i * 2 + i].data[2]);
      //   fprintf(tess_file, "%.6f,%.6f,%.6f\n", 
      //     current->corners[i * 2 + (1 - i)].data[0],
      //     current->corners[i * 2 + (1 - i)].data[1],
      //     current->corners[i * 2 + (1 - i)].data[2]);
      // }
      // fprintf(tess_file, "%.6f,%.6f,%.6f\n\n",
      //   current->corners[0].data[0],
      //   current->corners[0].data[1],
      //   current->corners[0].data[2]);
      n_tiles++;
    }
    for (Tile& child : current->children) {
      stack.push_back(&child);
    }
  }
  // fclose(tess_file);
  // printf("Total tiles after refinement: %d\n", n_tiles);
  ASSERT(n_tiles == 3553);

  return 0;
}
