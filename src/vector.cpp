#include "../include/vector.h"

using namespace std
void sum_vector(int *result, int *A, int *B, int len, int q) {
    for(int i=0;i<len;i++){
        result[i]=ensure_positive(A[i]+B[i],q) % q;
    }
}
void print_vector(int *vector, int len){
    for (int j = 0; j < len; j++) {
            printf("%d ", vector[j]);
    }
    printf("\n");
}
/* int compare_vectors(vectorA,int *B, int len){
    for(int i=0; i<len; i++)if(A[i]!=B[i]) return 0;
    return 1;
} */