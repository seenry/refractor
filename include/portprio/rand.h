#pragma once

#include "parlay.h"

float xavier(int n_in, int n_out);
float random_uniform(parlay::random& rng, size_t seed);
float random_normal(parlay::random& rng, float mean, float stddev);
