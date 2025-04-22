#include <gtest/gtest.h>
#include "trapdoor_handler.h"
#include "random.h"
PolynomialMatrix<PARAM_D, PARAM_M> A;
PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R;
PolynomialMatrix<1, PARAM_D> s;
PolynomialMatrix<PARAM_M, PARAM_K*PARAM_D> RI;
PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G;
PolynomialMatrix<1, PARAM_M> e;
TEST(TrapdoorTest, Trapgen2) {
    auto ret = TrapdoorHandler::Trapgen();
    A = ret.first;
    R = ret.second;
    
    // Stampiamo i risultati
    //cout << "Matrice a (R_q^{2l × k}):\n" << a << "\n\n";
    //cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";
    EXPECT_GT(A.rows(), 0) << "A_hat ha zero righe!";
    EXPECT_GT(A.cols(), 0) << "A_hat ha zero colonne!";
    EXPECT_GT(R.rows(), 0) << "R ha zero righe!";
    EXPECT_GT(R.cols(), 0) << "R ha zero colonne!";
}
TEST(TrapdoorTest, Obtain_s_times_G) {
    PolynomialMatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D> I = TrapdoorHandler::generate_identity_polymatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D>();


    //cout <<"Creazione RI..."<<endl;
    RI.block(0,0,2*PARAM_D,PARAM_D*PARAM_K) = R;
    RI.block(2*PARAM_D,0,PARAM_D*PARAM_K,PARAM_D*PARAM_K) = I;
    //cout << T << endl;
    //cout << "Fatto" << endl;


    G = TrapdoorHandler::generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();
    s = TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();


    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();

        for(int y=0; y<PARAM_N; y++){
            poly[y]=P_random();

        } 
        e(0,j)=poly;
    }

    PolynomialMatrix<1, PARAM_M> b=s*A+e;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat = b * RI;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat2 = s*G+e*RI;

    EXPECT_EQ(b_hat,b_hat2) << "they should be equal";

}
TEST(TrapdoorTest, ARI_G) {

    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K>  ARI=A*RI;
    EXPECT_EQ(ARI,G) << "they should be equal";

}

TEST(TrapdoorTest, Invert) {



    // Calcola public key
    PolynomialMatrix<1, PARAM_M> b=s*A+e;
    //cout << "Public Key b (R_q^{1 × m}):\n" << b << std::endl;
    
    auto ret2 = TrapdoorHandler::Invert(b,R);
    PolynomialMatrix<1, PARAM_D> s2 = ret2.first;
    PolynomialMatrix<1, PARAM_M> e2 = b-s2*A;
    for(int j=0; j<PARAM_M; j++){
        for(int y=0; y<PARAM_N; y++){
            if(e2(0,j)[y]>PARAM_Q/2)e2(0,j)[y]=e2(0,j)[y]-PARAM_Q;
        }
    }
    //cout << "Secret Key s2 (R_q^{1 × d}):\n" << s2 << "\n\n";
    //cout << "Error e2 (R_q^{1 × m}):\n" << e2 << "\n\n";

    EXPECT_EQ(s2,s) << "they should be equal";
    EXPECT_EQ(e,e2)<< "they should be equal";
} 

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}