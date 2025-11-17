#pragma once
#include "vec.h"

class Ray {
public:
    fv3_t origin;
    fv3_t direction;

    Ray(const fv3_t& o, const fv3_t& d);
};
