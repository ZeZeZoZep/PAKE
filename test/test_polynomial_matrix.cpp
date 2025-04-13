#include <gtest/gtest.h>
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include <stdio.h>



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

TEST(PolynomialMatrixTest, EncodeDecode) {
    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }
    
    std::vector<uint16_t> flat = PolynomialMatrixUtils::Encode(A);
    cout<<flat.size()<<endl;
    PolynomialMatrix<4, 4> A2 = PolynomialMatrixUtils::Decode<4, 4>(flat);


    EXPECT_EQ(A, A2);
}
// Esegue tutti i test
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
