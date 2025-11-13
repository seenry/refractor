#pragma once
#include <array>
#include <vector>

class fv3_t {
public:
  fv3_t() : data {0.0f, 0.0f, 0.0f} {}
  fv3_t(float x, float y, float z) : data {x, y, z} {}

  std::array<float, 3> data;

  fv3_t operator+(const fv3_t& other) const;
  fv3_t operator-(const fv3_t& other) const;
  fv3_t operator*(float scalar) const;
};

class NurbSurf {
public:
  static const int degree = 3;
  int point_res;

  std::vector<std::vector<fv3_t>> ctrl;
  std::vector<std::vector<float>> weight;
  std::array<std::vector<float>, 2> knot;

  NurbSurf();
  NurbSurf(int res);

  float basis(int axis, int i, int p, float t);
  fv3_t get_point(float t_x, float t_y);
  float dbasis(int axis, int i, int p, float t);
  fv3_t get_normal(float t_x, float t_y);

  // void get_intersect(float x, float y, float* u, float* v);
};
