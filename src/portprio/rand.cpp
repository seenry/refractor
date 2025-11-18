#include "portprio/rand.h"

float random_uniform(parlay::random& rng) {
	return static_cast<float>(rng.rand()) / static_cast<float>(rng.max());
}

float random_normal(parlay::random& rng, float mean, float stddev) {
	float x, y, s;

	int i = 0;
	parlay::random local_rng;
	do {
		local_rng = rng.fork(i++);
		x = random_uniform(local_rng);
		local_rng = rng.fork(i++);
		y = random_uniform(local_rng);
		x = x * 2.0f - 1.0f;
		y = y * 2.0f - 1.0f;
		s = x*x + y*y;
	} while (s <= 1e-6f || s >= 1.0f);

	s = std::sqrtf((-2.0f * std::logf(s)) / s);
	return mean + x * s * stddev;
}