#include "portprio/rand.h"

float random_normal(parlay::random& rng, float mean, float stddev) {
	float x, y, s;

	int i = 0;
	do {
		x = static_cast<float>(rng[i++]) / static_cast<float>(rng.max());
		y = static_cast<float>(rng[i++]) / static_cast<float>(rng.max());
		x = x * 2.0f - 1.0f;
		y = y * 2.0f - 1.0f;
		s = x*x + y*y;
	} while (s <= 1e-6f || s >= 1.0f);

	s = std::sqrtf((-2.0f * std::logf(s)) / s);
	return mean + x * s * stddev;
}