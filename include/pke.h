#ifndef PKE_H
#define PKE_H
#include <random>
#include <iostream>
#include <Eigen/Dense>

#include "random.h"
#include "ntt.h"
#include "polynomial_matrix.h"
#include "common.h"

using namespace std;
using namespace Eigen;
class PKE{
    public:
        PKE();
        ~PKE();
        tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> KeyGen(const vector<uint8_t>& seed);
        tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> Encrypt(PolynomialMatrix<1, PARAM_D>t,const vector<uint8_t>& rho,const vector<uint8_t>& m, const vector<uint8_t>& r) ;
};


#endif