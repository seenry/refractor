#include "ray.h"

Ray::Ray(const fv3_t& o, const fv3_t& d)
  : origin(o), direction(d.normalize()) {}
