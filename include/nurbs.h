#pragma once
#include <array>
#include <vector>
#include <list>

#include "vec.h"

class NurbSurf;
class Tile;

class Tile {
public:
  std::array<std::array<float, 2>, 2> domain;
  std::list<Tile> children;

  std::array<fv3_t, 4> corners;
  std::array<fv3_t, 2> bbox;

  void add_reference(NurbSurf& surf);
  fv3_t get_point(float u, float v) const;
  void split();
  void update_bbox();

  Tile();
  Tile(float u0, float du, float v0, float dv);
};

class NurbSurf {
public:
  static const int degree = 3;
  
  std::array<int, 2> point_res;
  std::vector<std::vector<fv3_t>> ctrl;
  std::vector<std::vector<float>> weight;
  std::array<std::vector<float>, 2> knot;

  Tile tesselation;

  NurbSurf();
  NurbSurf(int res);

  float basis(int axis, int i, int p, float t) const;
  float dbasis(int axis, int i, int p, float t) const;
  fv3_t get_point(float t_x, float t_y) const;
  fv3_t get_normal(float t_x, float t_y) const;

  void refine_mesh();

private:
  int get_support(int axis, float t) const;
};
