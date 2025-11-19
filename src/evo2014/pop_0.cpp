#include "evo2014/pop_0.h"

#include "portprio/rand.h"
#include "portprio/misc.h"

int Pop_0::pop_size = 0;
int Pop_0::elite = 0;

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

void Pop_0::sort() {
  parlay::sequence<int> rank = parlay::tabulate(pop_size, [&](size_t i) {
    return static_cast<int>(i);
  });
  parlay::sort_inplace(rank, [&](int i, int j) {
    return fitnesses[i] > fitnesses[j];
  });
  parlay::sequence<Chromosome_0> sorted_members = parlay::tabulate(pop_size, [&](size_t i) {
    return members[rank[i]];
  });
  std::vector<float> sorted_fitnesses(pop_size);
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    sorted_fitnesses[i] = fitnesses[rank[i]];
  });
  members = std::move(sorted_members);
  fitnesses = std::move(sorted_fitnesses);
}

void Pop_0::crossover_and_mutate() {
  float min = parlay::reduce(fitnesses, parlay::minimum<float>());
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    fitnesses[i] = fitnesses[i] - min;
  });
  float max = parlay::reduce(fitnesses, parlay::maximum<float>());
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    fitnesses[i] = ((fitnesses[i] - max) * 0.95f) + max;
  });

  parlay::scan_inclusive_inplace(fitnesses);

  float total = fitnesses[pop_size - 1];
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    fitnesses[i] /= total;
  });

  parlay::sequence<Chromosome_0> next_gen = parlay::tabulate(pop_size, [&](size_t i) {
    if (i < elite) {
      return members[i];
    }
    float r = random_uniform(base_rng, i);
    int idx = search(fitnesses.data(), pop_size, r);
    return members[idx];
  });
  base_rng = base_rng.fork(pop_size);

  parlay::parallel_for(elite, pop_size, [&](size_t i) {
    members[i].genome = next_gen[i].genome;
    int partner = ((i ^ 0x1) < pop_size) ? (i ^ 0x1) : (i - 1);
    members[i].crossover(next_gen[partner].genome);
    members[i].mutate(0.05f, 0.1f);
  });
  base_rng = base_rng.fork(pop_size);
}

