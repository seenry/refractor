#include <cmath>
#include <stdio.h>

#include "vec.h"
#include "defines.h"

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
float fv3_t::dot(const fv3_t& other) const {
  return data[0] * other.data[0] + data[1] * other.data[1] + data[2] * other.data[2];
}
float fv3_t::length() const {
  return std::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
}
fv3_t fv3_t::cross(const fv3_t& other) const {
  return fv3_t(
    data[1] * other.data[2] - data[2] * other.data[1],
    data[2] * other.data[0] - data[0] * other.data[2],
    data[0] * other.data[1] - data[1] * other.data[0]
  );
}
fv3_t fv3_t::normalize() const {
  float len = length();
  return (*this) * (1.0f / len);
}
fv3_t fv3_t::max(const fv3_t& a, const fv3_t& b) {
  return fv3_t(
    (a.data[0] > b.data[0]) ? a.data[0] : b.data[0],
    (a.data[1] > b.data[1]) ? a.data[1] : b.data[1],
    (a.data[2] > b.data[2]) ? a.data[2] : b.data[2]
  );
}
fv3_t fv3_t::min(const fv3_t& a, const fv3_t& b) {
  return fv3_t(
    (a.data[0] < b.data[0]) ? a.data[0] : b.data[0],
    (a.data[1] < b.data[1]) ? a.data[1] : b.data[1],
    (a.data[2] < b.data[2]) ? a.data[2] : b.data[2]
  );
}
