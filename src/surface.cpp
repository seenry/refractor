#include "surface.h"

fv3_t fv3_t::operator+(const fv3_t& other) const {
  return fv3_t(
    data[0] + other.data[0],
    data[1] + other.data[1],
    data[2] + other.data[2]
  );
}
fv3_t fv3_t::operator*(float scalar) const {
  return fv3_t(
    data[0] * scalar,
    data[1] * scalar,
    data[2] * scalar
  );
}

NurbCurve::NurbCurve() : NurbCurve(16) {}

NurbCurve::NurbCurve(int res)
  : point_res(res)
{
  ctrl.resize(point_res);
  weight.resize(point_res, 1.0f);
  knot.resize(point_res + degree + 1, 0.0f);
}

NurbSurf::NurbSurf() : NurbSurf(16) {}

NurbSurf::NurbSurf(int res)
  : point_res(res)
{
  curves = std::make_tuple(NurbCurve(res), NurbCurve(res));
}

float NurbCurve::blend(int i, int p, float t) {
  float tol = 1e-8f;

  if (p == 0) {
    return (
      (knot[i] <= t + tol) && (t - tol < knot[i + 1])
    ) ? 1.0f : 0.0f;
  }

  float d1 = knot[i + p] - knot[i];
  float c1 = (d1 < tol) ? 0.0f : (t - knot[i]) / d1;
  float d2 = knot[i + p + 1] - knot[i + 1];
  float c2 = (d2 < tol) ? 0.0f : (knot[i + p + 1] - t) / d2;

  return c1 * blend(i, p - 1, t) + c2 * blend(i + 1, p - 1, t);
}

fv3_t NurbCurve::get_point(float t) {
  fv3_t n { 0.0f, 0.0f, 0.0f };
  float d = 0.0f;

  for (int i = 0; i < point_res; ++i) {
    float c = blend(i, degree, t) * weight[i];
    d = d + c;
    n = n + (ctrl[i] * c);
  }

  return n * (1.0f / d);
}

fv3_t NurbSurf::get_point(float t_x, float t_y) {
  NurbCurve curve_x = std::get<0>(curves);
  NurbCurve curve_y = std::get<1>(curves);

  fv3_t n = fv3_t { 0.0f, 0.0f, 0.0f };
  float d = 0.0f;

  for (int i = 0; i < point_res; ++i) {
    float c_x = curve_x.blend(i, degree, t_x) * curve_x.weight[i];
    fv3_t p_y = curve_y.get_point(t_y);
    float c_y = curve_y.blend(i, degree, t_y) * curve_y.weight[i];
    float c = c_x * c_y;
    d = d + c;
    n = n + (p_y * c);
  }

  float c = (1.0f / d);
  return n * c;
}
