#pragma once

#include "parlay/random.h"

float uniform(parlay::random& generator);
float normal(parlay::random& generator, float mean, float std);
int search(float* arr, int len, float val);
float sigmoid(float x, float alpha=4.9f);
float relu(float x);
float l_relu(float x, float alpha=0.01f);
