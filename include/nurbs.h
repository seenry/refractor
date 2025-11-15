#pragma once
#include <array>
#include <vector>

#include "vec.h"

class NurbSurf {
public:
  static const int degree = 3;
  
  std::array<int, 2> point_res;
  std::vector<std::vector<fv3_t>> ctrl;
  std::vector<std::vector<float>> weight;
  std::array<std::vector<float>, 2> knot;

  NurbSurf();
  NurbSurf(int res);

  float basis(int axis, int i, int p, float t) const;
  float dbasis(int axis, int i, int p, float t) const;
  fv3_t get_point(float t_x, float t_y) const;
  fv3_t get_normal(float t_x, float t_y) const;

  void insert_knot(int axis, float t);

  void refine_mesh();

private:
  int get_support(int axis, float t) const;
};
