//#include "matrix.h"
#include <random>
#include <iostream>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;



const int q = 3329;
// Funzione per generare un elemento casuale in R_q
int randomElement() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<int> dist(0, q - 1);
    return dist(gen);
}
MatrixXi generateG(int l, int k, int q) {
    // Creiamo il vettore g^T = [1, 2, 4, ..., 2^(k-1)] mod q
    VectorXi g(k);
    for (int i = 0; i < k; i++) {
        g(i) = (1 << i) % q;  // 2^i % q
    }

    // Creiamo la matrice G di dimensione l*k x k
    MatrixXi G = MatrixXi::Zero(k,l * k);

    // Inseriamo g^T in blocchi diagonali
    for (int i = 0; i < l; i++) {
        G.block(i * k, 0, 1, k) = g;
    }

    return G;
}


// Funzione per generare una matrice casuale in R_q di dimensione rows x cols
MatrixXi randomMatrix(int rows, int cols) {
    MatrixXi M(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            M(i, j) = randomElement();
        }
    }
    return M;
}

// Algoritmo per generare il vettore `a` e la trapdoor `R`
pair<MatrixXi,MatrixXi> Trapgen(int l,int k, int n ) {
    // 1. Generiamo la matrice a_0 in modo uniforme in R_q^{l × k}
    MatrixXi a0 = randomMatrix(l+k,n);
    cout << "Matrice a (a0):\n" << a0 << "\n\n";
    // 2. Scegliamo h ∈ R_q (qui lo settiamo a 1 per semplicità)
    VectorXi h(k);
    h[0]=1;

    // 3. Generiamo la matrice R ∈ R_q^{l×k} con distribuzione χ
    MatrixXi R = randomMatrix(l, k);

     // 4. Generiamo g come un vettore riga deterministico (g ∈ R_q^{1×k})
    RowVectorXi g(k);
    for (int i = 0; i < k; i++) {
        g(i) = (1 << i);  // 2^i mod q
    } 
    cout << "Matrice a (G):\n" << h * g  << "\n\n";
    // 5. Calcoliamo il secondo blocco di `a`
    MatrixXi a1 = (h * g - a0.transpose() * R).unaryExpr([](int x) { return (x % q + q) % q; });

    // 6. Costruiamo `a` concatenando `a_0` e `a_1`
    MatrixXi a;
    a.resize(2 * l, k);
    a << a0, a1;
    return {a,R};
}
int main() {
    int l = 4; // Numero di righe di a_0
    int k = 4; // Numero di colonne (uguale per a_0 e a_1)
    int n = 256;
    

    auto ret = Trapgen(l, k,n);
    MatrixXi a = ret.first;
    MatrixXi R = ret.second;
    // Stampiamo i risultati
    cout << "Matrice a (R_q^{2l × k}):\n" << a << "\n\n";
    cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";

    return 0;
}
