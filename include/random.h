#ifndef RANDOM_H
#define RANDOM_H

#include <cmath>
#include <random>
#include <x86intrin.h>

#include "common.h"

signed int P_random();
signed int gaussian_random(RR_t mean, RR_t std_dev);
uint16_t uniform_q_random(int q);

#endif