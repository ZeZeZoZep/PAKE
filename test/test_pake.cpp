#include <gtest/gtest.h>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include "ideal_cipher.h"
#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "kem.h"

using namespace std;
using namespace boost::multiprecision;

/* vector<uint8_t> cpp_int_to_bytes(const cpp_int& value) {
    vector<uint8_t> bytes;
    export_bits(value, std::back_inserter(bytes), 8);
    return bytes;
}
cpp_int bytes_to_cpp_int(const vector<uint8_t>& bytes) {
    cpp_int result;
    import_bits(result, bytes.begin(), bytes.end(), 8, true);  // false = MSB first (big endian)
    return result;
}
 */
IdealCipher ideal_cipher;
PolynomialMatrix<1, PARAM_D> sk;
PolynomialMatrix<1, PARAM_D> pk;
vector<uint8_t> rho;
vector<uint8_t> z;
KEM kem;
    
TEST(PakeTest, STEP0) {
    //CODICE CLIENT
    KEM kem;
    
    auto ret = kem.KeyGen();
    auto keygen_ret = ret.first;
    
    pk=get<0>(keygen_ret);
    sk=get<1>(keygen_ret);
    rho=get<2>(keygen_ret); 

    std::vector<uint16_t> flat = PolynomialMatrixUtils::Encode(pk);

    cpp_int pk_int = vector_to_bigint_baseq(flat, PARAM_Q);
    vector<uint8_t> pk_bytes = IdealCipher::cpp_int_to_padded_bytes(pk_int,16);
    pk_bytes.insert(pk_bytes.end(),rho.begin(),rho.end());

    
    vector<uint8_t> c_bytes = ideal_cipher.encrypt_bytes(pk_bytes);
    
    //CODICE SERVER
    
    vector<uint8_t> m2_bytes = ideal_cipher.decrypt_bytes(c_bytes);

    vector<uint8_t> rho2;
    vector<uint8_t> pk_bytes2;
    pk_bytes2.insert(pk_bytes2.begin(),m2_bytes.begin(),m2_bytes.end() - size(rho));
    rho2.insert(rho2.begin(),m2_bytes.end()- size(rho),m2_bytes.end());

    cpp_int m2=IdealCipher::bytes_to_cpp_int(pk_bytes2);
    
    std::vector<uint16_t> flat2 = bigint_to_vector_baseq(m2, PARAM_Q, flat.size());
    PolynomialMatrix<1, 4> A2 = PolynomialMatrixUtils::Decode<1, 4>(flat2);

    EXPECT_EQ(pk_int , m2);
    EXPECT_EQ(rho , rho2);
    EXPECT_EQ(flat, flat2);
    EXPECT_EQ(pk, A2);
}
TEST(PakeTest, STEP1) {

    KEM kem_server;
    kem_server.Encaps(pk,rho);


    cpp_int pk_int = vector_to_bigint_baseq(flat, PARAM_Q);
    vector<uint8_t> pk_bytes = IdealCipher::cpp_int_to_padded_bytes(pk_int,16);
    pk_bytes.insert(pk_bytes.end(),rho.begin(),rho.end());

    
    vector<uint8_t> c_bytes = ideal_cipher.encrypt_bytes(pk_bytes);
    
    //CODICE SERVER
    
    vector<uint8_t> m2_bytes = ideal_cipher.decrypt_bytes(c_bytes);

    vector<uint8_t> rho2;
    vector<uint8_t> pk_bytes2;
    pk_bytes2.insert(pk_bytes2.begin(),m2_bytes.begin(),m2_bytes.end() - size(rho));
    rho2.insert(rho2.begin(),m2_bytes.end()- size(rho),m2_bytes.end());

    cpp_int m2=IdealCipher::bytes_to_cpp_int(pk_bytes2);
    
    std::vector<uint16_t> flat2 = bigint_to_vector_baseq(m2, PARAM_Q, flat.size());
    PolynomialMatrix<1, 4> A2 = PolynomialMatrixUtils::Decode<1, 4>(flat2);

    EXPECT_EQ(pk_int , m2);
    EXPECT_EQ(rho , rho2);
    EXPECT_EQ(flat, flat2);
    EXPECT_EQ(pk, A2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}