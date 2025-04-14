#include <gtest/gtest.h>
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include <stdio.h>
#include <openssl/rand.h>
#include "int.h"
#include "ideal_cipher.h"


TEST(PolynomialMatrixTest, NTT1) {
    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }

    PolynomialMatrix<4, 4> A_ntt=A.toNTT();
    Polynomial p_test = A_ntt(0, 0).fromNTT();
    for(int k=0; k<256; k++)
        EXPECT_EQ(A(0, 0)[k], p_test[k]);
}
TEST(PolynomialMatrixTest, NTT2) {
    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }

    PolynomialMatrix<4, 4> A_ntt=A.toNTT();
    Polynomial p_test = A_ntt(0, 0).fromNTT();
    for(int k=0; k<256; k++)
        EXPECT_EQ(1, p_test[k]);
}

TEST(PolynomialMatrixTest, NTT3) {
    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }

    PolynomialMatrix<4, 4> A_ntt=A.toNTT();
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;
    A_ntt(0,0)=p1.toNTT();
    PolynomialMatrix<4, 4> A_original=A_ntt.fromNTT();
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            for(int k=0; k<256; k++)
                EXPECT_EQ(1, A_original(i, j)[k]);
}
TEST(PolynomialMatrixTest, NTT4) {
    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }

    PolynomialMatrix<4, 4> A_ntt=A.toNTT();
    PolynomialMatrix<4, 4> A_original=A_ntt.fromNTT();
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            for(int k=0; k<256; k++)
                EXPECT_EQ(1, A_original(i, j)[k]);
}
TEST(PolynomialMatrixTest, Addition) {
    PolynomialMatrix<4, 4> A, B;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            Polynomial p2(256);
            for(int k=0; k<256; k++){
                p1[k] = 1;
                p2[k] = 1;
            }
            A(i, j)=p1;
            B(i, j)=p2;
        }
        



    PolynomialMatrix<4, 4> C = A + B;

    EXPECT_EQ(C(0, 0)[0], 2);
    EXPECT_EQ(C(0, 1)[1], 2);
    EXPECT_EQ(C(1, 0)[2], 2);
    EXPECT_EQ(C(1, 1)[3], 2);
}

TEST(PolynomialMatrixTest, Multiplication1) {
    PolynomialMatrix<4, 4> A, B;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            Polynomial p2(256);
            for(int k=0; k<256; k++){
                p1[k] = 1;
                p2[k] = 0;
            }
            p2[0] = 2;
            std::cout << p2 << std::endl;
            A(i, j)=p1;
            B(i, j)=p2;
        }
    }
    //std::cout << A << std::endl;
    //std::cout << B << std::endl;



    PolynomialMatrix<4, 4> C = A * B;

    //std::cout << C << std::endl;
    EXPECT_EQ(C(0, 0)[0], 8);
    EXPECT_EQ(C(0, 1)[1], 8);
    EXPECT_EQ(C(1, 0)[2], 8);
    EXPECT_EQ(C(1, 1)[3], 8);
}
// Test per la moltiplicazione di polinomi
TEST(PolynomialMatrixTest, Multiplication_constant1) {
    PolynomialMatrix<4, 4> A;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int k=0; k<256; k++){
                p1[k] = 1;
            }
            A(i, j)=p1;
        }
    }
    Matrix<int, 4, 4> mat(4,4);
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            mat(i, j)=2;

    PolynomialMatrix<4, 4> C = A * mat;

    //std::cout << C << std::endl;
    EXPECT_EQ(C(0, 0)[0], 8);
    EXPECT_EQ(C(0, 1)[1], 8);
    EXPECT_EQ(C(1, 0)[2], 8);
    EXPECT_EQ(C(1, 1)[3], 8);
}

// Test per la moltiplicazione di polinomi
TEST(PolynomialMatrixTest, Multiplication_constant2) {
    PolynomialMatrix<4, 4> A;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int k=0; k<256; k++){
                p1[k] = 1;
            }
            A(i, j)=p1;
        }
    }
    Matrix<int, 4, 4> mat(4,4);
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            mat(i, j)=-1;

    PolynomialMatrix<4, 4> C = A * mat;

    //std::cout << C << std::endl;
    EXPECT_EQ(C(0, 0)[0], 3325);
    EXPECT_EQ(C(0, 1)[1], 3325);
    EXPECT_EQ(C(1, 0)[2], 3325);
    EXPECT_EQ(C(1, 1)[3], 3325);
}

TEST(PolynomialMatrixTest, EncodeDecode1) {
    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }
    
    std::vector<uint16_t> flat = PolynomialMatrixUtils::Encode(A);
    cpp_int val = vector_to_bigint_baseq(flat, PARAM_Q);

    std::vector<uint16_t> flat2 = bigint_to_vector_baseq(val, PARAM_Q, flat.size());
    PolynomialMatrix<4, 4> A2 = PolynomialMatrixUtils::Decode<4, 4>(flat2);


    EXPECT_EQ(A, A2);
}
/* TEST(PolynomialMatrixTest, EncodeDecode2) {
    PolynomialMatrix<1, 4> A;

    for(int i=0; i<1; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }
    
    std::vector<uint16_t> flat = PolynomialMatrixUtils::Encode(A);


    // 2. Codifica
    cpp_int pk_int = vector_to_bigint_baseq(flat, PARAM_Q);
    cpp_int domain_size = boost::multiprecision::pow(cpp_int(PARAM_Q), flat.size());

    // 3. Chiave AES
    vector<uint8_t> aes_key(16);
    RAND_bytes(aes_key.data(), aes_key.size());
    size_t encoded_len_in_bytes = boost::multiprecision::msb(pk_int) / 8 + 1;
    // 4. Cifra e decifra
    auto shuffled = IdealCipher::shake_permute(pk_int, aes_key, encoded_len_in_bytes);
    auto reshuffled = IdealCipher::shake_permute(shuffled, aes_key, encoded_len_in_bytes);


    // 5. Riconversione
    vector<uint16_t> pk2 = bigint_to_vector_baseq(reshuffled, PARAM_Q, flat.size());

    PolynomialMatrix<1, 4> A2 = PolynomialMatrixUtils::Decode<1, 4>(pk2);


    EXPECT_EQ(A, A2);
} */
// Esegue tutti i test
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
