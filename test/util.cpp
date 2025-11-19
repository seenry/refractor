#include "test.h"

#include "portprio/rand.h"
#include "portprio/misc.h"

TESTCASE(util_0) {
  float arr[] = {0.1f, 0.4f, 0.5f, 0.7f, 0.9f};
  int len = 5;

  ASSERT(search(arr, len, 0.0f) == 0);
  ASSERT(search(arr, len, 0.1f) == 0);
  ASSERT(search(arr, len, 0.3f) == 1);
  ASSERT(search(arr, len, 0.5f) == 2);
  ASSERT(search(arr, len, 0.6f) == 3);
  ASSERT(search(arr, len, 0.9f) == 4);
  ASSERT(search(arr, len, 1.0f) == 5);

  return 0;
}
