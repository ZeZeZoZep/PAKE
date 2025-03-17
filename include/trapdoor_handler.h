#ifndef TRAPDOOR_HANDLER_H
#define TRAPDOOR_HANDLER_H
#include <random>
#include <iostream>
#include <Eigen/Dense>

#include "random.h"
#include "ntt.h"
#include "polynomial_matrix.h"
#include "common.h"
#include "babai.h"
using namespace std;
using namespace Eigen;

class TrapdoorHandler {
    public:
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
            
            return ret.fromNTT();
        }
        template<int Rows, int Cols>
        static PolynomialMatrix<Rows, Cols> generate_uniform_constant_polymatrix() {
            PolynomialMatrix<Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    Polynomial poly(PARAM_N);
                    poly.setZero();
                    poly[0]=uniform_q_random(PARAM_Q);
                    ret(i,j)=poly;
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
                    for(int k=0; k<PARAM_N; k++){
                        //poly[k]=P_random();
                        //poly[k]=gaussian_random(0, PARAM_SIGMA);
                        //if(poly[k]<0)poly[k]=poly[k]*(-1);
                    }
                    //if(poly[0]<0)poly[0]=poly[0]*(-1);
                    ret(i,j)=poly;
                }
            }
            return ret;
        }
        template<int Rows, int Cols>
        static Matrix<int,Rows,Cols> generate_gaussian_matrix() {
            Matrix<int,Rows,Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
//gaussian_random(0, PARAM_SIGMA);
                    ret(i,j)=2;//gaussian_random(0, PARAM_SIGMA);
                }
            }
            return ret;
        }
        template<int Rows, int Cols>
        static Matrix<int,Rows, Cols> generate_identity_matrix() {
            Matrix<int,Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    if(i==j)ret(i,j)=1;
                    else ret(i,j)=0 ;
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
        static Matrix<int,Rows, Cols> generate_gadget_matrix() {
            Matrix<int,Rows, Cols> ret;
            for(int i=0; i<Rows; i++) {
                for(int j=0; j<Cols; j++) {
                    if(i==static_cast<int>(j/PARAM_K))ret(i,j)=(1 << j%PARAM_K);
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
                    if(i==static_cast<int>(j/PARAM_K))poly[0]=(1 << j%PARAM_K);
                    ret(i,j)=poly;
                    }
                }
            return ret;
        }

        // Algoritmo per generare il vettore `a` e la trapdoor `R`
    static pair<PolynomialMatrix<PARAM_D, PARAM_M>,PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>> Trapgen();
    static pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1,PARAM_M>> Invert(PolynomialMatrix<1,PARAM_M>& b,PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>& T);
};
#endif

