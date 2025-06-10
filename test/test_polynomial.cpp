#include <gtest/gtest.h>
#include "polynomial.h"
#include "random.h"
// Test per la somma di polinomi
TEST(PolynomialTest, Addition1) {
    Polynomial p1(3);
    Polynomial p2(3);
    p1 << 1, 2, 3;
    p2 << 4, 5, 6;

    Polynomial sum = p1 + p2;
    EXPECT_EQ(sum[0], 5);
    EXPECT_EQ(sum[1], 7);
    EXPECT_EQ(sum[2], 9);
}

TEST(PolynomialTest, Addition2) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;
    Polynomial p2(256);
    for(int i=0; i<256; i++)
        p2[i] = 1;

    Polynomial sum = p1 + p2;
    for(int i=1; i<256; i++)
        EXPECT_EQ(sum[i], 2);

}


// Test per la trasformazione NTT
TEST(PolynomialTest, NTTConversion) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;

    Polynomial p_ntt = p1.toNTT();
    Polynomial p_original = p_ntt.fromNTT();
    for(int i=0; i<256; i++)
        EXPECT_EQ(p_original[i], p1[i]);
}
// Test per la moltiplicazione di polinomi
TEST(PolynomialTest, Multiplication1) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;
    Polynomial p2(256);
    for(int i=0; i<256; i++)
        p2[i] = 0;
    p2[0] = 1;
    Polynomial p1_ntt = p1.toNTT();
    Polynomial p2_ntt = p2.toNTT();
    Polynomial prod = p1_ntt * p2_ntt; // Sostituire con la logica della tua NTT multiplication
    Polynomial p_original = prod.fromNTT();

    for(int i=0; i<256; i++)
        EXPECT_EQ(p_original[i], p1[i]);
}
TEST(PolynomialTest, Multiplication2) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;
    Polynomial p2(256);
    for(int i=0; i<256; i++)
        p2[i] = 0;
    p2[0] = 2;
    std::cout << p2 << std::endl;
    Polynomial p1_ntt = p1.toNTT();
    Polynomial p2_ntt = p2.toNTT();
    std::cout << p2_ntt << std::endl;
    Polynomial prod = p1_ntt * p2_ntt; // Sostituire con la logica della tua NTT multiplication
    Polynomial p_original = prod.fromNTT();
    for(int i=0; i<256; i++)
        EXPECT_EQ(p_original[i], 2*p1[i]);
}
// Test per la moltiplicazione di polinomi
TEST(PolynomialTest, Multiplication3) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;
    Polynomial p2(256);
    for(int i=0; i<256; i++)
        p2[i] = 0;
    p2[0] = 8;
    std::cout << p2 << std::endl;
    Polynomial p1_ntt = p1.toNTT();
    Polynomial p2_ntt = p2.toNTT();
    std::cout << p2_ntt << std::endl;
    Polynomial prod = p1_ntt * p2_ntt; // Sostituire con la logica della tua NTT multiplication
    Polynomial p_original = prod.fromNTT();
    for(int i=0; i<256; i++)
        EXPECT_EQ(p_original[i], 8*p1[i]);
}
/* TEST(PolynomialTest, NTTConversion2) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = -1;
    Polynomial p2(256);
    for(int i=0; i<256; i++)
        p2[i] = 3328;
    Polynomial p1_mod = p1.mod(PARAM_Q);
    std::cout<<p1_mod<<std::endl;

    int count=0;
    for(int i=0; i<256; i++)
        if(p1_mod[i]!=p2[i])count++;
    EXPECT_EQ(count, 0);    

    Polynomial p1_ntt = p1.toNTT();
    Polynomial p2_ntt = p2.toNTT();
    std::cout<<p1_ntt<<std::endl;

    std::cout<<p2_ntt<<std::endl;

    count=0;
    for(int i=0; i<256; i++)
        if(p1_ntt[i]!=p2_ntt[i])count++;
    EXPECT_GT(count, 0);
} 

// Test per la moltiplicazione di polinomi
TEST(PolynomialTest, Multiplication_constant1) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;


    Polynomial prod = p1 * 8; // Sostituire con la logica della tua NTT multiplication
    for(int i=0; i<256; i++)
        EXPECT_EQ(prod[i], 8*p1[i]);
}

// Test per la moltiplicazione di polinomi
TEST(PolynomialTest, Multiplication_constant2) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;


    Polynomial prod = p1 * -1; // Sostituire con la logica della tua NTT multiplication
    for(int i=0; i<256; i++)
        EXPECT_EQ(prod[i], 3328);
} */
/* TEST(PolynomialTest, Multiplication_constant3) {
    Polynomial p1(256);
    for(int i=0; i<256; i++)
        p1[i] = 1;
    Polynomial p2(256);
    p2.setZero();
    p2[1] = 3328;

    Polynomial p1_ntt = p1.toNTT();
    Polynomial p2_ntt = p2.toNTT();

    Polynomial prod = p1_ntt * p2_ntt; // Sostituire con la logica della tua NTT multiplication
    EXPECT_EQ(prod.fromNTT(), p1*-1);
} */
// Esegue tutti i test
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}