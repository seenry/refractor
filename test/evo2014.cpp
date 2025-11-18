#include "test.h"

#include "evo2014/chr_0.h"
#include "evo2014/pop_0.h"

#include "portprio/rand.h"

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

auto get_ffn(parlay::random& rng) {
  auto ffn = [&](Chromosome_0& c) {
    auto input = Matrix(
      [&](size_t i) {
        parlay::random local_rng = rng.fork(i);
        return random_uniform(local_rng); 
      }, 1, c.topology[0]
    );
    
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

  return ffn;
}

TESTCASE(evo2014_pop_0_1) {
  std::vector<int> topology = {2, 3, 3, 1};

  Pop_0::pop_size = 3;
  Pop_0 population(777, topology);

  FILE* reference = fopen("test/ref/evo2014_pop_0_1.ref", "r");
  char line[64];
  parlay::random rng(67);
  for (int iter = 0; iter < 5; iter++) {
    parlay::random local_rng = rng.fork(iter);
    population.evaluate_fitness(get_ffn(local_rng));
    for (int i = 0; i < Pop_0::pop_size; i++) {
      float fit = population.fitnesses[i];
      fgets(line, 64, reference);
      float ref_fit = atof(line);
      ASSERT(F_EQ(fit, ref_fit));
    }
  }

  ASSERT(1==1);
  return 0;
}

TESTCASE(evo2014_pop_0_stats) {
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
  for (int i = 0; i < n_bucket; i++) {
    int n_stars = std::floor(68.0f
      * static_cast<float>(buckets[i])
      / static_cast<float>(max_bucket));
    int offset = snprintf(line, 81, "[%2d] ", i);
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

  return 0;
}
