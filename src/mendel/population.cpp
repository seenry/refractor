#include "mendel/engine.h"

#include "util.h"

int Population::pop_size = 0;
int Population::genome_size = 0;

Population::Population() : Population(0.0f, 1.0f) {}
Population::Population(float mean, float variance) {
  candidates = parlay::tabulate(pop_size, [&](size_t i) {
    Candidate c(genome_size, mean, variance);
    c.id = static_cast<int>(i);
    return c;
  });
}

void Population::evaluate(float (*get_fitness)(Candidate&)) {
  fitnesses = parlay::tabulate(candidates.size(), [&](size_t i) {
    return get_fitness(candidates[i]);
  });

  float max_fitness = parlay::reduce(fitnesses, parlay::maximum<float>());
  std::cout << "Max fitness: " << max_fitness << std::endl;
  float min_fitness = parlay::reduce(fitnesses, parlay::minimum<float>());
  std::cout << "Min fitness: " << min_fitness << std::endl;
  float avg_fitness = parlay::reduce(fitnesses) / fitnesses.size();
  std::cout << "Avg fitness: " << avg_fitness << std::endl;

  parlay::parallel_for(0, fitnesses.size(), [&](size_t i) {
    fitnesses[i] = fitnesses[i] - min_fitness;
  });
  float total_fitness = parlay::reduce(fitnesses);
  parlay::parallel_for(0, fitnesses.size(), [&](size_t i) {
    fitnesses[i] = fitnesses[i] / total_fitness;
  });
  parlay::scan_inclusive_inplace(fitnesses);
}

void Population::select() {
  parlay::sequence<Candidate> new_candidates = parlay::tabulate(candidates.size(), [&](size_t i) {
    parlay::random generator(i + 1);
    float r = uniform(generator);
    size_t idx = search(fitnesses.data(), fitnesses.size(), r);
    return candidates[idx];
  });

  parlay::parallel_for(0, candidates.size(), [&](size_t i) {
    candidates[i].genome = parlay::tabulate(new_candidates[i].genome.size(), [&](size_t j) {
      parlay::random generator(i * genome_size + j + 1);
      if (uniform(generator) < 0.5f) {
        return new_candidates[i].genome[j];
      } else {
        int partner = ((i ^ 1) < candidates.size()) ? (i ^ 1) : 0;
        return new_candidates[partner].genome[j];
      }
    });
  });
}

void Population::mutate() {
  parlay::parallel_for(0, candidates.size(), [&](size_t i) {
    parlay::parallel_for(0, candidates[i].genome.size(), [&](size_t j) {
      parlay::random generator(i * candidates[i].genome.size() + j + 1);
      if (uniform(generator) < 0.03f) {
        candidates[i].genome[j] += normal(generator, 0.0f, 0.1f);
      }
    });
  });
}
