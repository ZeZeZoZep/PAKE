#include <iostream>
#include <fplll.h>
#include <vector>
#include <cmath>
#include <fplll/svpcvp.h>
using namespace std;
using namespace fplll;
using ZT = Z_NR<mpz_t>;
using FT = FP_NR<double>;

Matrix<ZT> multiplyMatrices(const Matrix<ZT> &A, const Matrix<ZT> &B, const ZT &q) {
    int k = A.get_rows();
    Matrix<ZT> C(k, k);

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            C(i, j) = 0;
            for (int m = 0; m < k; m++) {
                ZT temp;
                temp.mul(A(i, m), B(m, j));  // A[i, m] * B[m, j]
                temp.mod(temp,q);
                C(i, j).add(C(i, j), temp);
                C(i, j).mod(C(i, j), q);  // Riduzione modulo q per sicurezza
            }
        }
    }
    return C;
}

// ✅ Funzione per calcolare v_lattice = w dot V
vector<ZT> computeLatticeVector(const vector<ZT> &w, const Matrix<ZT> &V) {
    int k = V.get_rows();
    vector<ZT> v_lattice(k);

    for (int i = 0; i < k; i++) {
        v_lattice[i] = 0;
        for (int j = 0; j < k; j++) {
            ZT temp;
            temp.mul(w[j], V(j, i));  // w_j * V[j, i]
            v_lattice[i].add(v_lattice[i], temp);
        }
    }
    return v_lattice;
}
bool invert_matrix(Matrix<ZT> &A, Matrix<ZT> &A_inv, ZT q) {
    int n = A.get_rows();
    A_inv = Matrix<ZT>(n, n);

    // ✅ 1. Creiamo una matrice aumentata [A | I]
    Matrix<ZT> augmented(n, 2 * n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented(i, j) = A(i, j); // Parte sinistra: matrice originale
            augmented(i, j + n) = (i == j) ? 1 : 0; // Parte destra: identità
        }
    }

    // ✅ 2. Algoritmo di Gauss-Jordan
    for (int i = 0; i < n; i++) {
        // Trova il pivot (deve essere invertibile modulo q)
        if (augmented(i, i) == 0) {
            cout << "Errore: la matrice non è invertibile (pivot nullo)" << endl;
            return false;
        }

        // Calcola l'inverso modulo q
        ZT inv_pivot;
        if (mpz_invert(inv_pivot.get_data(), augmented(i, i).get_data(), q.get_data()) == 0) {
            cout << "Errore: impossibile calcolare l'inverso di " << augmented(i, i) << " modulo " << q << endl;
            return false;
        }

        // Scala la riga corrente
        for (int j = 0; j < 2 * n; j++) {
            augmented(i, j).mul(augmented(i, j), inv_pivot);
            augmented(i, j).mod(augmented(i, j), q);
        }

        // Elimina gli altri elementi nella colonna
        for (int k = 0; k < n; k++) {
            if (k != i) {
                ZT factor = augmented(k, i);
                for (int j = 0; j < 2 * n; j++) {
                    ZT temp;
                    temp.mul(augmented(i, j), factor);
                    augmented(k, j).sub(augmented(k, j), temp);
                    augmented(k, j).mod(augmented(k, j), q);
                }
            }
        }
    }

    // ✅ 3. Estraiamo l'inversa dalla matrice aumentata
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A_inv(i, j) = augmented(i, j + n);
        }
    }

    return true; // Successo!
}

vector<ZT> babaiUsingFPLLL(Matrix<ZT> &B, vector<ZT> &b) {
    int n = B.get_rows();

    // ✅ 1. Creiamo la base del reticolo con il tipo corretto
/*     Matrix<Z_NR<mpz_t>> Bx; // Usa Z_NR<mpz_t> invece di mpz_t
     Bx(0, 0) = 1;  Bx(0, 1) = 2;  Bx(0, 2) = 3;
    Bx(1, 0) = 4;  Bx(1, 1) = 5;  Bx(1, 2) = 6;
    Bx(2, 0) = 7;  Bx(2, 1) = 8;  Bx(2, 2) = 10; 
 */
    // ✅ 2. Creiamo le matrici per la decomposizione GSO con FP_NR<double>
    Matrix<ZT> U(n, n);      // Matrice di Gram-Schmidt
    Matrix<ZT> Uinv_t(n, n); // Inversa trasposta di U
    // ✅ 3. Creiamo l'oggetto MatGSO con i tipi corretti
    int flags = GSO_DEFAULT;
    MatGSO<ZT, FT> gso(B,U,Uinv_t,GSO_DEFAULT);

    LLLReduction<ZT, FT> lr(gso, LLL_DEF_DELTA,LLL_DEF_ETA,LLL_DEFAULT);
    lr.lll();
    gso.update_gso();

    cout << "coefficents:\n";
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cout <<Uinv_t(i,j).get_data() << " ";
        }
        cout << endl;
    }
    // Creazione dell'oggetto per la riduzione del reticolo

    cout << endl;
    gso.babai(b,0,n,false);
    // Calcola l'errore e = b - closest_vector



    return b;
}
/* double euclideanDistance(vector<ZT> &v1, vector<ZT> &v2) {
    double sum = 0.0;
    for (size_t i = 0; i < v1.size(); i++) {
        ZT diff;
        diff.sub(v1[i], v2[i]);  // Calcola diff = v1[i] - v2[i]
        double diff_d = mpz_get_d(diff.get_data()); // Converte diff in double
        sum += diff_d * diff_d;
    }
    return sqrt(sum);
}

int main() {
    int n = 3; // Dimensione del reticolo

    // ✅ 1. Creiamo la base del reticolo (matrice B)
    Matrix<ZT> B(n, n);
    B(0, 0) = 1;  B(0, 1) = 2;  B(0, 2) = 3;
    B(1, 0) = 4;  B(1, 1) = 5;  B(1, 2) = 6;
    B(2, 0) = 7;  B(2, 1) = 8;  B(2, 2) = 10;

    // ✅ 2. Definiamo il vettore target b (fuori dal reticolo)
    vector<ZT> b(3);
    mpz_set_si(b[0].get_data(), 3);
    mpz_set_si(b[1].get_data(), 5);
    mpz_set_si(b[2].get_data(), 7);

    // ✅ 3. Troviamo il vettore del reticolo più vicino a b
    vector<ZT> closest_vector = babaiUsingFPLLL(B, b);

    // ✅ 4. Verifica se il risultato è nel reticolo (moltiplicazione B * x)
    vector<ZT> lattice_vector(3);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ZT temp;
            temp.mul(B(i, j), closest_vector[j]); // temp = B[i][j] * x[j]
            lattice_vector[i].add(lattice_vector[i], temp); // Somma alla riga
        }
    }

    // ✅ 5. Stampa il vettore più vicino nel reticolo
    cout << "Vettore più vicino nel reticolo:\n";
    for (const auto &val : closest_vector) {
        cout << val << " ";
    }
    cout << endl;

    // ✅ 6. Stampa il vettore calcolato con B*x
    cout << "Vettore nel reticolo (B * x):\n";
    for (const auto &val : lattice_vector) {
        cout << val << " ";
    }
    cout << endl;

    // ✅ 7. Calcola la distanza tra b e il vettore trovato
    double distance = euclideanDistance(b, lattice_vector);
    cout << "Distanza Euclidea tra b e il vettore nel reticolo: " << distance << endl;

    return 0;
} */

int main() {
    int k = 12;
    //int q = 3329;  // Modulo primo
    ZT q;
    mpz_set_si(q.get_data(),3329);
    // Base del reticolo Λq(g)
    Matrix<ZT>  S_k(k, k);
    ZT s;
    mpz_set_si(s.get_data(),1);

    NumVect<ZT> g(12);
    g[0] = 1;  g[1] = 2;  g[2] = 4;  g[3] = 8;
    g[4] = 16; g[5] = 32;  g[6] = 64;  g[7] = 128;
    g[8] = 256; g[9] = 512;  g[10] = 1024; g[11] = 2048;

    cout << "primitive vector:\n";
    for (ZT &v : g) {
        cout << v.get_data() << " ";
    }
    cout << endl;

    NumVect<ZT> e(12); // Vettore di lunghezza 12
    
    // ✅ 3. Inizializzare manualmente i valori
    e[0] = 10;  e[1] = -10;  e[2] = 20;  e[3] = -10;
    e[4] = -10; e[5] = 10;   e[6] = 10;  e[7] = 10;
    e[8] = -20; e[9] = -10;  e[10] = -10; e[11] = 10;
    // Inizializzazione della matrice con i valori specificati
    S_k(0,0)=2;  //S_k(0, 11) = 1;
    S_k(1, 0) = -1; S_k(1, 1) = 2;
    S_k(2, 1) = -1; S_k(2, 2) = 2;
    S_k(3, 2) = -1; S_k(3, 3) = 2;
    S_k(4, 3) = -1; S_k(4, 4) = 2;
    S_k(5, 4) = -1; S_k(5, 5) = 2;
    S_k(6, 5) = -1; S_k(6, 6) = 2;
    S_k(7, 6) = -1; S_k(7, 7) = 2;
    S_k(8, 7) = -1; S_k(8, 8) = 2;  //S_k(8, 11) = 1;
    S_k(9, 8) = -1; S_k(9, 9) = 2;
    S_k(10, 9) = -1; S_k(10, 10) = 2; //S_k(10, 11) = 1;
    S_k(11, 10) = -1; S_k(11, 11) = 2;
    cout << "S_k:\n";
    for(int i=0; i<k; i++){
        for(int j=0; j<k; j++){
            cout <<S_k(i,j).get_data() << " ";
        }
        cout << endl;
    }
    vector<ZT> b(k);
    cout << "g*s:\n";
    for(int i=0; i<12; i++){
        b[i].mul(g[i], s);
        //b[i].mod(b[i],q);
        //cout << b[i].get_d() << " ";
        //b[i].add(b[i], e[i]);
        b[i].mod(b[i],q);
    }
    cout << endl;
    cout << "b = g*s+e:\n";
    for (ZT &v : b) {
        cout << v.get_data() << " ";
    }
    cout << endl;
    // Usa il solver CVP di FPLLL
    cout << "DIOCANE\n";

    Matrix<ZT> V(k, k);

    // Initialize the matrix with the computed values of q S_k^(-T)
    int values[12][12] = {
        {1664, 832, 416, 208, 104, 52, 26, 13, 6, 3, 1, 1},
        {-1, 1664, 832, 416, 208, 104, 52, 26, 12, 6, 2, 2},
        {-2, -1, 1664, 832, 416, 208, 104, 52, 24, 12, 4, 4},
        {-4, -2, -1, 1664, 832, 416, 208, 104, 48, 24, 8, 8},
        {-8, -4, -2, -1, 1664, 832, 416, 208, 96, 48, 16, 16},
        {-16, -8, -4, -2, -1, 1664, 832, 416, 192, 96, 32, 32},
        {-32, -16, -8, -4, -2, -1, 1664, 832, 384, 192, 64, 64},
        {-64, -32, -16, -8, -4, -2, -1, 1664, 768, 384, 128, 128},
        {-128, -64, -32, -16, -8, -4, -2, -1, 1664, 768, 256, 256},
        {-256, -128, -64, -32, -16, -8, -4, -2, -1, 1664, 512, 512},
        {-512, -256, -128, -64, -32, -16, -8, -4, -2, -1, 1664, 1024},
        {-1024, -512, -256, -128, -64, -32, -16, -8, -4, -2, -1, 1664}
    };
    // Fill the Matrix<ZT> with the values
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            mpz_set_si(V(i, j).get_data(), values[i][j]);
        }
    }

    vector<ZT> w = babaiUsingFPLLL(V, b);
    cout << "coefficents:\n";
    for (ZT &v : w) {
        cout << v.get_data() << " ";
    }
    cout << endl;
    vector<ZT> nearest = computeLatticeVector(w, V);
    cout << "closest_vector:\n";
    for (ZT &v : nearest) {
        cout << v.get_data() << " ";
    }
    cout << endl;
    return 0;
}
 