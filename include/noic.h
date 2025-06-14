#ifndef NOIC_H
#define NOIC_H

#include <iostream>
#include <vector>
#include <string>
       

#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>

#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "kem.h"
#include "hash.h"
#include "../include/2feistel.h"

using namespace std;
using boost::multiprecision::cpp_int;

class NOIC_PAKE{
private:
    uint ssid;
    string password;
    Feistel feistel;
    KEM kem;
    PolynomialMatrix<1, PARAM_D> sk;
    vector<uint8_t> K;
    vector<uint8_t> z;
    vector<uint8_t> r;

public:
    PolynomialMatrix<1, PARAM_D> pk;

    vector<uint8_t> rho;
    vector<uint8_t> pk_rho_bytes;
    vector<uint8_t> Epk;


    NOIC_PAKE(uint ssid, string password, vector<uint8_t> Epk = {});
    ~NOIC_PAKE(){};
    pair<vector<uint8_t>,vector<uint8_t>> auth();
    bool check(vector<uint8_t> c, vector<uint8_t> auth);
    vector<uint8_t> derive(vector<uint8_t> c, vector<uint8_t> auth);
};



#endif