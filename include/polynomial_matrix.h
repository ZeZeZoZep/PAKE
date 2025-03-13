#ifndef POLYNOMIAL_MATRIX_H
#define POLYNOMIAL_MATRIX_H

#include <Eigen/Dense>
#include "polynomial.h"

// Classe che estende Eigen::Matrix<Polynomial> e aggiunge un metodo extra
template<int Rows, int Cols>
class PolynomialMatrix : public Eigen::Matrix<Polynomial, Rows, Cols> {
public:
    using Base = Eigen::Matrix<Polynomial, Rows, Cols>;
    using Base::Base;  // Usa i costruttori della classe base
    PolynomialMatrix<Rows, Cols>  PolynomialMatrix<Rows, Cols>::toNTT() const;
    PolynomialMatrix<Rows, Cols>  PolynomialMatrix<Rows, Cols>::fromNTT() const;
};

#endif // POLYNOMIAL_MATRIX_H