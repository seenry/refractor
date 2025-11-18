#include "test.h"

#include "evo2014.h"

#include "portprio/rand.h"

TESTCASE(chr_0_0) {
  std::vector<int> topology = {3, 5, 2};
  parlay::random rng(67);
  Chromosome_0 c(rng, topology);
  ASSERT(c.genome.size() == topology.size() - 1);
  ASSERT(c.genome[0].dims[0] == topology[0]);
  ASSERT(c.genome[0].dims[1] == topology[1]);
  ASSERT(c.genome[1].dims[0] == topology[1]);
  ASSERT(c.genome[1].dims[1] == topology[2]);
  ASSERT(c.genome[0].dims[0] == c.genome[0].d.size());
  ASSERT(c.genome[0].dims[1] == c.genome[0].d[0].size());
  ASSERT(c.genome[1].dims[0] == c.genome[1].d.size());
  ASSERT(c.genome[1].dims[1] == c.genome[1].d[0].size());

  return 0;
}

TESTCASE(pop_0_0) {
  std::vector<int> topology = {5, 4, 3};
  Pop_0::pop_size = 10;
  Pop_0 population(777, topology);
  ASSERT(population.members.size() == 10);
  ASSERT(population.fitnesses.size() == 10);

  return 0;
}

auto get_ffn(parlay::random& rng) {
  auto ffn = [&](Chromosome_0& c) {
    auto input = Matrix(
      [&](size_t i) {
        parlay::random local_rng = rng.fork(i);
        return random_uniform(local_rng); 
      }, 1, c.topology[0]
    );
    
    
    return input.d[0][0];
  };

  return ffn;
}

TESTCASE(pop_0_1) {
  std::vector<int> topology = {2, 3, 3, 1};

  Pop_0::pop_size = 1;
  Pop_0 pop(777, topology);
  std::vector<Matrix>& w = pop.members[0].genome;
  w[0].d[0][0] = -0.623901f; w[0].d[0][1] =  1.299682f; w[0].d[0][2] =  1.407398f;
  w[0].d[1][0] =  0.849654f; w[0].d[1][1] = -0.536967f; w[0].d[1][2] = -0.725496f;
  w[1].d[0][0] = -0.961907f; w[1].d[0][1] =  1.827444f; w[1].d[0][2] = -0.378491f;
  w[1].d[1][0] = -1.287295f; w[1].d[1][1] =  0.077923f; w[1].d[1][2] =  1.357746f;
  w[1].d[2][0] = -0.773786f; w[1].d[2][1] =  0.798857f; w[1].d[2][2] =  0.295523f;
  w[2].d[0][0] =  0.302616f;
  w[2].d[1][0] =  0.126007f;
  w[2].d[2][0] =  0.310623f;

  Matrix input(1, 2);
  input.d[0][0] = 0.5f;
  input.d[0][1] = -0.5f;
  
  Chromosome_0& c = pop.members[0];
  for (int layer = 0; layer + 1 < c.topology.size(); layer++) {
    input = Matrix::mult(input, c.genome[layer]);
    for (int i = 0; i < input.dims[1]; i++) {
      if (input.d[0][i] < 0.0f) {
        input.d[0][i] *= 0.01f;
      }
    }
  }

  ASSERT(F_EQ(input.d[0][0], 0.59468013f));

  ASSERT(1==1);
  return 0;
}

TESTCASE(pop_0_stats) {
  Pop_0::pop_size = 1024;
  std::vector<int> topology = {5, 8, 10, 2};
  std::vector<float> weights(8 * 10 * 1024);
  float mean;
  float var;

  for (int layer = 0; layer + 1 < topology.size(); layer++) {
    int layer_size = topology[layer] * topology[layer + 1];
    mean = 0.0f;
    var = 0.0f;
    for (int iter = 0; iter < 10; iter++) {
      int i = (iter ^ 0x1337abcd) * 0xdeadbeef;
      Pop_0 population(i, topology);
      for (int j = 0; j < Pop_0::pop_size; j++) {
        Matrix& m = population.members[j].genome[layer];
        for (int r = 0; r < topology[layer]; r++) {
          for (int c = 0; c < topology[layer + 1]; c++) {
            size_t idx = j * layer_size + r * topology[layer + 1] + c;
            weights[idx] = m.d[r][c];
          }
        }
      }
    }
    
    int n_weights = layer_size * Pop_0::pop_size;
    for (int i = 0; i < n_weights; i++) {
      mean += weights[i];
    }
    mean /= static_cast<float>(n_weights);
    ASSERT(std::abs(mean) < 0.1f);

    float xavier2 = 2.0f / (static_cast<float>(topology[layer]) + static_cast<float>(topology[layer + 1]));
    var = 0.0f;
    for (int j = 0; j < n_weights; j++) {
      float diff = weights[j] - mean;
      var += diff * diff;
    }
    var /= static_cast<float>(n_weights);
    ASSERT(std::abs(var - xavier2) < 0.1f);
  }

  return 0;
}
