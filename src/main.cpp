#include <iostream>
#include <vector>

#include "mendel/engine.h"
#include "util.h"

float xor_test(Candidate& c) {
  float f = 0.0f;

  float test_cases[4][3] = {{0,0,0}, {0,1,1}, {1,0,1}, {1,1,0}};
  for (int t = 0; t < 4; t++) {
    float x = test_cases[t][0];
    float y = test_cases[t][1];
    float target = test_cases[t][2];
  // for (int i = 0; i < 1000; i++) {
  //   float x = rand() / static_cast<float>(RAND_MAX);
  //   float y = rand() / static_cast<float>(RAND_MAX);
  //   float target = ((x > 0.5f) != (y > 0.5f)) ? 1.0f : 0.0f;
    
    float h0[3];
    for (int i = 0; i < 3; i++) {
      h0[i] = sigmoid(
        c.genome[i*3 + 0] * x
      + c.genome[i*3 + 1] * y
      + c.genome[i*3 + 2]);
    }

    float h1[3];
    for (int i = 0; i < 3; i++) {
      h1[i] = sigmoid(
        c.genome[9 + i*4 + 0] * h0[0]
      + c.genome[9 + i*4 + 1] * h0[1]
      + c.genome[9 + i*4 + 2] * h0[2]
      + c.genome[9 + i*4 + 3]);
    }

    float out = sigmoid(
      c.genome[21] * h1[0]
    + c.genome[22] * h1[1]
    + c.genome[23] * h1[2]
    + c.genome[24]
    );

    f += (out - target) * (out - target);
  }

  return 1.0f / (f + 0.001f);
}

float sphere_test(Candidate& c) {
  float sum = 0.0f;
  for (size_t i = 0; i < c.genome.size(); i++) {
    sum += c.genome[i] * c.genome[i];
  }
  return 1.0f / (sum + 1.0f);
}

int main() {
  srand(67);
  
  Population::pop_size = 20;
  Population::genome_size = 25;

  Population pop(0.0f, 1.0f);
  for (int gen = 0; gen < 1000; gen++) {
    std::cout << "Generation " << gen << std::endl;
    pop.evaluate(xor_test);
    pop.select();
    pop.mutate();
  }

  return 0;
}
