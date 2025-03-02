#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <eigen3/Eigen/Dense>
using namespace std;
using namespace Eigen;

MatrixXd generateLatticeBasis(const VectorXd& g, int q, int k) {
    MatrixXd B(g.size(), k);
    for (int j = 0; j < k; ++j) {
        B.col(j) = (g * (j + 1)).unaryExpr([q](double x) { return fmod(x, q); });
    }
    return B;
}

// Babai's Nearest Plane Algorithm con Householder QR
pair<int, VectorXd> babaiNearestPlane(const MatrixXd& B, const VectorXd& b, int q, const VectorXd& g) {
    int k = B.cols();
    
    // Calcola la decomposizione QR di B per ottenere la base ortogonale v*
    HouseholderQR<MatrixXd> qr(B);
    MatrixXd Q = qr.householderQ(); // Matrice ortogonale (base ortonormale)

    VectorXd current = b;
    VectorXi rounded_l(k);  // Contiene i valori arrotondati ⟦l_j⟧

    // Calcola il vettore c_j = v_j / g mod q
    VectorXi c(k);
    for (int j = 0; j < k; ++j) {
        c[j] = static_cast<int>(B.col(j).dot(g) / g.dot(g)) % q;
        if (c[j] < 0) c[j] += q; // Assicura valori non negativi
    }
    cout << c << endl;

    // Passo 2: Scansiona all'indietro da k → 1
    for (int j = k - 1; j >= 0; --j) {
        VectorXd v_star_j = Q.col(j);
        double lj = v_star_j.dot(current) / v_star_j.dot(v_star_j);
        rounded_l[j] = round(lj); // ⟦l_j⟧ è un intero
        
        // Aggiornamento: b_{j-1} = b_j - (l_j - ⟦l_j⟧) * v*_j - ⟦l_j⟧ * v_j
        current -= (lj - rounded_l[j]) * v_star_j + rounded_l[j] * B.col(j);
    }

    // Passo 3: Calcolo di z e dell'errore
    int z = (rounded_l.dot(c)) % q;
    if (z < 0) z += q; // Assicura che z sia non negativo

    VectorXd gz = g * z;
    VectorXd e = b - gz;

    return {z, e};
}

// Esempio di utilizzo
int main() {
    int p = 3329;  // Modulo primo
    int k = 12;

    // Polinomio target da approssimare
    uint16_t s = 2;
    VectorXd g(k);
    g<<1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048;
    VectorXd e(k);
    e<<1, -1, 2, -1, -1, 1, 1, 1, -2, -1, -1, 1;
    Eigen::MatrixXd S_k(k,k);
    S_k << 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 ,
           -1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0,-1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0,-1, 2, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0,-1, 2, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0,-1, 2, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,-1, 2, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,-1, 2, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0,-1, 2, 0, 0, 1,
            0, 0, 0, 0, 0, 0, 0, 0,-1, 2, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 2, 1,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 1;
    //print_matrix(S_k);
    S_k.transposeInPlace(); 
    S_k *= p;
    //MatrixXd S_k = generateLatticeBasis( g, p,  k) ;
    std::cout << "Orthogonalized matrix Q:\n" << S_k << std::endl;
    //print_matrix(S_k);
    Eigen::HouseholderQR<Eigen::MatrixXd> qr(S_k);
    Eigen::MatrixXd Q = qr.householderQ(); // Q contains the orthogonalized vectors

    std::cout << "Orthogonalized matrix Q:\n" << Q << std::endl;
    //print_matrix(S_k);
    VectorXd b = VectorXd::Zero(k);
    
    for(int i=0; i<12; i++){
        b[i] = g[i] * s;
        b[i] = ((static_cast<int>(b[i]) % p) + p) % p;
    }
    //b -= e;
    std::cout << "b:\n" << b << std::endl;
    auto result = babaiNearestPlane(S_k, b, p,g);
    int z= result.first;
    VectorXd e1 = result.second;
    // Trova il polinomio più vicino nel reticolo
    cout << "s:\n " << z << std::endl;
    std::cout << "e;\n" << e1 << std::endl;
    return 0;
}