#include <gtest/gtest.h>
#include <random>
#include <iostream>
#include <Eigen/Dense>
#include <openssl/rand.h>
#include "random.h"
#include "ntt.h"
#include "polynomial_matrix.h"
#include "common.h"

/* PolynomialMatrix<1, PARAM_D> t;
PolynomialMatrix<1, PARAM_D> s; */
vector<uint8_t> m(32, UINT8_MAX);
vector<uint8_t> rho;
std::vector<uint8_t> seed(32);
const int m_param=PARAM_D+40;

int CountBitDifferences(const std::vector<uint8_t>& v1, const std::vector<uint8_t>& v2) {
     int diff_bits = 0;
    for (size_t i = 0; i < v1.size(); ++i) {
        uint8_t xor_byte = v1[i] ^ v2[i];
        diff_bits += __builtin_popcount(xor_byte); // GCC/Clang built-in
    }
    return diff_bits;
}
TEST(BUG, mmmhh) {
    if (RAND_bytes(seed.data(), seed.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    vector<uint8_t> rho(seed.begin(), seed.begin() + 32);
    vector<uint8_t> sigma(seed.begin() + 32, seed.end());
    PolynomialMatrix<PARAM_D, m_param> A;
    PolynomialMatrix<m_param, PARAM_D> AT;

    uint8_t N = 0;
    for(int i=0; i<PARAM_D; i++){
        for(int j=0; j<m_param; j++){
            A(i,j)=SampleNTT(rho, j, i);
        }
    }
    for (size_t i = 0; i < PARAM_D; ++i) {
        for (size_t j = 0; j < m_param; ++j) {
            AT(j,i) = A(i,j);
        }
    }
    PolynomialMatrix<1, PARAM_D> s;
    for(int i=0; i<PARAM_D; i++){
        s(0,i)=SamplePolyCBD(PRF(PARAM_ETA1, rho, N), PARAM_ETA1);
        N++;
    }
    PolynomialMatrix<1, m_param> e;
    for(int i=0; i<m_param; i++){
        e(0,i)=SamplePolyCBD(PRF(PARAM_ETA1, rho, N), PARAM_ETA1);
        N++;
    }
    PolynomialMatrix<1, m_param> t = s*A + e;


    N = 0;
    PolynomialMatrix<1, m_param> y;
    for(int i=0; i<m_param; i++){
        y(0,i)=SamplePolyCBD(PRF(PARAM_ETA1, rho, N), PARAM_ETA1);
        N++;
    }
    PolynomialMatrix<1,PARAM_D> e1;
    for(int i=0; i<PARAM_D; i++){
        e1(0,i)=SamplePolyCBD(PRF(PARAM_ETA2, rho, N), PARAM_ETA2);
        N++;
    }
    PolynomialMatrix<1, 1> e2;
    e2(0,0) = SamplePolyCBD(PRF(PARAM_ETA2, rho, N), PARAM_ETA2);

    
    PolynomialMatrix<m_param,1> y_transposed;

    for(int i=0; i<m_param; i++)y_transposed(i,0)=y(0,i);   
    PolynomialMatrix<1,1> mu;
    Polynomial poly(256);
    
    vector<uint16_t> decode = ByteDecode(m,1,PARAM_Q);
    vector<uint16_t> temp = Decompress(decode,1,PARAM_Q);

    for(int i=0; i<PARAM_N; i++)poly[i]=temp[i];
    mu(0,0)=poly;
    PolynomialMatrix<1,PARAM_D> u =y*AT + e1;
    PolynomialMatrix<1, 1> v = t*y_transposed + e2 + mu;

    PolynomialMatrix<PARAM_D,1> s_transposed;
    for(int i=0; i<PARAM_D; i++)s_transposed(i,0)=s(0,i);
    PolynomialMatrix<1, 1> w = v - u*s_transposed;

    vector<uint16_t> w_array(256);
    for(int i=0; i<PARAM_N; i++)w_array[i]=w(0,0)[i];



    vector<uint8_t> m2 = ByteEncode(Compress(w_array,1,PARAM_Q),1,PARAM_Q);
    int bit_diff = CountBitDifferences(m, m2);
    EXPECT_EQ(bit_diff, 0);

}
Polynomial PolyMulNaive(const Polynomial& a,
                                  const Polynomial& b,
                                  int32_t n, int32_t q) {
    Polynomial result(n);
    for (int i = 0; i < n; ++i) result[i] = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int idx = (i + j);
            int sign = 1;

            if (idx >= n) {
                // Riduci X^n ≡ -1
                idx -= n;
                sign = -1;
            }

            result[idx] = (result[idx] + sign * a[i] * b[j]) % q;

            if (result[idx] < 0) result[idx] += q;
        }
    }

    return result;
}
TEST(BUG, Multiplication4) {
    if (RAND_bytes(seed.data(), seed.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    vector<uint8_t> rho(seed.begin(), seed.begin() + 32);
    Polynomial p = SampleNTT(rho, 0, 0).fromNTT();    // polinomio in dominio coeff.
    Polynomial p_ntt = p.toNTT();
    Polynomial p_roundtrip = p_ntt.fromNTT();

// Questo DEVE essere identico (mod q) al polinomio di partenza:
    for (int i = 0; i < 256; ++i) {
    EXPECT_EQ(p[i], p_roundtrip[i]);
    }
    Polynomial p1(256);
    Polynomial p2(256);
/*     Polynomial p1_)= SampleNTT(rho, 0, 0);
    Polynomial p2= SampleNTT(rho, 1, 1); */
    for (int i = 0; i < 256; ++i) p1[i] = 1;
    for (int i = 0; i < 256; ++i) p2[i] = 1;
    cout << p1 << endl;
    cout << p2 << endl;
 

    Polynomial prod = p1.toNTT() * p2.toNTT(); // Sostituire con la logica della tua NTT multiplication
    
    
    Polynomial p_original = prod.fromNTT();
    cout << p_original << endl;
    cout << PolyMulNaive(p1, p2, 256, PARAM_Q) << endl;
    for(int i=0; i<1; i++)
        EXPECT_EQ(p_original[i], PolyMulNaive(p1, p2, 256, PARAM_Q)[i]);

       
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
