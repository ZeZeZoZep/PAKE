#ifndef TRAPDOOR_HANDLER_H
#define TRAPDOOR_HANDLER_H
#include <random>
#include <iostream>
#include <Eigen/Dense>
#include "babai.h"
#include "random.h"
#include "ntt.h"
#include "polynomial_matrix.h"
#include "common.h"
using namespace std;
using namespace Eigen;

class TrapdoorHandler {
    private:
        template<int Rows, int Cols>
        static PolynomialMatrix<Rows, Cols> generate_uniform_polymatrix() {
            vector<uint8_t> seed(32);
            for (size_t i = 0; i < seed.size(); i++) {
                seed[i] = static_cast<uint8_t>(i);  // 0, 1, 2, ..., 31
            }
            
            // Due indici (ad es. per diversificare il campionamento)
            uint8_t idx1 = 0x12;
            uint8_t idx2 = 0x34;
            PolynomialMatrix<Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    ret(i,j)=SampleNTT(seed, idx1, idx2);
                }
            }
            return ret;
        }
        template<int Rows, int Cols>
        static PolynomialMatrix<Rows, Cols> generate_gaussian_constant_polymatrix() {
            PolynomialMatrix<Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    Polynomial poly(PARAM_N);
                    poly.setZero();
                    poly[0]=gaussian_random(0, PARAM_SIGMA);
                    ret(i,j)=poly;
                    }
                }
            return ret;
        }
        template<int Rows, int Cols>
        static PolynomialMatrix<Rows, Cols> generate_identity_polymatrix() {
            PolynomialMatrix<Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    Polynomial poly(PARAM_N);
                    poly.setZero();
                    if(i==j)poly[0]=1;
                    ret(i,j)=poly;
                    }
                }
            return ret;
        }
        template<int Rows, int Cols>
        static PolynomialMatrix<Rows, Cols> generate_gadget_polymatrix() {
            PolynomialMatrix<Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    Polynomial poly(PARAM_N);
                    poly.setZero();
                    if(i==static_cast<int>(j/PARAM_K))poly[0]=(1 << i);
                    ret(i,j)=poly;
                    }
                }
            return ret;
        }
    public:
        // Algoritmo per generare il vettore `a` e la trapdoor `R`
    static pair<PolynomialMatrix<PARAM_D, PARAM_M>,PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>> Trapgen();
};
#endif

/* 
void Invert(int *b, int **A, int **R, int q, int n, int m, int k, int *s, int *e);

void TrapGen(int **A, int **R, int n, int m, int q);

int* oracle(int *s,int **A, int *e, int Rows , int Cols, int q); */
