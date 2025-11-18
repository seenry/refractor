#pragma once

#include "parlay.h"

float random_uniform(parlay::random& rng);
float random_normal(parlay::random& rng, float mean, float stddev);
