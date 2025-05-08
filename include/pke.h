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
class PKE {
    public:
        PKE();
        virtual ~PKE();  // distruttore virtuale, buona pratica
    
        // rendiamo virtual tutti i metodi che vogliamo ridefinire
        virtual
        tuple<PolynomialMatrix<1, PARAM_D>,
              PolynomialMatrix<1, PARAM_D>,
              vector<uint8_t>>
        KeyGen(const vector<uint8_t>& seed);
    
        virtual
        pair<PolynomialMatrix<1, PARAM_D>,
             PolynomialMatrix<1, 1>>
        Encrypt(PolynomialMatrix<1, PARAM_D> t,
                const vector<uint8_t>& rho,
                const vector<uint8_t>& m,
                const vector<uint8_t>& r);
    
        virtual
        vector<uint8_t>
        Decrypt(PolynomialMatrix<1, PARAM_D> u,
                PolynomialMatrix<1, 1> v,
                PolynomialMatrix<1, PARAM_D> s);
    };
    


#endif