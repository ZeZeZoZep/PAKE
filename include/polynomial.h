#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <Eigen/Dense>
#include <iostream>
#include "common.h"
// Dichiarazione della classe Polynomial
class Polynomial : public Eigen::VectorXi {
private:
    int q= PARAM_Q;
public:
    using Eigen::VectorXi::VectorXi;  // Mantiene i costruttori di VectorXi
    bool isNTT=false;
    Polynomial operator+(const Polynomial& other) const;
    Polynomial operator-(const Polynomial& other) const;
    // Operatore di moltiplicazione personalizzato (implementato in polynomial.cpp)
    Polynomial operator*(const Polynomial& other) const;
    Polynomial operator*(int scalar) const;
        // Converte il polinomio nella sua rappresentazione NTT
    Polynomial toNTT() const;

    // Converte un polinomio da NTT alla forma originale
    Polynomial fromNTT() const;
    Polynomial mod(const int& mod) const;
    // Metodo per stampare il polinomio
    void print() const {
        std::cout << *this << std::endl;
    }
};
inline std::ostream& operator<<(std::ostream& os, const Polynomial& poly) {
    os << "Polynomial " << (poly.isNTT ? "(NTT)" : "(Standard)") << ": [";
    for (int i = 0; i < poly.size(); i++) {
        os << poly[i] << (i < poly.size() - 1 ? ", " : "");
    }
    os << "]"<<std::endl;
    return os;
}


#endif // POLYNOMIAL_H
