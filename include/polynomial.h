#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <Eigen/Dense>
#include <iostream>
#include "common.h"
// Dichiarazione della classe Polynomial
class Polynomial : public Eigen::VectorXi {
public:
    using Eigen::VectorXi::VectorXi;  // Mantiene i costruttori di VectorXi
    bool isNTT;
    Polynomial operator+(const Polynomial& other) const;
    Polynomial operator-(const Polynomial& other) const;
    // Operatore di moltiplicazione personalizzato (implementato in polynomial.cpp)
    Polynomial operator*(const Polynomial& other) const;
        // Converte il polinomio nella sua rappresentazione NTT
    Polynomial toNTT() const;

    // Converte un polinomio da NTT alla forma originale
    Polynomial fromNTT() const;
    // Metodo per stampare il polinomio
    void print() const {
        std::cout << "Polynomial: [";
        for (int i = 0; i < this->size(); i++) {
            std::cout << (*this)[i] << (i < this->size() - 1 ? ", " : "");
        }
        std::cout << "]\n";
    }
};

#endif // POLYNOMIAL_H
