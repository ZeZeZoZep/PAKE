#ifndef POLYNOMIAL_MATRIX_H
#define POLYNOMIAL_MATRIX_H

#include <Eigen/Dense>
#include "polynomial.h"
#include "polynomial_traits.h"  // Includi la specializzazione NumTraits
#include <type_traits>
using namespace std;
using namespace Eigen;
// Classe che estende Eigen::Matrix<Polynomial> e aggiunge un metodo extra
template<int Rows, int Cols>
class PolynomialMatrix : public Eigen::Matrix<Polynomial, Rows, Cols> {
public:
    using Base = Eigen::Matrix<Polynomial, Rows, Cols>;
    using Base::Base;  // Usa i costruttori della classe base
    template<int OtherCols>
    PolynomialMatrix<Rows, OtherCols> operator*(const PolynomialMatrix<Cols, OtherCols>& other) const;
    template<int OtherCols>
    PolynomialMatrix<Rows, OtherCols> operator*(const Matrix<int, Cols, OtherCols>& other) const;
    void print_constants() const;
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
// Implementazione di print()
template<int Rows, int Cols>
inline std::ostream& operator<<(std::ostream& os, const PolynomialMatrix<Rows, Cols>& poly) {
    os << "Matrix: [\n";
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Cols; j++) {
            os <<"{ " << i << ", " << j << " } " << poly(i, j);
        }
    }
    os << "]"<<std::endl;
    return os;
}
template<int Rows, int Cols>
void PolynomialMatrix<Rows, Cols>::print_constants() const {
    cout << "Matrix_constants: [\n";
    for (int i = 0; i < Rows; ++i) {
        for (int j = 0; j < Cols; ++j) {
            cout << this->operator()(i, j)[0]<< (j < Cols - 1 ? ", " : "\n");  // Usa operator% già definito in Polynomial
        }
    }
    cout<<"]"<<endl;
}

// Implementazione di `operator*`
template<int Rows, int Cols>
template<int OtherCols>
PolynomialMatrix<Rows, OtherCols> PolynomialMatrix<Rows, Cols>::operator*(
    const PolynomialMatrix<Cols, OtherCols>& other) const {
    cout<<"PERCHE!!!!"<<endl;
    PolynomialMatrix<Rows, Cols> first;
    PolynomialMatrix<Cols, OtherCols> second;
    for(int i=0;i<Rows;i++){
        for (int j = 0; j < Cols; ++j) {
            first(i,j)=this->operator()(i,j).mod(PARAM_Q);
        }
    }
    for(int i=0;i<Cols;i++){
        for (int j = 0; j < OtherCols; ++j) {
            second(i,j)=other(i,j).mod(PARAM_Q);
        }
    }   
    first=first.toNTT();
    second=second.toNTT();
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
// Implementazione di `operator*`
template<int Rows, int Cols>
template<int OtherCols>
PolynomialMatrix<Rows, OtherCols> PolynomialMatrix<Rows, Cols>::operator*(
    const Matrix<int, Cols, OtherCols>& other) const {
/*     if (other.rows() != Rows || other.cols() != Cols) {
        throw std::invalid_argument("Le dimensioni delle matrici devono corrispondere per la moltiplicazione elemento per elemento.");
    } */
    
    PolynomialMatrix<Rows, OtherCols> result;
    for (int i = 0; i < Rows; ++i) {
        for (int j = 0; j < other.cols(); ++j) {
            Polynomial a(PARAM_N);
            result(i, j)=a.setZero();
            for (int k = 0; k < Cols; ++k) {
                Polynomial prod=(*this)(i, k)*static_cast<int>(other(k, j));
                
                result(i, j) = result(i, j) + prod;
            }
        }
    }
    return result;
}

#endif // POLYNOMIAL_MATRIX_H
