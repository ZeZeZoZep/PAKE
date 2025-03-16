#ifndef COMMON_H
#define COMMON_H


#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#define PARAM_Q 3329
#define PARAM_K 12
#define PARAM_N 256
#define PARAM_D 4
#define PARAM_M PARAM_D*(PARAM_K+2)
#define PARAM_SIGMA 2.00
#define NEEDED_AES_FOR_SALT 3

typedef long double RR_t;
typedef uint32_t scalar;

#endif