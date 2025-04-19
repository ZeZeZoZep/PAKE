#include <gtest/gtest.h>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include "ideal_cipher.h"
#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "kem.h"
#include "hash.h"

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
uint8_t ssid=223;
string password="password123";
vector<uint8_t> vec;
IdealCipher ideal_cipher;
PolynomialMatrix<1, PARAM_D> sk;
PolynomialMatrix<1, PARAM_D> pk;
vector<uint8_t> rho;
vector<uint8_t> z;
vector<uint8_t> Epk;

KEM kem;

vector<uint8_t> concat(const vector<uint8_t>& a, const vector<uint8_t>& b){
    vector<uint8_t> c(a.size() + b.size());
    copy(a.begin(), a.end(), c.begin());
    copy(b.begin(), b.end(), c.begin() + a.size());
    return c;
}

    
TEST(PakeTest, STEP0) {
    //CODICE CLIENT
    vec=vector<uint8_t>(password.begin(), password.end());
    vec.push_back(ssid);
    vec=H(vec);
    ideal_cipher=IdealCipher(vector<uint8_t>(vec.begin(),vec.begin() + 16));

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
    Epk = c_bytes;
    vector<uint8_t> m2_bytes = ideal_cipher.decrypt_bytes(c_bytes);

    vector<uint8_t> rho2;
    vector<uint8_t> pk_bytes2;
    pk_bytes2.insert(pk_bytes2.begin(),m2_bytes.begin(),m2_bytes.end() - size(rho));
    rho2.insert(rho2.begin(),m2_bytes.end()- size(rho),m2_bytes.end());

    cpp_int m2=IdealCipher::bytes_to_cpp_int(pk_bytes2);
    
    std::vector<uint16_t> flat2 = bigint_to_vector_baseq(m2, PARAM_Q, flat.size());
    PolynomialMatrix<1, PARAM_D> A2 = PolynomialMatrixUtils::Decode<1, PARAM_D>(flat2);

    EXPECT_EQ(pk_int , m2);
    EXPECT_EQ(rho , rho2);
    EXPECT_EQ(flat, flat2);
    EXPECT_EQ(pk, A2);
}
TEST(PakeTest, STEP1) {

    KEM kem_server;
    auto ret = kem_server.Encaps(pk,rho);
    auto c=ret.second;
    vector<uint8_t> K=ret.first;

    std::vector<uint16_t> flat;
    cpp_int int_m;

    vector<uint8_t> bytes_m; 

    flat = PolynomialMatrixUtils::Encode(c.first);
    int_m = vector_to_bigint_baseq(flat, PARAM_Q);
    vector<uint8_t> c1 = IdealCipher::cpp_int_to_padded_bytes(int_m,16);
    flat = PolynomialMatrixUtils::Encode(c.second);
    int_m = vector_to_bigint_baseq(flat, PARAM_Q);
    vector<uint8_t> c2 = IdealCipher::cpp_int_to_padded_bytes(int_m,16);
    
    bytes_m = concat(bytes_m, vec);
    bytes_m = concat(bytes_m, Epk);
    bytes_m = concat(bytes_m, c1);
    bytes_m = concat(bytes_m, c2);
    bytes_m = concat(bytes_m, K);


    //bytes_m = concat(bytes_m, static_cast<vector<uint8_t>>(password));
    //bytes_m = concat(bytes_m, static_cast<vector<uint8_t>>(ssid));

    vector<uint8_t> auth = H(bytes_m);

    
    //CODICE SERVER
    
    vector<uint8_t> c1_server = c1;
    vector<uint8_t> c2_server = c2;
    cpp_int c1_int;
    cpp_int c2_int;
    c1_int=IdealCipher::bytes_to_cpp_int(c1_server);
    c2_int=IdealCipher::bytes_to_cpp_int(c2_server);

    vector<uint16_t> flat1_server = bigint_to_vector_baseq(c1_int, PARAM_Q, PARAM_D*PARAM_N);
    vector<uint16_t> flat2_server = bigint_to_vector_baseq(c2_int, PARAM_Q, PARAM_N);

    PolynomialMatrix<1, PARAM_D> A2 = PolynomialMatrixUtils::Decode<1, PARAM_D>(flat1_server);
    PolynomialMatrix<1, 1> A3 = PolynomialMatrixUtils::Decode<1, 1>(flat2_server);

    
    vector<uint8_t> K_first=kem.Decaps(sk,pk,rho,z,A2,A3);


    flat = PolynomialMatrixUtils::Encode(c.first);
    int_m = vector_to_bigint_baseq(flat, PARAM_Q);
    vector<uint8_t> c1_s = IdealCipher::cpp_int_to_padded_bytes(int_m,16);
    flat = PolynomialMatrixUtils::Encode(c.second);
    int_m = vector_to_bigint_baseq(flat, PARAM_Q);
    vector<uint8_t> c2_s = IdealCipher::cpp_int_to_padded_bytes(int_m,16);
    vector<uint8_t> serve_bytes;

    serve_bytes = concat(serve_bytes, vec);
    serve_bytes = concat(serve_bytes, Epk);
    serve_bytes = concat(serve_bytes, c1_s);
    serve_bytes = concat(serve_bytes, c2_s);
    serve_bytes = concat(serve_bytes, K_first);
    vector<uint8_t> auth2 = H(serve_bytes);


    EXPECT_EQ(K, K_first);
    EXPECT_EQ(auth, auth2);

    bytes_m=vector<uint8_t>(bytes_m.begin()+password.length(),bytes_m.end());

    serve_bytes=vector<uint8_t>(serve_bytes.begin()+password.length(),serve_bytes.end());
    vector<uint8_t> sk_s = H(bytes_m);
    vector<uint8_t> sk_c = H(serve_bytes);
    EXPECT_EQ(sk_s, sk_c);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}