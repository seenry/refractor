#include "surface.h"

fv3_t fv3_t::operator+(const fv3_t& other) const {
  return fv3_t(
    data[0] + other.data[0],
    data[1] + other.data[1],
    data[2] + other.data[2]
  );
}
fv3_t fv3_t::operator-(const fv3_t& other) const {
  return fv3_t(
    data[0] - other.data[0],
    data[1] - other.data[1],
    data[2] - other.data[2]
  );
}
fv3_t fv3_t::operator*(float scalar) const {
  return fv3_t(
    data[0] * scalar,
    data[1] * scalar,
    data[2] * scalar
  );
}

NurbSurf::NurbSurf() : NurbSurf(16) {}

NurbSurf::NurbSurf(int res)
  : point_res(res)
{
  ctrl.resize(point_res, std::vector<fv3_t>(point_res));
  weight.resize(point_res, std::vector<float>(point_res, 1.0f));
  knot[0].resize(point_res + degree + 1, 0.0f);
  knot[1].resize(point_res + degree + 1, 0.0f);
}

float NurbSurf::blend(int axis, int i, int p, float t) {
  float tol = 1e-8f;
  const std::vector<float>& k = knot[axis];

  if (p == 0) {
    return (
      (k[i] <= t + tol) && (t - tol <= k[i + 1])
    ) ? 1.0f : 0.0f;
  }

  float d1 = k[i + p] - k[i];
  float c1 = (d1 < tol) ? 0.0f : (t - k[i]) / d1;
  float d2 = k[i + p + 1] - k[i + 1];
  float c2 = (d2 < tol) ? 0.0f : (k[i + p + 1] - t) / d2;

  return c1 * blend(axis, i, p - 1, t) + c2 * blend(axis, i + 1, p - 1, t);
}

fv3_t NurbSurf::get_point(float t_x, float t_y) {
  float tol = 1e-8f;
  fv3_t n(0.0f, 0.0f, 0.0f);
  float d = 0.0f;

  for (int i = 0; i < point_res; i++) {
    for (int j = 0; j < point_res; j++) {
      float c1 = blend(0, i, degree, t_x);
      float c2 = blend(1, j, degree, t_y);
      float w = weight[i][j];
      float b = c1 * c2 * w;
      n = n + ctrl[i][j] * b;
      d = d + b;
    }
  }

  return (d <= tol) ? fv3_t() : n * (1.0f / d);
}

fv3_t NurbSurf::get_normal(float t_x, float t_y) {
  float epsilon = 5e-5f;
  fv3_t ur = get_point(t_x + epsilon, t_y);
  fv3_t ul = get_point(t_x - epsilon, t_y);
  fv3_t vr = get_point(t_x, t_y + epsilon);
  fv3_t vl = get_point(t_x, t_y - epsilon);

  fv3_t du = (ur - ul) * (1.0f / (2.0f * epsilon));
  fv3_t dv = (vr - vl) * (1.0f / (2.0f * epsilon));
  // normalize
  float du_len = std::sqrt(du.data[0]*du.data[0] + du.data[1]*du.data[1] + du.data[2]*du.data[2]);
  float dv_len = std::sqrt(dv.data[0]*dv.data[0] + dv.data[1]*dv.data[1] + dv.data[2]*dv.data[2]);
  if (du_len >= 1e-8f) {
    du = du * (1.0f / du_len);
  } else { return fv3_t(0.0f, 0.0f, 0.0f); }
  if (dv_len >= 1e-8f) {
    dv = dv * (1.0f / dv_len);
  } else { return fv3_t(0.0f, 0.0f, 0.0f); }

  fv3_t normal(
    du.data[1] * dv.data[2] - du.data[2] * dv.data[1],
    du.data[2] * dv.data[0] - du.data[0] * dv.data[2],
    du.data[0] * dv.data[1] - du.data[1] * dv.data[0]
  );

  return normal;
}
