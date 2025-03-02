#include "..include/matrix.h"
// Matrix multiplication modulo q
void multiply_matrices(int **result, int **A, int **B, int n, int m, int p, int q) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            result[i][j] = 0;
            for (int k = 0; k < m; k++) {
                result[i][j] += ensure_positive(A[i][k] * B[k][j],q) % q;
                result[i][j] = result[i][j] % q;
            }
        }
    }
}
void transpose_matrix(int **matrix, int **transposed,int rows, int cols) {
    // Itera su ogni elemento della matrice
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Scambia le righe con le colonne
            transposed[j][i] = matrix[i][j];
        }
    }
}
// Matrix subtraction modulo q
void subtract_matrices(int **result, int **A, int **B, int rows, int cols, int q) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = (A[i][j] - B[i][j] + q) % q;
        }
    }
}
void sum_matrices(int **result, int **A, int **B, int rows, int cols, int q) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = (A[i][j] + B[i][j] + q) % q;
        }
    }
}
void concatenate_matrices(int **result, int **A, int **B, int rows, int cols_A, int cols_B) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols_A; j++) {
            result[i][j] = A[i][j];
        }
        for (int j = 0; j < cols_B; j++) {
            result[i][cols_A + j] = B[i][j];
        }
    }
}
void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
int** alloc_matrix(int rows, int cols) {
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
    }
    return matrix;
}
void print_matrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        printf("%d)... ", i);
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
int* multiply_vector_matrix(int *s, int **A, int rows, int cols, int q) {
    int* result =(int*) calloc(cols, sizeof(int));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++){
            result[j] += ensure_positive(A[i][j] * s[i],q) % q;
            result[j] = result[j] % q;
        }
    }
    return result;
}

