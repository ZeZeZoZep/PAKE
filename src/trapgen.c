#include "trapgen.h"
// Function prototypes

// Main function to generate A and R
void TrapGen(int **A, int **R, int n, int m, int q) {
    int k = log2(q);
    int w=n*k; 
    int m_bar=m-w;
    //sample random trapdoor
    generate_randomR_matrix(R, m_bar, w);
    
    //sample random uniform A_bar
    int **A_bar = alloc_matrix(n, m_bar);
    generate_uniform_matrix(A_bar, n, m_bar, q);

    
    int **G = alloc_matrix(n,w);
    generate_primitive_matrix(G,n,k,q);
    if(DEBUG) print_matrix(G,n,w);

    //compute A_bar*R
    int **AR = alloc_matrix(n,w);
    multiply_matrices(AR, A_bar, R, n, m_bar, w, q);
    if(DEBUG) print_matrix(AR,n, w);

    //compute A1 in place
    subtract_matrices(AR,G,AR,n,w,q);
    if(DEBUG)print_matrix(AR,n, w);


    //Concatenate A_bar and A1 to form A
    concatenate_matrices(A, A_bar,AR, n, m_bar, w);
    if(DEBUG)print_matrix(AR,n, w);
    // Free temporary matrices
    free_matrix(G,n);
    free_matrix(AR,n);
    free_matrix(A_bar,n);
}

int* oracle(int *s,int **A, int *e,int rows,int cols, int q){

    int* ret = multiply_vector_matrix(s,A,rows,cols,q);
    sum_vector(ret,ret,e,cols,q);
    if(DEBUG){
        printf("Vector s: ");
        print_vector(s,rows);
        printf("Vector e: ");
        print_vector(e,cols);   
        printf("Vector sA: ");
        print_vector(ret,cols);
    }
    return ret;
}
int** construct_RI(int **R,int rows,int cols){
    int **RI = alloc_matrix(rows+cols,cols);
    //concat vertically R with I (identity matrix)
    for(int i=0; i<rows+cols;i++){
        for(int j=0; j<cols; j++){
            if(i<rows) RI[i][j]=R[i][j];
            else {
                if ( i-rows==j) RI[i][j]=1;
                else RI[i][j]=0;
            }
        }
    }
    if(DEBUG){
        
        printf("\nMatrix RI:\n");
        print_matrix(RI,rows+cols,cols);
        printf("\n");  
    }
    return RI;
}
int* b_to_b_hat(int **R,int *b,int rows,int cols, int q){
    int **RI = construct_RI(R,rows, cols);
    int *b_hat=(int *)calloc(cols,sizeof(int));
    for (int i=0; i<rows+cols ; i++){
        for (int j=0; j<cols; j++){
            b_hat[j] += ensure_positive(RI[i][j] * b[i],q)% q;
            b_hat[j] = b_hat[j] % q;
        }
    }
    free_matrix(RI,rows+cols);
    return b_hat;
}

int invert_single(int *b,int *e,int k,int q){
    int s=0;
    for(int i=k-1; i>=0; i--){
        unsigned int x=(unsigned int)round(b[i]-pow(2,i)*s) % q;
            // Estrai il bit x_{k-1} (il bit più significativo)
        unsigned int x_k_minus_1 = (x >> (k - 1)) & 1;

        // Estrai il bit x_{k-2}
        unsigned int x_k_minus_2 = (x >> (k - 2)) & 1;

        // Calcola x_{k-1} ⊕ x_{k-2}
        unsigned int result = x_k_minus_1 ^ x_k_minus_2;
        s=s+pow(2,k-1-i)*result;
        int temp=x;
        while(!(temp>=-q/4 && temp<q/4)) if (temp>0) temp-=q/2; else temp += q/2;
        e[i]=temp;
    }
    return s;
    
}


void Invert(int *b,int **A, int **R, int q, int n, int m, int k, int *s, int *e) {
    int *b_hat= b_to_b_hat(R,b,m-n*k, n*k,q);
    int *e_hat= alloc_vector(n*k);

    // Calcola il valore di s per ciascun blocco
    for (int i = 0; i < n; i++) {
        s[i]=invert_single(b_hat+i*k, e_hat+i*k,k,q);
    }
    int *temp = multiply_vector_matrix(s,A,n,m,q);
    for(int i=0; i<m; i++){
        e[i]=b[i]-temp[i];
        while(!(e[i]>=-q/4 && e[i]<q/4)) if (e[i]>0) e[i]-=q/2; else e[i] += q/2;
    }
    if(DEBUG){
        printf("\nVettore b_hat recuperato:\n");
        print_vector(b_hat,n*k);
        printf("\nVettore e_hat recuperato:\n");
        print_vector(e_hat,n*k);
    }
    free(b_hat);
    free(e_hat);
    free(temp);

}


