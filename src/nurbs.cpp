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
      tmp[j] = c2;~
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

// Roughly based on [3].
// [2] used as reference.
#define INDEX(matrix, axis, i, j) \
  (axis == 0 ? (matrix)[i][j] : (matrix)[j][i])
void NurbSurf::insert_knot(int axis, float t) {
  std::vector<float>& k = knot[axis];
  int first = get_support(axis, t);

  point_res[axis]++;
  if (axis == 0) {
    ctrl.insert(ctrl.begin() + first + degree, std::vector<fv3_t>(point_res[1]));
    weight.insert(weight.begin() + first + degree, std::vector<float>(point_res[1]));
  } else {
    for (int i = 0; i < point_res[0]; i++) {
      ctrl[i].insert(ctrl[i].begin() + first + degree, fv3_t(0.0f, 0.0f, 0.0f));
      weight[i].insert(weight[i].begin() + first + degree, 0.0f);
    }
  }
  
  for (int j = 0; j < point_res[1 - axis]; j++) {
    for (int i = first + degree; i >= first + 1; i--) {
      float alpha = (t - k[i]) / (k[i + degree] - k[i]);
      
      fv3_t ctrl_im1 = INDEX(ctrl, axis, i - 1, j);
      float weight_im1 = INDEX(weight, axis, i - 1, j);
      fv3_t ctrl_i = (i == first + degree)
        ? INDEX(ctrl, axis, i + 1, j)
        : INDEX(ctrl, axis, i, j);
      float weight_i = (i == first + degree)
        ? INDEX(weight, axis, i + 1, j)
        : INDEX(weight, axis, i, j);
      
      fv3_t q_i = ctrl_im1 * weight_im1 * (1.0f - alpha)
                + ctrl_i * weight_i * alpha;
      float w_q = weight_im1 * (1.0f - alpha)
                + weight_i * alpha;
      
      INDEX(weight, axis, i, j) = w_q;
      INDEX(ctrl, axis, i, j) = q_i * (1.0f / w_q);
    }
  }

  k.insert(k.begin() + first + degree + 1, t);
}

// [1] section 2.1
void NurbSurf::refine_mesh() {
  std::vector<float>& k_u = knot[0];
  // std::vector<float>& k_v = knot[1];

  int c_flat = 2;

  // 2:
  // k_u/k_v = degree[0]/[1]
  // N/M = point_res[0]/[1]
  // S is defined over the domain [u_{k_u -1}, u_N)x[v_{k_v -1}, v_M)
  // each non-empty subinterval [u_i, u_{i+1})x[v_j, v_{j+1})
  // corresponds to a surface patch.

  // [u_{k_u -1}, u_N)x[v_{k_v -1}, v_M)
  // [u_2, u_{point_res[0]}) x [v_2, v_{point_res[1]})

  // iterate over non-empty subintervals
  // for (size_t i = degree; i < point_res[0]; i++) {
  //   printf("u knot %zu: %f ", i, k_u[i]);
  //   printf("[%f, %f)\n", k_u[i], k_u[i + 1]);
  // }

  std::array<std::vector<int>, 2> n_refinement({
    std::vector<int>(point_res[0] - degree, 0),
    std::vector<int>(point_res[1] - degree, 0)
  });
  for (int row = degree; row < point_res[0]; row++) {

    // u : [row, row+1)
    int n_ref_u = 0;
    float interval_0 = k_u[row + 1] - k_u[row];
    float interval_1 = k_u[row + 2] - k_u[row];
    float interval_2 = k_u[row + 1] - k_u[row - 1];

    if (interval_0 < FTOL || interval_1 < FTOL || interval_2 < FTOL) {
      continue;
    }

    printf("analyzing u knot %d: [%f, %f)\n",
      row,
      k_u[row],
      k_u[row + 1]
    );

    for (int col = degree; col < point_res[1]; col++) {
      // |A_i| * ((t_{i+1} - t_i) ** 1.5)
      // div
      // ( 0.5 * (|V_{i-1}| + |V_i|) ) ** .5

      // A_i = (V_i - V_{i-1}) / (t_{i+1} - t_i)
      // V_i = 2 * (P_i - P_{i-1}) / (t_{i+2} - t_i)

      // Need |A_i| > 0
      //      -> |V_i - V_{i-1}| > 0
      //      t_{i+1} - t_i > 0
      
      fv3_t p_i = ctrl[row][col];
      fv3_t p_im1 = ctrl[row - 1][col];
      fv3_t p_im2 = ctrl[row - 2][col];
      fv3_t v_i = (p_i - p_im1) * (2.0f / interval_1);
      fv3_t v_im1 = (p_im1 - p_im2) * (2.0f / interval_2);
      fv3_t a_i = (v_i - v_im1) * (1.0f / interval_0);

      // printf("(%f, %f, %f)\n(%f, %f, %f)\n(%f, %f, %f)\n\n",
      //   p_im2.data[0], p_im2.data[1], p_im2.data[2],
      //   p_im1.data[0], p_im1.data[1], p_im1.data[2],
      //   p_i.data[0], p_i.data[1], p_i.data[2]
      // );

      printf("(%f, %f, %f)\n(%f, %f, %f)\n\n",
        v_im1.data[0], v_im1.data[1], v_im1.data[2],
        v_i.data[0], v_i.data[1], v_i.data[2]
      );

      int x = std::ceilf(
          c_flat * (a_i.length() * powf(interval_0, 1.5f))
        / powf(0.5f * (v_i.length() + v_im1.length()), 0.5f)
      );

      if (x > n_ref_u) {
        n_ref_u = x;
      }
    }

    n_refinement[0][row - degree] = n_ref_u;
  }

  for (size_t i = 0; i < n_refinement[0].size(); i++) {
    printf("want to add %d knots inside [%f, %f)]\n",
      n_refinement[0][i],
      k_u[i + degree],
      k_u[i + degree + 1]
    );
  }
}
