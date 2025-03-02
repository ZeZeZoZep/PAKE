#include "../include/ntt.h"
#include "../include/int.h"
#include "../include/random.h"
#include "../include/print.h"
using namespace std;
const bool test_sampleNTT = false;
const bool test_ntt = false;
const bool test_mul = false;
vector<uint16_t> sample_trapdoor_matrix(uint16_t rows,uint16_t cols,uint16_t rank){ 
    //cout << rows << cols << rank << endl;
    vector<uint16_t> R(rows * cols * rank);
    for(int i=0; i< rows * cols * rank; i++){
        //cout << "dai dai dai" << endl;
        R[i]=P_random();
        //cout << matrix[i] << endl;
    }
    return R;
}
vector<uint16_t> sample_uniform_q_matrix(uint16_t rows,uint16_t cols,uint16_t rank, int q){ 
    vector<uint16_t> R(rows * cols * rank);
    for(int i=0; i< rows * cols * rank; i++){
        R[i]=uniform_q_random(q);
        //cout << matrix[i] << endl;
    }
    return R;
}

vector<uint16_t> primitive_vector(int k, int q){
    vector<uint16_t> g(k);
    g[0] = 1; // 2^0 ≡ 1 mod q
    for (int i = 1; i < k; i++) {
        g[i] = (g[i - 1] * 2) % q;
    }
    return g;
}
void applyNTTtoMatrix(vector<uint16_t>& matrix) {

    for (int poly_idx = 0; poly_idx < 16; poly_idx++) { // 4x4 = 16 polinomi
        int offset = poly_idx * N; // Offset del polinomio nella matrice lineare

        // Estrai il polinomio corrente
        vector<uint16_t> poly(matrix.begin() + offset, matrix.begin() + offset + N);
        
        // Applica la NTT
        vector<uint16_t> transformed_poly = ntt(poly);
        
        // Scrivi il risultato nella matrice originale
        copy(transformed_poly.begin(), transformed_poly.end(), matrix.begin() + offset);
    }
}
vector<uint16_t> matrix_mul(const vector<uint16_t>& A,  const vector<uint16_t>& B, 
                            const uint16_t rowsA, uint16_t rowsBcolsA, uint16_t colsB,
                             int n, int q){ 
    vector<uint16_t> result(rowsA * colsB * n, 0);
    vector<uint16_t> A_ntt = A;
    applyNTTtoMatrix(A_ntt);
    
    vector<uint16_t> B_ntt = B;
    applyNTTtoMatrix(B_ntt);
    cout << "NTT matrices done " << endl;                           
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            for (int k = 0; k < rowsBcolsA; k++) {
                vector<uint16_t> polyA(A_ntt.begin() + i*k*n, A_ntt.begin() + i*k*n + n);\
                //cout << "TIPOTIPOTIPO " << endl;
                //poly_print(polyA,256);
                vector<uint16_t> polyB(B_ntt.begin() + j*k*n, B_ntt.begin() + j*k*n + n);
                //poly_print(polyB,256);
                vector<uint16_t> temp = multiplyNTTs(polyA,polyB);
                //poly_print(temp,256);
                vector<uint16_t> polyResult(result.begin() + j*i*n, result.begin() + j*i*n + n);
                //poly_print(polyResult,256);
                temp = addNTTs(temp,polyResult);
                //poly_print(temp,256);
                copy(temp.begin(), temp.end(), result.begin() + j*i*n);
            }
        }
    }
    return result;

}
vector<uint16_t> matrix_concat(const vector<uint16_t>& A, const vector<uint16_t>& B, uint16_t rows,uint16_t colsA,uint16_t colsB) {
/*     if (A.size() != MATRIX_SIZE_A || B.size() != MATRIX_SIZE_B) {
        throw invalid_argument("Le dimensioni delle matrici non corrispondono.");
    } */

    vector<uint16_t> C(rows*(colsA+colsB)*N, 0); // Matrice risultante 4x8

    // Concatenazione riga per riga
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < colsA; col++) {
            int src_offset = (row * colsA + col) * N;  // Offset in A
            int dest_offset = (row * (colsA + colsB) + col) * N;  // Offset in C
            copy(A.begin() + src_offset, A.begin() + src_offset + N, C.begin() + dest_offset);
        }
        for (int col = 0; col < colsB; col++) {
            int src_offset = (row * colsB + col) * N;  // Offset in B
            int dest_offset = (row * (colsA + colsB) + colsA + col) * N;  // Offset in C
            copy(B.begin() + src_offset, B.begin() + src_offset + N, C.begin() + dest_offset);
        }
    }

    return C;
}
    // Funzione che inverte i 7 bit di un numero (0 <= x < 128)
    uint16_t Bitrev7(uint16_t x) {
        uint16_t r = 0;
        cout << "pre: "<< x << endl;
        for (int i = 0; i < 7; i++) {
            r = (r << 1) | (x & 1);
            x >>= 1;
        }
        cout << "post: "<< r << endl;
        return r;
    }
    
    // Funzione che pre-calcola i valori zeta usando la formula: ζ[i] = bitrev7(i) mod q
    vector<uint16_t> precalcZetas(uint16_t q) {
        const int numZetas = 128;
        vector<uint16_t> zetas(numZetas);
        for (uint16_t i = 0; i < numZetas; i++) {
            zetas[i] = Bitrev7(i) % q;
        }
        return zetas;
    }
int main() {   
    
    uint16_t q = 3329; // Modulo
    vector<uint16_t> zetas = precalcZetas(q);

    // Stampa i valori pre-calcolati
    for (size_t i = 0; i < zetas.size(); i++) {
        cout << zetas[i] <<", ";
        if(i%16==15)cout << endl;
    }
    cout << endl;
    return 0;
    //trapgen
    uint16_t k = 256; // n in mlkem, grado dei polinomi
    uint8_t d = 4; //dimensione matrice A_bar in mlkem
    int m=2*d;
    int m_bar=m-d; //matrice 3 dimensioni flattened
    //sample random trapdoor
    vector<uint16_t> R = sample_trapdoor_matrix(d, d, k);
/*     cout << "R " << endl;
    cout << R[0] << endl;
    for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            cout << "(" << i << ", " << j <<") : [";

            
            for(int c=0; c<k; c++){
                cout << R[i*j*c + j*c + c] << ", ";

            }
            cout << "]\n";
        }
    } */ 
    //sample random uniform A_bar
    vector<uint16_t> A_bar = sample_uniform_q_matrix(d, d, k, q);

    cout << "A_bar " << endl;

/*     for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            cout << "(" << i << ", " << j <<") : [";

            
            for(int c=0; c<k; c++){
                cout << A_bar[i*j*c + j*c + c] << ", ";

            }
            cout << "]\n";
        }
    } */
    //vector<int16_t> G(d * d * k, 0);
    vector<uint16_t> g_ntt = ntt(primitive_vector(k, q));

    //if(DEBUG) print_matrix(G,n,w);
    cout << "primitive g " << endl;
    //compute A_bar*R
    vector<uint16_t> AR = matrix_mul(A_bar, R, d, d, d, k, q);
    //matrix_print(AR,4,4,256);


    //compute A1 in place
    for(int i=0; i<d; i++){
        vector<uint16_t> aux(AR.begin() + i*i*k, AR.begin() + i*i*k + k);
        aux=subNTTs(g_ntt,aux);
        copy(aux.begin(), aux.end(), AR.begin() + i*i*k);
    }


    //Concatenate A_bar and A1 to form A
    vector<uint16_t> A=matrix_concat(A_bar,AR, d, d, d);
    //if(DEBUG)print_matrix(AR,n, w);
    // Free temporary matrices
    matrix_print(A,d,2*d,k);


    if(test_sampleNTT) {// Esempio di seed di 32 byte (valori casuali)
        vector<uint8_t> seed(32);
        for (size_t i = 0; i < seed.size(); i++) {
            seed[i] = static_cast<uint8_t>(i);  // 0, 1, 2, ..., 31
        }
        
        // Due indici (ad es. per diversificare il campionamento)
        uint8_t idx1 = 0x12;
        uint8_t idx2 = 0x34;
        
        // Parametro q (tipicamente 3329 in alcune implementazioni NTT)
        uint16_t q = 3329;
        
        // Esegui SampleNTT per ottenere 256 coefficienti
        vector<uint16_t> coeffNTT = SampleNTT(seed, idx1, idx2, q);
        
        // Stampa i coefficienti ottenuti
        cout << "Coefficiente della NTT (256 elementi):" << endl;
        for (size_t i = 0; i < coeffNTT.size(); i++) {
            cout << coeffNTT[i] << " ";
        }
        cout << endl;
    }
    if(test_ntt){
        // Esempio: definiamo un polinomio f con 256 coefficienti in ℤ₍q₎.
        vector<uint16_t> f(N);
        for (int i = 0; i < N; i++) {
            f[i] = i % q; // Ad esempio: f[i] = i (mod q)
        }

        // Calcola la NTT (rappresentazione NTT del polinomio)
        vector<uint16_t> F = ntt(f);

        // Calcola l'inversa della NTT per recuperare il polinomio originale
        vector<uint16_t> f_recovered = inv_ntt(F);

        // Stampa i risultati
        cout << "NTT (rappresentazione NTT):" << endl;
        for (int i = 0; i < N; i++) {
            cout << F[i] << " ";
        }
        cout << "\n\n";

        cout << "Inverse NTT (polinomio recuperato):" << endl;
        for (int i = 0; i < N; i++) {
            cout << f_recovered[i] << " ";
        }
        cout << "\n";\
    }
    if (test_mul){
        
        // Definiamo due rappresentazioni NTT di esempio (vettori di 256 coefficienti modulo q).
        vector<uint16_t> f(N) , g(N);

        // Inizializziamo f e g con alcuni valori d'esempio.
        // Per esempio: f[i] = i mod q,  g[i] = (2*i) mod q.
        for (int i = 0; i < N; i++) {
            f[i] = i % q;
            g[i] = (2 * i) % q;
        }
        
        // Calcola il prodotto (in Tq) delle rappresentazioni NTT.
        vector<uint16_t> h = multiplyNTTs(f, g);
        
        // Stampa il risultato.
        cout << "Risultato di MultiplyNTTs:" << endl;
        for (int i = 0; i < N; i++) {
            cout << h[i] << " ";
        }
        cout << endl;

    }
    return 0;

}