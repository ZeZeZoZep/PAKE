#include <gtest/gtest.h>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include "int.h"
#include "lpke.h"
#include "pke.h"
#include "polynomial_matrix_utils.h"
#include "hash.h"


using namespace std;
using namespace boost::multiprecision;
//10496


std::vector<uint8_t> shake256_hash(const std::vector<uint8_t>& input, size_t output_len) {
    std::vector<uint8_t> output(output_len);
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_shake256(), NULL);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinalXOF(ctx, output.data(), output_len);
    EVP_MD_CTX_free(ctx);
    return output;
}
uint8_t ssid=223;
string password="password123";
vector<uint8_t> vec;

LPKE lpke;
PolynomialMatrix<PARAM_D, PARAM_M> A;
PolynomialMatrix<2*PARAM_D, PARAM_D * PARAM_K> T;
PolynomialMatrix<1, PARAM_M> lpk;
PolynomialMatrix<1, PARAM_D> lsk;

PKE pke;
PolynomialMatrix<1, PARAM_D> csk;
PolynomialMatrix<1, PARAM_D> cpk;
vector<uint8_t> rho;

vector<uint8_t> d(32);


    
TEST(PakeTest, Setup) {
  
    auto ret = lpke.LSetup();
    A = ret.first;
    T = ret.second;

    if (RAND_bytes(d.data(), d.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    auto ret2 = pke.KeyGen(d);
    cpk=get<0>(ret2);
    csk=get<1>(ret2);
    rho=get<2>(ret2);
}
TEST(PakeTest, STEP1) {

    KEM kem_server;
    auto ret = kem_server.Encaps(pk,rho);
    auto c=ret.second;
    vector<uint8_t> K=ret.first;

    vector<uint8_t> c1 = PolynomialMatrixUtils::Encode_pm(c.first);
    vector<uint8_t> c2 = PolynomialMatrixUtils::Encode_pm(c.second);
    
    vector<uint8_t> bytes_m = concatVectors(vec,Epk,c1,c2,K);


    //bytes_m = concat(bytes_m, static_cast<vector<uint8_t>>(password));
    //bytes_m = concat(bytes_m, static_cast<vector<uint8_t>>(ssid));

    vector<uint8_t> auth = H(bytes_m);

    
    //CODICE SERVER
    
    vector<uint8_t> c1_server = c1;
    vector<uint8_t> c2_server = c2;

    PolynomialMatrix<1, PARAM_D> A2 = PolynomialMatrixUtils::Decode_pm<1, PARAM_D>(c1_server);
    PolynomialMatrix<1, 1> A3 = PolynomialMatrixUtils::Decode_pm<1, 1>(c2_server);

    
    vector<uint8_t> K_first=kem.Decaps(sk,pk,rho,z,A2,A3);


    vector<uint8_t> c1_s = PolynomialMatrixUtils::Encode_pm(c.first);
    vector<uint8_t> c2_s = PolynomialMatrixUtils::Encode_pm(c.second);


    vector<uint8_t> serve_bytes = concatVectors(vec,Epk,c1_s,c2_s,K_first);

    vector<uint8_t> auth2 = H(serve_bytes);


    EXPECT_EQ(K, K_first);
    EXPECT_EQ(auth, auth2);

    bytes_m=vector<uint8_t>(bytes_m.begin()+password.length(),bytes_m.end());
    serve_bytes=vector<uint8_t>(serve_bytes.begin()+password.length(),serve_bytes.end());

    vector<uint8_t> sk_s = H(bytes_m);
    vector<uint8_t> sk_c = H(serve_bytes);
    EXPECT_EQ(sk_s, sk_c);

}
TEST(PakeTest, functions) {
    ID_PAKE client(ssid,password);
    ID_PAKE server(ssid,password,client.Epk);
    auto ret = server.auth();
    auto c = ret.first;
    auto auth = ret.second;
    vector<uint8_t> sessionkey_s= server.derive(c,auth);
    client.check(c,auth);
    vector<uint8_t> sessionkey_c = client.derive(c,auth);
    EXPECT_EQ(sessionkey_s, sessionkey_c);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}