#ifndef MATRIX_H
#define MATRIX_H
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
void multiply_matrices(int **result, int **A, int **B, int n, int m, int p, int q);
void subtract_matrices(int **result, int **A, int **B, int rows, int cols, int q);
void sum_matrices(int **result, int **A, int **B, int rows, int cols, int q);
void concatenate_matrices(int **result, int **A, int **B, int rows, int cols_A, int cols_B);
void print_matrix(int **matrix, int rows, int cols);
void free_matrix(int **matrix, int rows);
int** alloc_matrix(int rows, int cols);
void transpose_matrix(int **matrix, int **transposed,int rows, int cols);
int* multiply_vector_matrix(int *s, int **A, int rows, int cols, int q);
#endif