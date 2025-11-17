#include <cmath>
#include <atomic>

#include "util.h"

std::atomic<size_t> rng_counter = 0;

float uniform(parlay::random& generator) {
  float r =
    static_cast<float>(generator.ith_rand(rng_counter++))
  / static_cast<float>(generator.max());
  return r;
}

float normal(parlay::random& generator, float mean, float std) {
  float x, y, s;

  do {
    x = uniform(generator) * 2.0f - 1.0f;
    y = uniform(generator) * 2.0f - 1.0f;
    s = x*x + y*y;
  } while (s <= 1e-10f || s >= 1.0f);

  s = sqrtf(-2.0f * logf(s) / s);

  float result = mean + x * s * std;
  
  if (std::isnan(result) || std::isinf(result)) {
    std::cout << "Warning: normal() generated invalid number. Returning mean." << std::endl;
    return mean;
  }
  
  return result;
}

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

float sigmoid(float x, float alpha) {
  return 1.0f / (1.0f + expf(-alpha * x));
}

float relu(float x) {
  return (x > 0.0f) ? x : 0.0f;
}

float l_relu(float x, float alpha) {
  return (x > 0.0f) ? x : alpha * x;
}
