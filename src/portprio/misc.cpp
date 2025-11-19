#include "portprio/misc.h"

int search(float* arr, int len, float val) {
  int min = 0;
  int max = len - 1;

  while (min <= max) {
    int mid = min + (max - min) / 2;
    if (arr[mid] < val) {
      min = mid + 1;
    } else {
      max = mid - 1;
    }
  }

  return min;
}
