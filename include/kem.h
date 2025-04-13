#ifndef KEM_H
#define KEM_H
#include <random>
#include <iostream>
#include <Eigen/Dense>
#include <openssl/rand.h>
#include "random.h"
#include "pke.h"
#include "ntt.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "hash.h"
#include "common.h"

using namespace std;
using namespace Eigen;
class KEM{
    private:
        PKE pke;
        PolynomialMatrix<1, PARAM_D> sk;
        PolynomialMatrix<1, PARAM_D> pk;
        vector<uint8_t> rho;

        tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> 
        KeyGen_internal(
            vector<uint8_t>& d,
            vector<uint8_t>& z
        );
        
        pair<vector<uint8_t>,pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, 1>>> 
        Encaps_internal(
            PolynomialMatrix<1, PARAM_D> pk,
            vector<uint8_t>& rho,
            vector<uint8_t>& m
        );

        vector<uint8_t>
        Decaps_internal(
            PolynomialMatrix<1, PARAM_D> sk,
            PolynomialMatrix<1, PARAM_D> pk,
            vector<uint8_t>& rho,
            vector<uint8_t>& z, 
            PolynomialMatrix<1, PARAM_D> c1,
            PolynomialMatrix<1, 1> c2 
        );

    public:
        KEM();
        ~KEM();

        pair<tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>>,vector<uint8_t>> 
        KeyGen();

        pair<vector<uint8_t>,pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, 1>>> 
        Encaps(
            PolynomialMatrix<1, PARAM_D> pk,
            vector<uint8_t>& rho
        );
        vector<uint8_t>
        Decaps(
            PolynomialMatrix<1, PARAM_D> sk,
            PolynomialMatrix<1, PARAM_D> pk,
            vector<uint8_t>& rho,
            vector<uint8_t>& z, 
            PolynomialMatrix<1, PARAM_D> c1,
            PolynomialMatrix<1, 1> c2 
        );


};

#endif // KEM_H

