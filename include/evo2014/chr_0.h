#pragma once

#include <vector>

#include "parlay.h"

#include "portprio/linalg.h"

class Chromosome_0 {
public:
	std::vector<Matrix> genome;
	std::vector<int> topology;
  parlay::random base_rng;
  int id;

  Chromosome_0() {};
  Chromosome_0(std::vector<int>& topology_);
  Chromosome_0(parlay::random rng, std::vector<int>& topology_);

  void crossover(std::vector<Matrix>& other);
  void mutate(float rate, float scale);
};
