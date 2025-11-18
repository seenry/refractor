#pragma once

#include <vector>

#include "parlay.h"

#include "portprio/linalg.h"

class Chromosome_0 {
public:
	std::vector<Matrix> genome;
	std::vector<int> topology;

  Chromosome_0(std::vector<int>& topology_);
  Chromosome_0(int seed, std::vector<int>& topology_);
};
