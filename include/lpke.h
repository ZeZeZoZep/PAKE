#ifndef LPKE_H
#define LPKE_H
#include <random>
#include <iostream>
#include "common.h"
#include "polynomial_matrix.h"
#include "trapdoor_handler.h"
using namespace std;
using namespace Eigen;
struct Cyphertext {
    PolynomialMatrix<PARAM_D,1> u;
    PolynomialMatrix<1,1> c;
};
class LPKE {  
    private:
        static double compute_norm(PolynomialMatrix<1, PARAM_M>& e);
        //static double R(PolynomialMatrix<1, PARAM_M>& e);
    public:
        int q = PARAM_Q;

        PolynomialMatrix<PARAM_D, PARAM_M> A;
        PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> T;

        PolynomialMatrix<1, PARAM_M> pk;
        PolynomialMatrix<1, PARAM_D> sk;
        
        LPKE();
        ~LPKE();
        pair<PolynomialMatrix<PARAM_D, PARAM_M>,PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>> LSetup() const;
        pair<PolynomialMatrix<1, PARAM_M>,PolynomialMatrix<1, PARAM_D>> LKeyGen(PolynomialMatrix<1, PARAM_M>& v) const;
        Cyphertext LEnc(PolynomialMatrix<1, PARAM_M>& pk, vector<uint8_t>& m, PolynomialMatrix<1, PARAM_M>& v,vector<uint8_t> seed={}) const;
        vector<uint8_t> LDec(PolynomialMatrix<1, PARAM_D>& sk, Cyphertext& ct) const;
        bool IsLossy(PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>& T, PolynomialMatrix<1, PARAM_M>& pk, PolynomialMatrix<1, PARAM_M>& v) const;
};
#endif // LPKE_H

