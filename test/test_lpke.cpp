#include <gtest/gtest.h>
#include "../include/lpke.h"
#include "../include/random.h"
TEST(LPKETest, Keygen) {
    LPKE lpke;
    PolynomialMatrix<1, PARAM_M> v;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=1;
            //if(poly[y]<0)poly[y]=poly[y]*(-1);
        } 
        v(0,j)=poly;
    }
    auto ret = lpke.LKeyGen(v);
    PolynomialMatrix<1, PARAM_M> pk=ret.first;
    PolynomialMatrix<1, PARAM_D> sk=ret.second;
    int error=0;
    for(int j=0; j<PARAM_M; j++){
        for(int y=0; y<PARAM_N; y++){
            if(pk(0,j)[y]==0){
                //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                error++;
            }
        }
    }
    EXPECT_LT(error,10)<< "on "<< PARAM_M*PARAM_N<< " retrieved integers for pk";
    error=0;
    for(int j=0; j<PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            if(sk(0,j)[y]==0){
                //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                error++;
            }
        }
    }
    EXPECT_LT(error,10)<< "on "<< PARAM_M*PARAM_N<< " retrieved integers for sk";

}
TEST(LPKETest, IsLossy) {
    LPKE lpke;
    PolynomialMatrix<1, PARAM_M> v;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=1;
            //if(poly[y]<0)poly[y]=poly[y]*(-1);
        } 
        v(0,j)=poly;
    }
    auto ret = lpke.LKeyGen(v);
    PolynomialMatrix<1, PARAM_M> pk=ret.first;
    PolynomialMatrix<1, PARAM_D> sk=ret.second;


    EXPECT_FALSE(lpke.IsLossy(lpke.T, pk, v)) << "It should NOT be lossy";
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=3;
            //if(poly[y]<0)poly[y]=poly[y]*(-1);
        } 
        v(0,j)=poly;
    }
    EXPECT_TRUE(lpke.IsLossy(lpke.T, pk, v)) << "It should be lossy";
}
TEST(LPKETest, IsLossyRandom) {
    LPKE lpke;
    PolynomialMatrix<1, PARAM_M> v = TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_M>();;

    auto ret = lpke.LKeyGen(v);
    PolynomialMatrix<1, PARAM_M> pk=ret.first;
    PolynomialMatrix<1, PARAM_D> sk=ret.second;
    

    EXPECT_FALSE(lpke.IsLossy(lpke.T, pk, v)) << "It should NOT be lossy";
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=uniform_q_random(PARAM_Q);
            //if(poly[y]<0)poly[y]=poly[y]*(-1);
        } 
        v(0,j)=poly;
    }
    EXPECT_TRUE(lpke.IsLossy(lpke.T, pk, v)) << "It should be lossy";
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}