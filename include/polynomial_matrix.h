#ifndef POLYNOMIAL_MATRIX_H
#define POLYNOMIAL_MATRIX_H

#include <Eigen/Dense>
#include "polynomial.h"
#include "polynomial_traits.h"  // Includi la specializzazione NumTraits

using namespace std;
// Classe che estende Eigen::Matrix<Polynomial> e aggiunge un metodo extra
template<int Rows, int Cols>
class PolynomialMatrix : public Eigen::Matrix<Polynomial, Rows, Cols> {
public:
    using Base = Eigen::Matrix<Polynomial, Rows, Cols>;
    using Base::Base;  // Usa i costruttori della classe base
    template<int OtherCols>
    PolynomialMatrix<Rows, OtherCols> operator*(const PolynomialMatrix<Cols, OtherCols>& other) const;
    PolynomialMatrix toNTT() const;
    PolynomialMatrix fromNTT() const;
};

// Metodo aggiuntivo: applica il modulo p a ogni elemento della matrice
template<int Rows, int Cols>
PolynomialMatrix<Rows, Cols>  PolynomialMatrix<Rows, Cols>::toNTT() const {
    PolynomialMatrix<Rows, Cols> result;
    for (int i = 0; i < Rows; ++i) {
        for (int j = 0; j < Cols; ++j) {
            result(i, j)=this->operator()(i, j).toNTT();  // Usa operator% già definito in Polynomial
        }
    }
    return result;
}

// Implementazione di fromNTT()
template<int Rows, int Cols>
PolynomialMatrix<Rows, Cols>  PolynomialMatrix<Rows, Cols>::fromNTT() const {
    PolynomialMatrix<Rows, Cols>  result;
    for (int i = 0; i < Rows; ++i) {
        for (int j = 0; j < Cols; ++j) {
            result(i, j)=this->operator()(i, j).fromNTT();  // Usa operator% già definito in Polynomial
        }
    }
    return result;
}
// Implementazione di `operator*`
template<int Rows, int Cols>
template<int OtherCols>
PolynomialMatrix<Rows, OtherCols> PolynomialMatrix<Rows, Cols>::operator*(
    const PolynomialMatrix<Cols, OtherCols>& other) const {
    PolynomialMatrix<Rows, Cols> first=this->toNTT();
    PolynomialMatrix<Cols, OtherCols> second=other.toNTT();
    PolynomialMatrix<Rows, OtherCols> result;
    for (int i = 0; i < Rows; ++i) {
        for (int j = 0; j < OtherCols; ++j) {
            result(i, j) = Polynomial(first(i, 0).size());  // Inizializza con lo stesso grado
            result(i, j).setZero();
            for (int k = 0; k < Cols; ++k) {
                Polynomial prod=first(i, k)*second(k, j);
                prod=prod.fromNTT();
                result(i, j) = result(i, j) + prod;
            }
        }
    }
    return result;
}

#endif // POLYNOMIAL_MATRIX_H
