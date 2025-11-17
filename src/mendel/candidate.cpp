#include "mendel/engine.h"

#include <cstdlib>
#include "util.h"

Candidate::Candidate() : Candidate(4, 0.0f, 1.0f) {}
Candidate::Candidate(int genome_size, float mean, float variance) {
  float std = sqrtf(variance);
  parlay::random generator(rand());
  
  genome = parlay::sequence<float>(genome_size);
  for (int i = 0; i < genome_size; i++) {
    genome[i] = normal(generator, mean, std);
  }
}
