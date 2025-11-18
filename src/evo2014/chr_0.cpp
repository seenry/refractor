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
		float xavier = std::sqrtf(2.0f / (static_cast<float>(topology[layer]) + static_cast<float>(topology[layer + 1])));
		genome[layer].d = parlay::tabulate(topology[layer], [&](size_t rows) {
			return parlay::tabulate(topology[layer + 1], [&](size_t cols) {
				size_t local_offset = offset + rows * topology[layer + 1] + cols;
				parlay::random local_rng = rng.fork(local_offset);
				return random_normal(local_rng, 0.0f, xavier);
			});
		});
		genome[layer].dims = { topology[layer], topology[layer + 1] };
		offset += topology[layer] * topology[layer + 1];
	}
	
	base_rng = rng.fork(offset);
}

