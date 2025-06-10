#include <gtest/gtest.h>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include "ideal_cipher.h"
#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"



/* TEST(IdealCipherTest, EncDec) {

    PolynomialMatrix<4, 4> A;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            Polynomial p1(256);
            for(int i=0; i<256; i++)
                p1[i] = 1;
            A(i, j)=p1;
        }
    
    std::vector<uint32_t> flat = PolynomialMatrixUtils::Encode(A);
    cpp_int m = vector_to_bigint_baseq(flat, PARAM_Q);

    IdealCipher ideal_cipher;
    
    cpp_int c = ideal_cipher.encrypt_cpp_int(m);
    cpp_int m2 = ideal_cipher.decrypt_cpp_int(c);

    std::vector<uint32_t> flat2 = bigint_to_vector_baseq(m2, PARAM_Q, flat.size());
    PolynomialMatrix<4, 4> A2 = PolynomialMatrixUtils::Decode<4, 4>(flat2);

    EXPECT_EQ(m , m2);
} */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}