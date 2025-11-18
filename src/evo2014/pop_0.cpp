#include "evo2014/pop_0.h"

int Pop_0::pop_size = 0;

Pop_0::Pop_0(int seed, std::vector<int>& topology_) {
  topology = topology_;
  base_rng = parlay::random(seed);

  members.resize(pop_size);
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    members[i] = Chromosome_0(base_rng.fork(i), topology);
  });

  fitnesses.resize(pop_size);

  base_rng = base_rng.fork(pop_size);
}

void Pop_0::evaluate_fitness(std::function<float(Chromosome_0& c)> ffn) {
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    fitnesses[i] = ffn(members[i]);
  });
}
