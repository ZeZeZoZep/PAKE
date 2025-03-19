#include <gtest/gtest.h>
#include "trapdoor_handler.h"
#include "random.h"
TEST(TrapdoorTest, Trapgen2) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> a = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
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
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    PolynomialMatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D> I = TrapdoorHandler::generate_identity_polymatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D>();
    PolynomialMatrix<PARAM_M, PARAM_K*PARAM_D> RI;

    //cout <<"Creazione RI..."<<endl;
    RI.block(0,0,2*PARAM_D,PARAM_D*PARAM_K) = R;
    RI.block(2*PARAM_D,0,PARAM_D*PARAM_K,PARAM_D*PARAM_K) = I;
    //cout << T << endl;
    //cout << "Fatto" << endl;


    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G = TrapdoorHandler::generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();
    //cout << "Generate Secret Key s (R_q^{1 × d})...\n\n"<< std::endl;
    PolynomialMatrix<1, PARAM_D> s=TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();
/*     PolynomialMatrix<1, PARAM_D> s;
    for(int j=0; j<PARAM_D; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=y+j+1;
        }
        s(0,j)=poly; }*/
        //cout << "ciao ciao ciao "<< j << "\n\n" << std::endl;
        
    
    //cout << "Secret Key e (R_q^{1 × m}):\n" << e << std::endl;
    PolynomialMatrix<1, PARAM_M> e;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=P_random();
            //if(poly[y]<0)poly[y]=poly[y]*(-1);
        } 
        e(0,j)=poly;
    }
    //cout<<"TESTTESTTEST"<< e*RI<< endl;
    PolynomialMatrix<1, PARAM_M> b=s*A+e;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat = b * RI;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat2 = s*G+e*RI;

    //cout << "Public Key b (R_q^{1 × m}):\n" << b_hat << std::endl;
    //G.print_constants();
    //RI.print_constants();
    //cout << "Public Key b (R_q^{1 × m}):\n" << b << std::endl;
    //cout << "Secret Key s2 (R_q^{1 × d}):\n" << s2 << "\n\n";
    //cout << "Secret Key e2 (R_q^{1 × m}):\n" << e2 << "\n\n";
    int error=0;
    for(int j=0; j<PARAM_K*PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            if(b_hat2(0,j)[y]!=b_hat(0,j)[y]){
                //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                error++;
            }
        }
    }
    EXPECT_EQ(error,0)<< "on "<< PARAM_K*PARAM_D*PARAM_N<< " retrieved integers";
}
TEST(TrapdoorTest, ARI_G) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> A = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    PolynomialMatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D> I = TrapdoorHandler::generate_identity_polymatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D>();
    PolynomialMatrix<PARAM_M, PARAM_K*PARAM_D> RI;

    RI.block(0,0,2*PARAM_D,PARAM_D*PARAM_K) = R;
    RI.block(2*PARAM_D,0,PARAM_D*PARAM_K,PARAM_D*PARAM_K) = I;

    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G = TrapdoorHandler::generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();


    //cout<<"TESTTESTTEST"<< e*RI<< endl;
    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K>  ARI=A*RI;
    int error=0;
    for(int i=0; i<PARAM_D; i++){
        for(int j=0; j<PARAM_K*PARAM_D; j++){
            for(int y=0; y<PARAM_N; y++){
                if(ARI(i,j)[y]!=G(i,j)[y]){
                    //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                    error++;
                }
            }
        }
    }
    //cout<<ARI<<endl;
    EXPECT_EQ(error,0)<< "on "<< PARAM_K*PARAM_D*PARAM_N<< " retrieved integers";

}
/* TEST(TrapdoorTest, Obtain_s_times_G2) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> A = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    PolynomialMatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D> I = TrapdoorHandler::generate_identity_polymatrix<PARAM_K*PARAM_D, PARAM_K*PARAM_D>();
    PolynomialMatrix<PARAM_M, PARAM_K*PARAM_D> RI;

    RI.block(0,0,2*PARAM_D,PARAM_D*PARAM_K) = R;
    RI.block(2*PARAM_D,0,PARAM_D*PARAM_K,PARAM_D*PARAM_K) = I;

    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G = TrapdoorHandler::generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();

    PolynomialMatrix<1, PARAM_D> s=TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();

    PolynomialMatrix<1, PARAM_M> e;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=P_random();
        } 
        e(0,j)=poly;
    }
    //cout<<"TESTTESTTEST"<< e*RI<< endl;
    PolynomialMatrix<1, PARAM_M> b=s*A+e;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat = b * RI;
    PolynomialMatrix<1, PARAM_K*PARAM_D> b_hat2 = s*G+e*RI;

    int error=0;
    for(int j=0; j<PARAM_K*PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            if(b_hat2(0,j)[y]!=b_hat(0,j)[y]){
                cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                error++;
            }
        }
    }
    EXPECT_EQ(error,0)<< "on "<< PARAM_K*PARAM_D*PARAM_N<< " retrieved integers";

} */

TEST(TrapdoorTest, Invert) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> A = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    
    PolynomialMatrix<1, PARAM_D> s=TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();
    //cout << "Secret Key s (R_q^{1 × d}):\n" << s << std::endl;

    PolynomialMatrix<1, PARAM_M> e;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=P_random();
        } 
        e(0,j)=poly;
    }
    cout << "Error e (R_q^{1 × m}):\n" << e << std::endl;

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
    cout << "Error e2 (R_q^{1 × m}):\n" << e2 << "\n\n";

    int error=0;
    for(int j=0; j<PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            if(s2(0,j)[y]!=s(0,j)[y])error++;
            //EXPECT_EQ(s2(0,j)[y],s(0,j)[y]) << "A_hat ha zero righe!";
        }
    }
    EXPECT_EQ(error,0)<< "on "<< PARAM_D*PARAM_N<< " retrieved integers";
    //EXPECT_EQ(1,0)<< "on "<< PARAM_D*PARAM_N<< " retrieved integers";
    error=0;
    for(int j=0; j<PARAM_M; j++){
        for(int y=0; y<PARAM_N; y++){
            if(e(0,j)[y]!=e2(0,j)[y])error++;
            //EXPECT_EQ(s2(0,j)[y],s(0,j)[y]) << "A_hat ha zero righe!";
        }
    }
    EXPECT_EQ(error,0)<< "on "<< PARAM_M*PARAM_N<< " retrieved integers";
} 

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}