#include "../include/trapdoor_handler.h"

using namespace std;
using namespace Eigen;


pair<PolynomialMatrix<PARAM_D, PARAM_M>,PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>> TrapdoorHandler::Trapgen() {
    // 1. Generiamo la matrice a_0 in modo uniforme in R_q^{d × d}
    PolynomialMatrix<PARAM_D, PARAM_D> A_hat = generate_uniform_polymatrix<PARAM_D, PARAM_D>();
    //cout << "Matrice a (a0):\n" << A_hat << "\n\n";

    // 3. Generiamo la matrice T ∈ R^{2d×dk} con distribuzione χ
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> T = generate_gaussian_constant_polymatrix<2*PARAM_D,PARAM_D*PARAM_K>();
/*     for(int i=0; i<2*PARAM_D; i++){
        for(int j=0; j<PARAM_D*PARAM_K; j++){
            if(T(i,j)[0]<0)T(i,j)[0]=T(i,j)[0]*-1;
        }
    } */
    
    PolynomialMatrix<PARAM_D, PARAM_D> I = generate_identity_polymatrix<PARAM_D, PARAM_D>();
    

    PolynomialMatrix<PARAM_D, 2*PARAM_D> A_first;
    A_first << I , A_hat;
    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G = generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();
    //G.print_constants();
    // 4. Generiamo g come un vettore riga deterministico (g ∈ R_q^{1×k})
    // 5. Calcoliamo il secondo blocco di `a`
    PolynomialMatrix<PARAM_D,PARAM_D*PARAM_K> A1 = (G - A_first * T);
    //cout << "Matrice a (R_q^{2l × k}):\n" << A_first * T << "\n\n";

    // 6. Costruiamo `a` concatenando `a_0` e `a_1`
    PolynomialMatrix<PARAM_D, PARAM_M> A;
    A << A_first, A1;
    return {A,T};
}
pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1,PARAM_M>> TrapdoorHandler::Invert(PolynomialMatrix<1,PARAM_M>& b, PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>& T) {

    PolynomialMatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D> I = generate_identity_polymatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D>();
    PolynomialMatrix<PARAM_M, PARAM_K*PARAM_D> RI;

    //cout <<"Creazione RI..."<<endl;
    RI.block(0,0,2*PARAM_D,PARAM_D*PARAM_K) = T;
    RI.block(2*PARAM_D,0,PARAM_D*PARAM_K,PARAM_D*PARAM_K) = I;
    //cout <<  << endl;
    //cout << "Fatto" << endl;
    
    //cout <<"Creazione b_hat..."<<endl;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat = b * RI;
    //cout << b_hat << endl;
    
    //cout << "Fatto" << endl;

    PolynomialMatrix<1, PARAM_D> s;
    PolynomialMatrix<1, PARAM_M> e;
    for(int x=0; x<PARAM_M; x++){
        e(0,x)=Polynomial(PARAM_N);
        e(0,x).setZero();
    }

    for(int j=0; j<PARAM_K*PARAM_D; j+=PARAM_K){
        //cout <<"j "<< j <<endl;
        Polynomial s_j(PARAM_N);
        for(int y=0; y<PARAM_N; y++){
            //cout <<"y "<< y <<endl;
            PolynomialMatrix<1, PARAM_K> bj=b_hat.block(0,j,1,PARAM_K);
            VectorXi bjx(PARAM_K);
            for(int x=0; x<PARAM_K; x++){
                bjx(x)=bj(0,x)[y];
            }
            auto [z, e_result] = babai_nearest_plane(bjx);
            //cout<<e_result<<endl;
            s_j[y]=z;
            for(int x=0; x<PARAM_K; x++){

                e(0,x)[y]=e_result(x);
            }
            
        }
        s(0,j/PARAM_K)=s_j;
        
    }
    return {s,e};
} 
/* int main() {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> a = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    // Stampiamo i risultati
    cout << "Matrice a (R_q^{2l × k}):\n" << a << "\n\n";
    cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";

    return 0;
} */
