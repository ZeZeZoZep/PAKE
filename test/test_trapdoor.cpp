#include <gtest/gtest.h>
#include "trapdoor_handler.h"
// Test generazione
TEST(TrapdoorTest, Trapgen) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> a = ret.first;
    Matrix<int,2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    // Stampiamo i risultati
    //cout << "Matrice a (R_q^{2l × k}):\n" << a << "\n\n";
    //cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";
    EXPECT_GT(a.rows(), 0) << "A_hat ha zero righe!";
    EXPECT_GT(a.cols(), 0) << "A_hat ha zero colonne!";
    EXPECT_GT(R.rows(), 0) << "R ha zero righe!";
    EXPECT_GT(R.cols(), 0) << "R ha zero colonne!";
}
TEST(TrapdoorTest, Obtain_s_times_G) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> A = ret.first;
    Matrix<int,2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    Matrix<int,PARAM_K*PARAM_D, PARAM_K*PARAM_D> I = TrapdoorHandler::generate_identity_matrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D>();
    Matrix<int,PARAM_M, PARAM_K*PARAM_D> RI;

    //cout <<"Creazione RI..."<<endl;
    RI.block(0,0,2*PARAM_D,PARAM_D*PARAM_K) = R;
    RI.block(2*PARAM_D,0,PARAM_D*PARAM_K,PARAM_D*PARAM_K) = I;
    //cout << T << endl;
    //cout << "Fatto" << endl;


    Matrix<int,PARAM_D, PARAM_D*PARAM_K> G = TrapdoorHandler::generate_gadget_matrix<PARAM_D, PARAM_D*PARAM_K>();
    //cout << "Generate Secret Key s (R_q^{1 × d})...\n\n"<< std::endl;
    PolynomialMatrix<1, PARAM_D> s;
    for(int j=0; j<PARAM_D; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=j+1;
        }
        s(0,j)=poly;
        //cout << "ciao ciao ciao "<< j << "\n\n" << std::endl;
        
    }
    //cout << "Secret Key e (R_q^{1 × m}):\n" << e << std::endl;

    // Calcola public key
    cout<<"prova"<<endl;
    PolynomialMatrix<1, PARAM_M> b=s*A;
    cout<<b<<endl;
    cout<<"prova2"<<endl;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat = b * RI;
    cout<<"prov3"<<endl;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat2 = s*G;
    cout<<G<<endl;
    cout<<RI<<endl;
    //cout << "Public Key b (R_q^{1 × m}):\n" << b << std::endl;
    //cout << "Secret Key s2 (R_q^{1 × d}):\n" << s2 << "\n\n";
    //cout << "Secret Key e2 (R_q^{1 × m}):\n" << e2 << "\n\n";

/*     for(int j=0; j<PARAM_K*PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            EXPECT_EQ(b_hat2(0,j)[y],b_hat(0,j)[y]) << "A_hat ha zero righe!";
        }
    }  */
    EXPECT_EQ(b_hat2(0,0)[0],b_hat(0,0)[0]);
}
/* TEST(TrapdoorTest, Invert) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> A = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    // Stampiamo i risultati
    //cout << "Matrice a (R_q^{2l × k}):\n" << A << "\n\n";
    //cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";
    
    //cout << "Generate Secret Key s (R_q^{1 × d})...\n\n"<< std::endl;
    PolynomialMatrix<1, PARAM_D> s;
    for(int j=0; j<PARAM_D; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=y+j+1;
        }
        s(0,j)=poly;
        //cout << "ciao ciao ciao "<< j << "\n\n" << std::endl;
        
    }
    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G = TrapdoorHandler::generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat2 = s*G;
    G.print_constants();

    //cout << "Secret Key s (R_q^{1 × d}):\n" << G << std::endl;
    //cout << "Secret Key s (R_q^{1 × d}):\n" << s << std::endl;

    PolynomialMatrix<1, PARAM_M> e;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        for(int y=0; y<PARAM_N; y++){
            poly[y]=0; //gaussian_random(0, 2);
        }
        e(0,j)=poly;
    }
    //cout << "Secret Key e (R_q^{1 × m}):\n" << e << std::endl;

    // Calcola public key
    PolynomialMatrix<1, PARAM_M> b=s*A-e;
    //cout << "Public Key b (R_q^{1 × m}):\n" << b << std::endl;
    
    auto ret2 = TrapdoorHandler::Invert(b,R);
    PolynomialMatrix<1, PARAM_D> s2 = ret2.first;
    PolynomialMatrix<1, PARAM_M> e2 = ret2.second;
    //cout << "Secret Key s2 (R_q^{1 × d}):\n" << s2 << "\n\n";
    //cout << "Secret Key e2 (R_q^{1 × m}):\n" << e2 << "\n\n";

    for(int j=0; j<PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            EXPECT_EQ(s2(0,j)[y],s(0,j)[y]) << "A_hat ha zero righe!";
        }
    }
    //EXPECT_EQ(s2(0,0)[0],s(0,0)[0]);
} */
// Esegue tutti i test
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}