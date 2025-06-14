#include <gtest/gtest.h>
#include "2feistel.h"  // include la tua classe e le dipendenze
#include "polynomial_matrix_utils.h"
#include "../include/kem.h"
#include "../include/random.h"
#include <openssl/rand.h>

using namespace std;

vector<uint8_t> randomKey(32);
PolynomialMatrix<1, PARAM_D> pk;
PolynomialMatrix<1, PARAM_D> sk;
KEM kem;


vector<uint8_t> z;
vector<uint8_t> rho;

TEST(FeistelTest, EncryptDecryptRoundTrip) {
    auto ret_keygen = kem.KeyGen();
    auto ret = ret_keygen.first;
    z = ret_keygen.second;
    
    pk=get<0>(ret);
    sk=get<1>(ret);
    rho=get<2>(ret);




    if (RAND_bytes(randomKey.data(), randomKey.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    Feistel feistel(randomKey);

    vector<uint8_t> r(96);  // esempio: dimensione di r
    if (RAND_bytes(r.data(), r.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    PolynomialMatrix<1, PARAM_D> M =pk;

    auto [s, T] = feistel.encrypt<PARAM_D>(r, M);
    auto [r_recovered, M_recovered] = feistel.decrypt<PARAM_D>(s, T);

    EXPECT_EQ(r, r_recovered);
    EXPECT_EQ(M, M_recovered);
}
TEST(FeistelTest, DifferentRProducesDifferentT) {
    Feistel feistel(randomKey);

    vector<uint8_t> r1(96);  // esempio: dimensione di r
    if (RAND_bytes(r1.data(), r1.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    vector<uint8_t> r2(96);  // esempio: dimensione di r
    if (RAND_bytes(r2.data(), r2.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    PolynomialMatrix<1, PARAM_D> M = pk;

    auto [s1, T1] = feistel.encrypt<PARAM_D>(r1, M);
    auto [s2, T2] = feistel.encrypt<PARAM_D>(r2, M);

    EXPECT_NE(T1, T2);
    EXPECT_NE(s1, s2);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}