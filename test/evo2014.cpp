#include "test.h"

#include "evo2014/chr_0.h"
#include "evo2014/pop_0.h"

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
  Pop_0::pop_size = 1000;
  std::vector<int> topology = {10, 10};
  std::vector<float> weights(10 * 10 * 1000);
  float mean = 0.0f;
  float var = 0.0f;

  for (int i = 0; i < 100000; i += 2861) {
    Pop_0 population(i, topology);
    for (int j = 0; j < Pop_0::pop_size; j++) {
      for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
          weights[j * 100 + r * 10 + c] =
            population.members[j].genome[0].d[r][c];
        }
      }
    }

    mean = 0.0f;
    for (int j = 0; j < weights.size(); j++) {
      mean += weights[j];
    }
    mean /= static_cast<float>(weights.size());
    ASSERT(std::abs(mean) < 0.1f);

    var = 0.0f;
    for (int j = 0; j < weights.size(); j++) {
      float diff = weights[j] - mean;
      var += diff * diff;
    }
    var /= static_cast<float>(weights.size());
    ASSERT(std::abs(var - 1.0f) < 0.1f);
  }

  int n_bucket = 20;
  std::vector<int> buckets(n_bucket);
  float min = 100.0f;
  float max = -100.0f;
  for (int i = 0; i < weights.size(); i++) {
    if (weights[i] < min) min = weights[i];
    if (weights[i] > max) max = weights[i];
  }
  max -= min;
  for (int i = 0; i < weights.size(); i++) {
    float val = (weights[i] - min) / max;
    int b = std::floor(val * static_cast<float>(n_bucket));
    if (b >= n_bucket) b = n_bucket - 1;
    buckets[b]++;
  }
  int max_bucket = 0;
  for (int i = 0; i < n_bucket; i++) {
    if (buckets[i] > max_bucket) {
      max_bucket = buckets[i];
    }
  }
  max_bucket++;
  
  printf("\n");
  char line[81];
  printf("/\n");
  for (int i = 0; i < n_bucket; i++) {
    int n_stars = std::floor(71.0f
      * static_cast<float>(buckets[i])
      / static_cast<float>(max_bucket));
    int offset = snprintf(line, 81, "| ");
    for (int j = 0; j < n_stars; j++) {
      offset += snprintf(line + offset, 81 - offset, "*");
    }
    offset += snprintf(line + offset, 81 - offset, " (%d)", buckets[i]);
    printf("%s", line);
    if (i < n_bucket - 2) {
      printf("\n");
    } else {
      if (i == n_bucket - 2) {
        char mean_out[32];
        int mean_off = snprintf(mean_out, 32, " mean: %f", mean);
        int remaining = 80 - offset - mean_off;
        for (int j = 0; j < remaining; j++) {
          printf(" ");
        }
        printf("%s\n", mean_out);
      } else {
        char var_out[32];
        int var_off = snprintf(var_out, 32, " variance: %f", var);
        int remaining = 80 - offset - var_off;
        for (int j = 0; j < remaining; j++) {
          printf(" ");
        }
        printf("%s\n", var_out);
      }
    }
  }
  printf("\\\n");

  return 0;
}
