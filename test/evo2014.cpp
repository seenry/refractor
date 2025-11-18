#include "test.h"

#include "evo2014/chr_0.h"
#include "evo2014/pop_0.h"

TESTCASE(evo2014_chr_0_0) {
  std::vector<int> topology = {3, 5, 2};
  parlay::random rng(67);
  Chromosome_0 c(rng, topology);
  ASSERT(c.genome.size() == 2);
  ASSERT(c.genome[0].dims[0] == 3);
  ASSERT(c.genome[0].dims[1] == 5);
  ASSERT(c.genome[1].dims[0] == 5);
  ASSERT(c.genome[1].dims[1] == 2);

  FILE* reference = fopen("test/ref/evo2014_chr_0_0.ref", "r");
  char line[32];
  for (int i = 0; i < c.genome.size(); i++) {
    for (int j = 0; j < c.genome[i].dims[0]; j++) {
      for (int k = 0; k < c.genome[i].dims[1]; k++) {
        float val = c.genome[i].d[j][k];
        fgets(line, 32, reference);
        float ref_val = atof(line);
        ASSERT(F_EQ(val, ref_val));
      }
    }
  }
  fclose(reference);

  return 0;
}

TESTCASE(evo2014_pop_0_0) {
  std::vector<int> topology = {5, 4, 3};
  Pop_0::pop_size = 10;
  Pop_0 population(777, topology);
  ASSERT(population.members.size() == 10);
  for (int i = 0; i < Pop_0::pop_size; i++) {
    ASSERT(population.members[i].genome.size() == 2);
    ASSERT(population.members[i].genome[0].dims[0] == 5);
    ASSERT(population.members[i].genome[0].dims[1] == 4);
    ASSERT(population.members[i].genome[1].dims[0] == 4);
    ASSERT(population.members[i].genome[1].dims[1] == 3);
  }

  FILE* reference = fopen("test/ref/evo2014_pop_0_0.ref", "r");
  char line[64];
  for (int i = 0; i < Pop_0::pop_size; i += 3) {
    for (int j = 0; j < population.members[i].genome.size(); j++) {
      for (int k = 0; k < population.members[i].genome[j].dims[0]; k++) {
        for (int l = 0; l < population.members[i].genome[j].dims[1]; l++) {
          float val = population.members[i].genome[j].d[k][l];
          fgets(line, 64, reference);
          float ref_val = atof(line);
          ASSERT(F_EQ(val, ref_val));
        }
      }
    }
  }
  fclose(reference);

  return 0;
}

TESTCASE(evo2014_pop_0_1) {
  // std::vector<int> topology = {2, 3, 3, 1};

  // auto ffn = [](Chromosome_0& c) {
  //   parlay::random rng(67);
  //   parlay::sequence<float> input = Matrix(
  //     [&](size_t i) {
  //       return random_uniform(rng.fork(i), 0.0f, 1.5f);
  //     },
  //     1, c.topology[0]
  //   )
  //   // parlay::tabulate(c.topology[0], [&](size_t i) {
  //   //   return random_uniform(rng.fork(i), 0.0f, 1.5f);
  //   // });
  //   rng = rng.fork(c.topology[0]);

  //   for (int layer = 0; layer + 1 < c.topology.size(); layer++) {
  //     parlay::sequence<float> output = parlay::tabulate(c.topology[layer + 1], [&](size_t j) {
  //       float sum = 0.0f;
  //       for (int i = 0; i < c.topology[layer]; i++) {
  //         sum += input[i] * c.genome[layer].d[i][j];
  //       }
  //       // Activation function: ReLU
  //       return sum > 0.0f ? sum : 0.0f;
  //     });
  //     input = output;
  //   }
  //   // float sum = 0.0f;
  //   // for (int i = 0; i < c.genome.size(); i++) {
  //   //   for (int j = 0; j < c.genome[i].dims[0]; j++) {
  //   //     for (int k = 0; k < c.genome[i].dims[1]; k++) {
  //   //       sum += c.genome[i].d[j][k];
  //   //     }
  //   //   }
  //   // }
  //   // return sum;
  // };


  ASSERT(1 == 1);



  return 0;
}
