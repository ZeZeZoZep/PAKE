#include <gtest/gtest.h>
#include "../include/lpke.h"
#include "../include/random.h"
#include "../include/hash.h"

LPKE lpke;
PolynomialMatrix<1, PARAM_M> v;
PolynomialMatrix<1, PARAM_M> pk;
PolynomialMatrix<1, PARAM_D> sk;
TEST(LPKETest, Keygen) {


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
    pk=ret.first;
    sk=ret.second;
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

    v = TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_M>();;

    auto ret = lpke.LKeyGen(v);
    pk=ret.first;
    sk=ret.second;
    

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
TEST(LPKETest, LEnc_and_LDec) {
    //v = TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_M>();;
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
    pk=ret.first;
    sk=ret.second;
    cout << PARAM_M << " " << endl;
    vector<uint8_t> m={1,2,3,4};
    m=H(m);
    
    
    Cyphertext ct = lpke.LEnc(pk,m,v);
    cout << PARAM_M << " prcodoooo" << endl;
    vector<uint8_t> m2 = lpke.LDec(sk,ct);

 
    int error=0;
    for(int y=0; y<32; y++){
        if(m[y]!=m2[y]){
            //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
            error++;
        }
    }
    EXPECT_EQ(error,0)<< "on "<< 32<< " retrieved integers";
    EXPECT_EQ(m,m2) << "they should be equal";
    
} 

TEST(LPKETest, LEnc_and_LDec_lossy) {
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=3;//uniform_q_random(PARAM_Q);
            //if(poly[y]<0)poly[y]=poly[y]*(-1);
        } 
        v(0,j)=poly;
    }
    vector<uint8_t> m={1,2,3,4};
    m=H(m);
    
    
    Cyphertext ct = lpke.LEnc(pk,m,v);

    vector<uint8_t> m2 = lpke.LDec(sk,ct);

    EXPECT_NE(m,m2) << "they should not be equal";
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}