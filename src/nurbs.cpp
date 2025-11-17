#include "nurbs.h"

#include "vec.h"
#include "defines.h"

NurbSurf::NurbSurf() : NurbSurf(16) {}
NurbSurf::NurbSurf(int res)
  : point_res(std::array<int, 2>{res, res})
{
  ctrl.resize(res, std::vector<fv3_t>(res));
  weight.resize(res, std::vector<float>(res, 1.0f));
  knot[0].resize(res + degree + 1, 0.0f);
  knot[1].resize(res + degree + 1, 0.0f);

  tesselation = Tile(0.0f, 1.0f, 0.0f, 1.0f);
}

float NurbSurf::basis(int axis, int i, int p, float t) const {
  const std::vector<float>& k = knot[axis];

  if (p == 0) {
    return (
      (k[i] <= t + FTOL) && (t - FTOL <= k[i + 1])
    ) ? 1.0f : 0.0f;
  }

  float d1 = k[i + p] - k[i];
  float c1 = (d1 < FTOL) ? 0.0f : (t - k[i]) / d1;
  float d2 = k[i + p + 1] - k[i + 1];
  float c2 = (d2 < FTOL) ? 0.0f : (k[i + p + 1] - t) / d2;

  return c1 * basis(axis, i, p - 1, t) + c2 * basis(axis, i + 1, p - 1, t);
}

float NurbSurf::dbasis(int axis, int i, int p, float t) const {
  if (p == 0) {
    return 0.0f;
  }

  const std::vector<float>& k = knot[axis];

  float d1 = k[i + p] - k[i];
  float c1 = (d1 < FTOL) ? 0.0f : (float) p / d1;
  float d2 = k[i + p + 1] - k[i + 1];
  float c2 = (d2 < FTOL) ? 0.0f : (float) p / d2;

  return c1 * basis(axis, i, p - 1, t) - c2 * basis(axis, i + 1, p - 1, t);
}

int NurbSurf::get_support(int axis, float t) const {
  const std::vector<float>& k = knot[axis];
  int n = point_res[axis];
  if (k[degree] > t + FTOL) { return 0; }
  if (k[n] <= t - FTOL) { return n - 1 - degree; }
  
  int low = degree;
  int high = n;
  int mid = (low + high) / 2;
  while (k[mid] > t + FTOL || t - FTOL >= k[mid+1]) {
    if (k[mid] > t + FTOL) { high = mid; }
    else { low = mid + 1; }
    mid = (low + high) / 2;
  }
  return mid - degree;
}

fv3_t NurbSurf::get_point(float t_x, float t_y) const {
  fv3_t n(0.0f, 0.0f, 0.0f);
  float d = 0.0f;

  std::vector<float> tmp(point_res[1], 0.0f);
  {
    int i_0 = get_support(0, t_x);
    int j_0 = get_support(1, t_y);
    int i = i_0;
    float c1 = basis(0, i, degree, t_x);
    for (int j = j_0; j <= j_0 + degree; j++) {
      float c2 = basis(1, j, degree, t_y);
      tmp[j] = c2;
      float w = weight[i][j];
      float b = c1 * c2 * w;
      n = n + ctrl[i][j] * b;
      d = d + b;
    }
    i += 1;
    for (; i <= i_0 + degree; i++) {
      float c1 = basis(0, i, degree, t_x);
      for (int j = j_0; j <= j_0 + degree; j++) {
        float c2 = tmp[j];
        float w = weight[i][j];
        float b = c1 * c2 * w;
        n = n + ctrl[i][j] * b;
        d = d + b;
      }
    }
  }

  return (d <= FTOL) ? fv3_t() : n * (1.0f / d);
}

fv3_t NurbSurf::get_normal(float t_x, float t_y) const {
  std::vector<float> n_(degree + 1, 0.0f);
  std::vector<float> m_(degree + 1, 0.0f);
  std::vector<float> dndu_(degree + 1, 0.0f);
  std::vector<float> dmdv_(degree + 1, 0.0f);

  int i_0 = get_support(0, t_x);
  for (int i = 0; i < degree + 1; i++) {
    n_[i] = basis(0, i_0 + i, degree, t_x);
    dndu_[i] = dbasis(0, i_0 + i, degree, t_x);
  }
  int j_0 = get_support(1, t_y);
  for (int j = 0; j < degree + 1; j++) {
    m_[j] = basis(1, j_0 + j, degree, t_y);
    dmdv_[j] = dbasis(1, j_0 + j, degree, t_y);
  }

  float D = 0.0f;
  float dDdu = 0.0f;
  float dDdv = 0.0f;
  for (int i = 0; i < degree + 1; i++) {
    for (int j = 0; j < degree + 1; j++) {
      float w = weight[i_0 + i][j_0 + j];
      D += n_[i] * m_[j] * w;
      dDdu += dndu_[i] * m_[j] * w;
      dDdv += n_[i] * dmdv_[j] * w;
    }
  }

  float DD = D * D;
  fv3_t du(0.0f, 0.0f, 0.0f);
  fv3_t dv(0.0f, 0.0f, 0.0f);
  for (int i = 0; i < degree + 1; i++) {
    for (int j = 0; j < degree + 1; j++) {
      float w = weight[i_0 + i][j_0 + j];
      float dRdu = m_[j] * w * (dndu_[i] * D - n_[i] * dDdu) / DD;
      float dRdv = n_[i] * w * (dmdv_[j] * D - m_[j] * dDdv) / DD;
      du = du + ctrl[i_0 + i][j_0 + j] * dRdu;
      dv = dv + ctrl[i_0 + i][j_0 + j] * dRdv;
    }
  }
  
  return du.cross(dv).normalize();
}

void NurbSurf::refine_mesh() {
  float err_tol = 5e-4f;
  int max_depth = 6;

  int n_sample = 32;

  std::vector<Tile*> queue;
  queue.push_back(&tesselation);

  for (int iter = 0; iter < max_depth; iter++) {
    if (queue.size() == 0) {
      printf("Finished refinement at depth %d.\n", iter);
      break;
    }
    std::vector<Tile*> next_queue;
    for (Tile* tile : queue) {
      char do_split = 0;
      for (int i = 0; i < n_sample; i++) {
        float i_ = (float) i / (float) (n_sample - 1);
        for (int j = 0; j < n_sample; j++) {
          float j_ = (float) j / (float) (n_sample - 1);
          float u = tile->domain[0][0] + tile->domain[0][1] * i_;
          float v = tile->domain[1][0] + tile->domain[1][1] * j_;
          fv3_t p_surf = get_point(u, v);
          fv3_t p_tile = tile->get_point(u, v);
          float err = (p_surf - p_tile).length();
          if (err > err_tol) {
            do_split = 1;
            break;
          }
        }
        if (do_split) { break; }
      }

      if (do_split) {
        tile->split();
        for (Tile& child : tile->children) {
          child.add_reference(*this);
          next_queue.push_back(&child);
        }
      }
    }
    queue = next_queue;
  }
  
  tesselation.update_bbox();
}

int NurbSurf::get_intersect(const Ray& ray, fv3_t& intersect) const {
  return tesselation.bbox_intersect(ray, intersect);
}
