#include "evo2014/chr_0.h"

#include "portprio/rand.h"

Chromosome_0::Chromosome_0(std::vector<int>& topology_) {
	topology = topology_;
	genome.resize(topology.size() - 1);
	for (int layer = 0; layer + 1 < topology.size(); layer++) {
		genome[layer].d = PARLAY_M0(topology[layer],
			                           topology[layer + 1]);
		genome[layer].dims = { topology[layer], topology[layer + 1] };
	}
}

Chromosome_0::Chromosome_0(parlay::random rng, std::vector<int>& topology_) {
  topology = topology_;

	genome.resize(topology.size() - 1);

	int offset = 0;
	for (int layer = 0; layer + 1 < topology.size(); layer++) {
		float sigma = std::sqrtf(xavier(topology[layer], topology[layer + 1]));
		genome[layer].d = parlay::tabulate(topology[layer], [&](size_t rows) {
			return parlay::tabulate(topology[layer + 1], [&](size_t cols) {
				size_t local_offset = offset + rows * topology[layer + 1] + cols;
				parlay::random local_rng = rng.fork(local_offset);
				return random_normal(local_rng, 0.0f, sigma);
			});
		});
		genome[layer].dims = { topology[layer], topology[layer + 1] };
		offset += topology[layer] * topology[layer + 1];
	}
	
	base_rng = rng.fork(offset);
}

void Chromosome_0::crossover(std::vector<Matrix>& other) {
	int offset = 0;
	for (int layer = 0; layer + 1 < topology.size(); layer++) {
		for (int r = 0; r < topology[layer]; r++) {
			for (int c = 0; c < topology[layer + 1]; c++) {
				float cross = random_uniform(base_rng, offset++);
				if (cross < 0.5f) {
					genome[layer].d[r][c] = other[layer].d[r][c];
				}
			}
		}
	}
	base_rng = base_rng.fork(offset);
}

void Chromosome_0::mutate(float rate, float scale) {
	int offset = 0;
	for (int layer = 0; layer + 1 < topology.size(); layer++) {
		float sigma = std::sqrtf(xavier(topology[layer], topology[layer + 1]) * scale);
		for (int r = 0; r < topology[layer]; r++) {
			for (int c = 0; c < topology[layer + 1]; c++) {
				float mutate = random_uniform(base_rng, offset++);
				if (mutate < rate) {
					parlay::random mut_rng = base_rng.fork(offset++);
					genome[layer].d[r][c] += random_normal(mut_rng, 0.0f, sigma);
				}
			}
		}
	}
	base_rng = base_rng.fork(offset);
}
