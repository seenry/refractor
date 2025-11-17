#include "nurbs.h"
#include <limits>

#include "vec.h"
#include "defines.h"

Tile::Tile()
  : Tile(0.0f, 0.0f, 1.0f, 1.0f) {}
Tile::Tile(float u0, float du, float v0, float dv) {
  domain[0][0] = u0;
  domain[0][1] = du;
  domain[1][0] = v0;
  domain[1][1] = dv;
}

#define F32_MAX std::numeric_limits<float>::max()
#define F32_MIN std::numeric_limits<float>::lowest()
void Tile::add_reference(NurbSurf& surf) {
  bbox[0] = fv3_t(F32_MAX, F32_MAX, F32_MAX);
  bbox[1] = fv3_t(F32_MIN, F32_MIN, F32_MIN);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      float u_ = domain[0][0] + (j ? domain[0][1] : 0.0f);
      float v_ = domain[1][0] + (i ? domain[1][1] : 0.0f);
      fv3_t p = surf.get_point(
        u_, v_
      );
      corners[i * 2 + j] = p;
      for (int k = 0; k < 3; k++) {
        if (p.data[k] < bbox[0].data[k]) {
          bbox[0].data[k] = p.data[k];
        }
        if (p.data[k] > bbox[1].data[k]) {
          bbox[1].data[k] = p.data[k];
        }
      }
    }
  }
}

fv3_t Tile::get_point(float u, float v) const {
  if ((u < domain[0][0])
   || (u > domain[0][0] + domain[0][1])
   || (v < domain[1][0])
   || (v > domain[1][0] + domain[1][1])) {
    return fv3_t(0.0f, 0.0f, 0.0f);
  }

  float u_ = (u - domain[0][0]) / domain[0][1];
  float v_ = (v - domain[1][0]) / domain[1][1];
  fv3_t p0 = corners[0] * (1.0f - u_) + corners[1] * u_;
  fv3_t p1 = corners[2] * (1.0f - u_) + corners[3] * u_;
  return p0 * (1.0f - v_) + p1 * v_;
}

void Tile::split() {
  float u0 = domain[0][0];
  float du = domain[0][1] * 0.5f;
  float v0 = domain[1][0];
  float dv = domain[1][1] * 0.5f;
  children.push_back(Tile(u0,    du, v0,    dv));
  children.push_back(Tile(u0,    du, v0+dv, dv));
  children.push_back(Tile(u0+du, du, v0,    dv));
  children.push_back(Tile(u0+du, du, v0+dv, dv));
}

void Tile::update_bbox() {
  if (children.size() == 0) { return; }
  
  for (Tile& child : children) {
    child.update_bbox();
    bbox[0] = fv3_t::min(bbox[0], child.bbox[0]);
    bbox[1] = fv3_t::max(bbox[1], child.bbox[1]);
  }
}
