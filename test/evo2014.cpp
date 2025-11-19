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

auto ffn = [](Chromosome_0& c) {
  Matrix input(1, 2);
  input.d[0][0] = 0.5f;
  input.d[0][1] = -0.5f;
  for (int layer = 0; layer + 1 < c.topology.size(); layer++) {
    input = Matrix::mult(input, c.genome[layer]);
    for (int i = 0; i < input.dims[1]; i++) {
      if (input.d[0][i] < 0.0f) {
        input.d[0][i] *= 0.01f;
      }
    }
  }
  return input.d[0][0];
};
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

  pop.evaluate_fitness(ffn);
  ASSERT(F_EQ(pop.fitnesses[0], 0.59468013f));

  return 0;
}

TESTCASE(pop_0_2) {
  std::vector<int> topology = {2, 1};
  Pop_0::pop_size = 100;
  Pop_0 pop(777, topology);
  for (int i = 0; i < Pop_0::pop_size; i++) {
    pop.members[i].id = i;
  }
  pop.evaluate_fitness(ffn);
  std::vector<float> fitnesses = pop.fitnesses;
  pop.sort();
  for (int i = 0; i < Pop_0::pop_size; i++) {
    for (int j = 0; j < Pop_0::pop_size; j++) {
      if (fitnesses[j] == pop.fitnesses[i]) {
        ASSERT(pop.members[i].id == j);
        break;
      }
    }
  }
  
  return 0;
}

TESTCASE(pop_0_3) {
  std::vector<int> topology = {2, 2, 1};
  Pop_0::pop_size = 10;
  Pop_0::elite = 2;
  Pop_0 pop(67, topology);
  pop.evaluate_fitness(ffn);
  pop.sort();
  parlay::sequence<Chromosome_0> before = pop.members;
  pop.crossover_and_mutate();

  for (int i = 0; i < Pop_0::elite; i++) {
    for (int layer = 0; layer + 1 < topology.size(); layer++) {
      for (int r = 0; r < topology[layer]; r++) {
        for (int c = 0; c < topology[layer + 1]; c++) {
          ASSERT(F_EQ(pop.members[i].genome[layer].d[r][c],
                      before[i].genome[layer].d[r][c]));
        }
      }
    }
  }

  return 0;
}

TESTCASE(chr_0_cross_stats) {
  int seed = std::rand();

  std::vector<int> topology = {1000, 1000};
  parlay::random rng(seed);
  Chromosome_0 c0(rng.fork(0), topology);
  Chromosome_0 c1(rng.fork(1), topology);
  std::vector<Matrix> before = c0.genome;
  c0.crossover(c1.genome);

  int changed = 0;
  for (int layer = 0; layer + 1 < topology.size(); layer++) {
    for (int r = 0; r < topology[layer]; r++) {
      for (int c = 0; c < topology[layer + 1]; c++) {
        if (!F_EQ(c0.genome[layer].d[r][c], before[layer].d[r][c])) {
          changed++;
        }
      }
    }
  }
  float change_rate = static_cast<float>(changed) /
                      static_cast<float>(topology[0] * topology[1]);
  ASSERT(std::abs(change_rate - 0.5f) < 0.01f);

  return 0;
}

TESTCASE(chr_0_mutate_stats) {
  int seed = std::rand();

  std::vector<int> topology = {2, 5, 3, 1, 10};
  parlay::random rng(seed);
  int n = 10000;
  float m_rate = 0.1f;
  float m_scale = 1.0f;
  std::vector<Chromosome_0> chromosomes(n);
  for (int i = 0; i < n; i++) {
    chromosomes[i] = Chromosome_0(topology);
    chromosomes[i].base_rng = rng.fork(i);

    chromosomes[i].mutate(m_rate, m_scale);
  }
  
  for (int layer = 0; layer + 1 < topology.size(); layer++) {
    int h = topology[layer];
    int w = topology[layer + 1];
    std::vector<float> diffs;
    diffs.reserve(n * w * h * m_rate);
    for (int i = 0; i < n; i++) {
      for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
          float after = chromosomes[i].genome[layer].d[r][c];
          if (!F_EQ(after, 0.0f)) {
            diffs.push_back(after);
          }
        }
      }
    }
    float mean = 0.0f;
    for (float d : diffs) {
      mean += d;
    }
    mean /= static_cast<float>(diffs.size());
    float var = 0.0f;
    for (float d : diffs) {
      float diff = d - mean;
      var += diff * diff;
    }
    var /= static_cast<float>(diffs.size());
    float expected_var = xavier(topology[layer], topology[layer + 1]) * m_scale;
    ASSERT(std::abs(var - expected_var) < 0.05f * expected_var);
    ASSERT(std::abs( (
      static_cast<float>(diffs.size())
    / static_cast<float>(n * w * h)
    ) - m_rate) < 0.05f * m_rate
    );
  }

  return 0;
}

TESTCASE(pop_0_stats) {
  int seed = std::rand();

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
      Pop_0 population(seed ^ i, topology);
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

    var = 0.0f;
    for (int j = 0; j < n_weights; j++) {
      float diff = weights[j] - mean;
      var += diff * diff;
    }
    var /= static_cast<float>(n_weights);
    float x2 = xavier(topology[layer], topology[layer + 1]);
    ASSERT(std::abs(var - x2) < 0.1f);
  }

  return 0;
}
