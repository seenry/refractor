#include "evo2014/pop_0.h"

#include "portprio/rand.h"
#include "portprio/misc.h"

int Pop_0::pop_size = 0;
int Pop_0::elite = 0;
float Pop_0::egality = 0.10f;
float Pop_0::mutation_rate = 0.05f;
float Pop_0::mutation_scale = 1.0f;

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

// #define PRINT_FITNESSES() \
//   for (int i_ = 0; i_ < pop_size; i_++) { \
//     printf("%f ", fitnesses[i_]); \
//     if ((i_ + 1) % 5 == 0) { \
//       printf("\n"); \
//     } \
//   } \
//   printf("\n\n")
void Pop_0::crossover_and_mutate() {
  // printf("\n");
  // PRINT_FITNESSES();

  float min = parlay::reduce(fitnesses, parlay::minimum<float>());
  float max = parlay::reduce(fitnesses, parlay::maximum<float>());
  parlay::parallel_for(0, pop_size, [&](size_t i) {
    fitnesses[i] -= min;
    fitnesses[i] *= 1.0f - Pop_0::egality;
    fitnesses[i] += (max - min) * Pop_0::egality;
  });
  // PRINT_FITNESSES();

  float total = parlay::scan_inclusive_inplace(fitnesses);
  // PRINT_FITNESSES();

  parlay::parallel_for(0, pop_size, [&](size_t i) {
    fitnesses[i] /= total;
  });
  // PRINT_FITNESSES();

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
    members[i].mutate(Pop_0::mutation_rate, Pop_0::mutation_scale);
  });
  base_rng = base_rng.fork(pop_size);
}

