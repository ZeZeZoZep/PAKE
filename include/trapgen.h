#ifndef TRAPGEN_H
#define TRAPGEN_H
#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "sample.h"
#include "matrix.h"
#include "vector.h"
#include "utils.h"

#define PARAM_ALPHA 0.03               //controlla sempre che  apha*q>sqrt(n)


void Invert(int *b, int **A, int **R, int q, int n, int m, int k, int *s, int *e);

void TrapGen(int **A, int **R, int n, int m, int q);

int* oracle(int *s,int **A, int *e, int rows , int cols, int q);

#endif