#pragma once
#include <array>
#include <vector>

class fv3_t {
public:
  fv3_t() : data {0.0f, 0.0f, 0.0f} {}
  fv3_t(float x, float y, float z) : data {x, y, z} {}

  std::array<float, 3> data;

  fv3_t operator+(const fv3_t& other) const;
  fv3_t operator*(float scalar) const;
};

class NurbCurve {
public:
  static const int degree = 3;
  int point_res;
  std::vector<fv3_t> ctrl;
  std::vector<float> weight;
  std::vector<float> knot;

  NurbCurve();
  NurbCurve(int res);

  float blend(int i, int p, float t);
  fv3_t get_point(float t);
};

class NurbSurf {
public:
  static const int degree = 3;
  int point_res;
  std::tuple<NurbCurve, NurbCurve> curves;

  NurbSurf();
  NurbSurf(int res);

  fv3_t get_point(float t_x, float t_y);
};
