#include "nurbs.h"
#include <limits>
#include <algorithm>

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

fv3_t Tile::du(float u, float v) const {
  (void) u;
  return (corners[1] - corners[0]) * (1.0f - v)
       + (corners[3] - corners[2]) * v;
}
fv3_t Tile::dv(float u, float v) const {
  (void) v;
  return (corners[2] - corners[0]) * (1.0f - u)
       + (corners[3] - corners[1]) * u;
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

// https://research.nvidia.com/sites/default/files/pubs/2019-03_Cool-Patches%3A-A/Chapter_08.pdf
int Tile::blpatch_intersect(const Ray& ray, fv3_t& intersect) const {
  fv3_t q00 = corners[0];
  fv3_t q10 = corners[1];
  fv3_t q01 = corners[2];
  fv3_t q11 = corners[3];
  fv3_t e10 = q10 - q00;
  fv3_t e11 = q11 - q10;
  fv3_t e00 = q01 - q00;
  fv3_t qn = e10.cross(q01 - q11);
  q00 = q00 - ray.origin;
  q10 = q10 - ray.origin;
  float a = q00.cross(ray.direction).dot(e00);
  float c = qn.dot(ray.direction);
  float b = q10.cross(ray.direction).dot(e11);
  b = b - (a + c);
  
  float det0 = b * b - 4.0f * a * c;
  if (det0 < 0) {
    return 1;
  }
  det0 = sqrtf(det0);

  float u1, u2;
  if (c == 0.0f) {
    if (b == 0.0f) {
      return 1;
    }
    u1 = -a / b;
    u2 = -1.0f;
  } else {
    u1 = (-b - copysignf(det0, b)) / 2.0f;
    u2 = a / u1;
    u1 /= c;
  }
  
  float best_t = F32_MAX;
  float best_u = 0.0f, best_v = 0.0f;
  bool found = false;
  if (u1 >= 0.0f && u1 <= 1.0f) {
    fv3_t pa = q00 * (1.0f - u1) + q10 * u1;
    fv3_t pb = e00 * (1.0f - u1) + e11 * u1;
    fv3_t n = ray.direction.cross(pb);
    float det = n.dot(n);
    n = n.cross(pa);
    float t1 = n.dot(pb);
    float v1 = n.dot(ray.direction);
    
    if (t1 > 0.0f && 0.0f <= v1 && v1 <= det) {
      t1 /= det;
      v1 /= det;
      if (t1 < best_t) {
        best_t = t1;
        best_u = u1;
        best_v = v1;
        found = true;
      }
    }
  }

  if (u2 >= 0.0f && u2 <= 1.0f) {
    fv3_t pa = q00 * (1.0f - u2) + q10 * u2;
    fv3_t pb = e00 * (1.0f - u2) + e11 * u2;
    fv3_t n = ray.direction.cross(pb);
    float det = n.dot(n);
    n = n.cross(pa);
    float t2 = n.dot(pb) / det;
    float v2 = n.dot(ray.direction);
    
    if (0.0f <= v2 && v2 <= det && best_t > t2 && t2 > 0.0f) {
      best_t = t2;
      best_u = u2;
      best_v = v2 / det;
      found = true;
    }
  }
  
  if (found) {
    intersect = get_point(
      domain[0][0] + best_u * domain[0][1],
      domain[1][0] + best_v * domain[1][1]
    );
    return 0;
  }
  
  return 1;
}

int Tile::bbox_intersect(const Ray& ray, fv3_t& intersect) const {
  float t_min = 0.0f;
  float t_max = F32_MAX;
  
  for (int axis = 0; axis < 3; axis++) {
    float inv_d = 1.0f / ray.direction.data[axis];
    float t0 = (bbox[0].data[axis] - ray.origin.data[axis]) * inv_d;
    float t1 = (bbox[1].data[axis] - ray.origin.data[axis]) * inv_d;
    if (inv_d < 0.0f) {
      std::swap(t0, t1);
    }
    
    t_min = t0 > t_min ? t0 : t_min;
    t_max = t1 < t_max ? t1 : t_max;
    if (t_max <= t_min) { return 1; }
  }

  if (children.size() == 0) {
    return blpatch_intersect(ray, intersect);
  }

  float closest_dist = F32_MAX;
  fv3_t closest_intersect;
  int invalid = 1;
  for (const Tile& child : children) {
    fv3_t child_intersect;
    if (child.bbox_intersect(ray, child_intersect) == 0) {
      float dist = (child_intersect - ray.origin).length();
      if (dist < closest_dist) {
        closest_dist = dist;
        closest_intersect = child_intersect;
        invalid = 0;
      }
    }
  }
  
  intersect = closest_intersect;
  return invalid;
}
