#pragma once

#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/random.h"

class Candidate {
public:
  parlay::sequence<float> genome;
  int id;

  Candidate();
  Candidate(int genome_size, float mean, float variance);
};

class Population {
public:
  parlay::sequence<Candidate> candidates;
  parlay::sequence<float> fitnesses;

  static int pop_size;
  static int genome_size;

  Population();
  Population(float mean, float variance);

  void evaluate(float (*get_fitness)(Candidate&));
  void select();
  void mutate();

  void sort();
};
