#include "evo2014/pop_0.h"

int Pop_0::pop_size = 0;

Pop_0::Pop_0(int seed, std::vector<int>& topology_) {
  topology = topology_;
  base_rng = parlay::random(seed);

  members.resize(pop_size);
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    members[i] = Chromosome_0(base_rng.fork(i), topology);
  });
}
