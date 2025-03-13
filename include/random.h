#ifndef RANDOM_H
#define RANDOM_H

#include <cmath>
#include <random>
#include <x86intrin.h>

#include "common.h"

uint16_t P_random();
int16_t gaussian_random(double mean, double std_dev);
uint16_t uniform_q_random(int q);

#endif