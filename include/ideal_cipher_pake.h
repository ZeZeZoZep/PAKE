#ifndef IDEAL_CIPHER_PAKE_H
#define IDEAL_CIPHER_PAKE_H


#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include "ideal_cipher.h"
#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "kem.h"
#include "hash.h"
class ID_PAKE{
private:
    uint ssid;
    string password;
    IdealCipher ideal_cipher;
    KEM kem;
    PolynomialMatrix<1, PARAM_D> sk;
    vector<uint8_t> K;
    vector<uint8_t> z;
public:
    PolynomialMatrix<1, PARAM_D> pk;
    vector<uint8_t> rho;
    vector<uint8_t> Epk;


    ID_PAKE(uint ssid, string password, vector<uint8_t> Epk = {});
    ~ID_PAKE(){};
    pair<vector<uint8_t>,vector<uint8_t>> auth();
    bool check(vector<uint8_t> c, vector<uint8_t> auth);
    vector<uint8_t> derive(vector<uint8_t> c, vector<uint8_t> auth);
};



#endif // IDEAL_CIPHER_H