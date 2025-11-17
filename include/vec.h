#pragma once
#include <array>

class fv3_t {
public:
  fv3_t() : data {0.0f, 0.0f, 0.0f} {}
  fv3_t(float x, float y, float z) : data {x, y, z} {}

  std::array<float, 3> data;

  fv3_t operator+(const fv3_t& other) const;
  fv3_t operator-(const fv3_t& other) const;
  fv3_t operator*(float scalar) const;
  fv3_t operator/(float scalar) const;
  
  float dot(const fv3_t& other) const;
  float length() const;
  fv3_t cross(const fv3_t& other) const;
  fv3_t normalize() const;
  // fv3_t max(const fv3_t& other) const;
  // fv3_t min(const fv3_t& other) const;

  static fv3_t max(const fv3_t& a, const fv3_t& b);
  static fv3_t min(const fv3_t& a, const fv3_t& b);
};