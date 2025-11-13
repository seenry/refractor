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

float NurbSurf::basis(int axis, int i, int p, float t) {
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

  return c1 * basis(axis, i, p - 1, t) + c2 * basis(axis, i + 1, p - 1, t);
}

float NurbSurf::dbasis(int axis, int i, int p, float t) {
  if (p == 0) {
    return 0.0f;
  }

  float tol = 1e-8f;
  const std::vector<float>& k = knot[axis];

  float d1 = k[i + p] - k[i];
  float c1 = (d1 < tol) ? 0.0f : (float) p / d1;
  float d2 = k[i + p + 1] - k[i + 1];
  float c2 = (d2 < tol) ? 0.0f : (float) p / d2;

  return c1 * basis(axis, i, p - 1, t) - c2 * basis(axis, i + 1, p - 1, t);
}

fv3_t NurbSurf::get_point(float t_x, float t_y) {
  float tol = 1e-8f;
  fv3_t n(0.0f, 0.0f, 0.0f);
  float d = 0.0f;

  std::vector<float> tmp(point_res, 0.0f);
  {
  int i = 0;
  if (i < point_res) {
    float c1 = basis(0, i, degree, t_x);
    for (int j = 0; j < point_res; j++) {
      float c2 = basis(1, j, degree, t_y);
      tmp[j] = c2;
      float w = weight[i][j];
      float b = c1 * c2 * w;
      n = n + ctrl[i][j] * b;
      d = d + b;
    }
  }
  for (int i = 1; i < point_res; i++) {
    float c1 = basis(0, i, degree, t_x);
    for (int j = 0; j < point_res; j++) {
      float c2 = tmp[j];
      float w = weight[i][j];
      float b = c1 * c2 * w;
      n = n + ctrl[i][j] * b;
      d = d + b;
    }
  }
  }

  return (d <= tol) ? fv3_t() : n * (1.0f / d);
}

fv3_t NurbSurf::get_normal(float t_x, float t_y) {
  std::vector<float> n_(point_res);
  std::vector<float> m_(point_res);
  std::vector<float> dndu_(point_res);
  std::vector<float> dmdv_(point_res);
  for (int i = 0; i < point_res; i++) {
    n_[i] = basis(0, i, degree, t_x);
    m_[i] = basis(1, i, degree, t_y);
    dndu_[i] = dbasis(0, i, degree, t_x);
    dmdv_[i] = dbasis(1, i, degree, t_y);
  }

  float D = 0.0f;
  float dDdu = 0.0f;
  float dDdv = 0.0f;
  for (int i = 0; i < point_res; i++) {
    for (int j = 0; j < point_res; j++) {
      float w = weight[i][j];
      D += n_[i] * m_[j] * w;
      dDdu += dndu_[i] * m_[j] * w;
      dDdv += n_[i] * dmdv_[j] * w;
    }
  }

  float DD = D * D;
  fv3_t du(0.0f, 0.0f, 0.0f);
  fv3_t dv(0.0f, 0.0f, 0.0f);
  for (int i = 0; i < point_res; i++) {
    for (int j = 0; j < point_res; j++) {
      float w = weight[i][j];
      float dRdu = m_[j] * w * (dndu_[i] * D - n_[i] * dDdu) / DD;
      float dRdv = n_[i] * w * (dmdv_[j] * D - m_[j] * dDdv) / DD;
      du = du + ctrl[i][j] * dRdu;
      dv = dv + ctrl[i][j] * dRdv;
    }
  }

  fv3_t normal(
    du.data[1] * dv.data[2] - du.data[2] * dv.data[1],
    du.data[2] * dv.data[0] - du.data[0] * dv.data[2],
    du.data[0] * dv.data[1] - du.data[1] * dv.data[0]
  );
  float len = sqrt(
    normal.data[0] * normal.data[0] +
    normal.data[1] * normal.data[1] +
    normal.data[2] * normal.data[2]
  );
  return (len > 1e-8f)
    ? normal * (1.0f / len)
    : fv3_t(0.0f, 0.0f, -1.0f);
}
