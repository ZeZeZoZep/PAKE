#include "../include/polynomial_matrix.h"

template<int Rows, int Cols>
class PolynomialMatrix : public Eigen::Matrix<Polynomial, Rows, Cols> {
public:
    using Base = Eigen::Matrix<Polynomial, Rows, Cols>;
    using Base::Base;  // Usa i costruttori della classe base

    // Metodo aggiuntivo: applica il modulo p a ogni elemento della matrice
    PolynomialMatrix<Rows, Cols>  PolynomialMatrix<Rows, Cols>::toNTT() const {
        PolynomialMatrix result(*this);
        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Cols; ++j) {
                result(i, j)=result(i, j).toNTT();  // Usa operator% già definito in Polynomial
            }
        }
        return result;
    }

    // Implementazione di fromNTT()
    PolynomialMatrix<Rows, Cols>  PolynomialMatrix<Rows, Cols>::fromNTT() const {
        PolynomialMatrix result(*this);
        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Cols; ++j) {
                result(i, j)=result(i, j).fromNTT();  // Usa operator% già definito in Polynomial
            }
        }
        return result;
    }
};