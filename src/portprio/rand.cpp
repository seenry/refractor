#include "portprio/rand.h"

float xavier(int n_in, int n_out) {
	return 2.0f / (static_cast<float>(n_in + n_out));
}

float random_uniform(parlay::random& rng, size_t seed) {
	return static_cast<float>(rng[seed]) / static_cast<float>(rng.max());
}

float random_normal(parlay::random& rng, float mean, float stddev) {
	float x, y, s;

	size_t i = 0;
	do {
		x = random_uniform(rng, i++);
		y = random_uniform(rng, i++);
		x = x * 2.0f - 1.0f;
		y = y * 2.0f - 1.0f;
		s = x*x + y*y;
	} while (s <= 1e-6f || s >= 1.0f);

	s = std::sqrtf((-2.0f * std::logf(s)) / s);
	return mean + x * s * stddev;
}