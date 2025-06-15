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
#include "timerset.h"


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
PolynomialMatrix<1, PARAM_M> b;

PKE pke;
PolynomialMatrix<1, PARAM_D> csk;
PolynomialMatrix<1, PARAM_D> cpk;
vector<uint8_t> rho;

vector<uint8_t> d(32);
vector<uint8_t> c1,c2,bytes,m;
pair<PolynomialMatrix<1, 2>, PolynomialMatrix<1, 1>> C;
Cyphertext ct;
TimerSet ts("PAKE_ROM", "Exchange");

TEST(PakeTest, Setup) {
  
    auto ret = lpke.LSetup();
    A = ret.first;
    T = ret.second;
    ts.start("Total");
    if (RAND_bytes(d.data(), d.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    auto ret2 = pke.KeyGen(d);
    cpk=get<0>(ret2);
    csk=get<1>(ret2);
    rho=get<2>(ret2);
}
TEST(PakeTest, STEP1) {
    b=PolynomialMatrixUtils::Decode_pm<1, PARAM_M>(shake256_hash(vector<uint8_t>(password.begin(), password.end()),10496));
    auto ret = lpke.LKeyGen(b);
    lpk=ret.first;
    lsk=ret.second;
}

TEST(PakeTest, STEP2) {
    m=vector<uint8_t>(32);
    if (RAND_bytes(m.data(), m.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    ct = lpke.LEnc(lpk,m,b,H(m));
    vector<uint8_t> lpk_bytes = PolynomialMatrixUtils::Encode_pm(lpk);
    c1 = PolynomialMatrixUtils::Encode_pm(ct.u);
    c2 = PolynomialMatrixUtils::Encode_pm(ct.c);
    bytes = concatVectors(vector<uint8_t>(password.begin(), password.end()), lpk_bytes,c1,c2);
    C=pke.Encrypt(cpk,rho,bytes,m);
}

TEST(PakeTest, STEP3) {
    Cyphertext ct;
    ct.u = PolynomialMatrixUtils::Decode_pm<PARAM_D, 1>(c1);
    ct.c = PolynomialMatrixUtils::Decode_pm<1, 1>(c2);
    vector<uint8_t> m_check = lpke.LDec(lsk,ct);

    Cyphertext ct_check = lpke.LEnc(lpk,m_check,b,H(m_check));
    vector<uint8_t> lpk_bytes = PolynomialMatrixUtils::Encode_pm(lpk);
    vector<uint8_t> c1_check = PolynomialMatrixUtils::Encode_pm(ct_check.u);
    vector<uint8_t> c2_check = PolynomialMatrixUtils::Encode_pm(ct_check.c);
    vector<uint8_t> bytes_check = concatVectors(vector<uint8_t>(password.begin(), password.end()), lpk_bytes,c1_check,c2_check);

    pair<PolynomialMatrix<1, 2>, PolynomialMatrix<1, 1>> C_check=pke.Encrypt(cpk,rho,bytes_check,m_check);
    ts.stop("Total");
    ts.commit(PARAM_D);

    EXPECT_EQ(m_check, m);
    EXPECT_EQ(C, C_check);
    EXPECT_EQ(ct.c, ct_check.c);
    EXPECT_EQ(ct.u, ct_check.u);

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}