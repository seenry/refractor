#pragma once

#include "parlay.h"

#include "evo2014/chr_0.h"

class Pop_0 {
public:
  parlay::sequence<Chromosome_0> members;
  std::vector<float> fitnesses;
  std::vector<int> topology;
  parlay::random base_rng;

  static int pop_size;
  static int elite;
  static float egality;
  static float mutation_rate;
  static float mutation_scale;

  Pop_0(int seed, std::vector<int>& topology_);
  
  void evaluate_fitness(std::function<float(Chromosome_0& c)> ffn);
  void sort();
  void crossover_and_mutate();
};
