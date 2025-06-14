#include <gtest/gtest.h>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include "2feistel.h"
#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "kem.h"
#include "hash.h"
#include "noic.h"

using namespace std;
using namespace boost::multiprecision;


uint8_t ssid=223;
string password="password123";


TEST(PakeTest, functions) {
    NOIC_PAKE client(ssid,password);
    NOIC_PAKE server(ssid,password,client.Epk);
    EXPECT_EQ(client.rho, server.rho);
    EXPECT_EQ(client.pk, server.pk);
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