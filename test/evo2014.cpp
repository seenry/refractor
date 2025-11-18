#include "test.h"

#include "evo2014/chr_0.h"

TESTCASE(evo2014_chr_0_0) {
  std::vector<int> topology = {3, 5, 2};
  Chromosome_0 c(67, topology);
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
