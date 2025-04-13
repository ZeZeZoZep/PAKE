#include <gtest/gtest.h>
#include "../include/kem.h"
#include "../include/random.h"
#include <openssl/rand.h>
PolynomialMatrix<1, PARAM_D> pk;
PolynomialMatrix<1, PARAM_D> sk;
KEM kem;


vector<uint8_t> z;
vector<uint8_t> rho;
std::vector<uint8_t> seed(32);


TEST(KEMTest, Keygen) {
    auto ret_keygen = kem.KeyGen();
    auto ret = ret_keygen.first;
    z = ret_keygen.second;
    
    pk=get<0>(ret);
    sk=get<1>(ret);
    rho=get<2>(ret);
     

    int error=0;
    for(int j=0; j<PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            if(pk(0,j)[y]==0){
                //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                error++;
            }
        }
    }
    EXPECT_LT(error,PARAM_D*PARAM_N)<< "on "<< PARAM_D*PARAM_N<< " retrieved integers for pk";

    error=0;
    for(int j=0; j<PARAM_D; j++){
        for(int y=0; y<PARAM_N; y++){
            if(sk(0,j)[y]==0){
                //cout<<b_hat(0,j)[y]<<" "<<b_hat2(0,j)[y]<<endl;
                error++;
            }
        }
    }
    EXPECT_LT(error,PARAM_D*PARAM_N)<< "on "<< PARAM_D*PARAM_N<< " retrieved integers for sk";

}

TEST(KEMTest, EncDec) {

    auto retE = kem.Encaps(pk,rho);

    auto retD = kem.Decaps(sk,pk,rho,z,retE.second.first,retE.second.second);

    int error=0;
    for(int i=0; i<32; i++){
        if(retE.first[i]!=retD[i])error++;
    }
    EXPECT_EQ(error,0)<< "they should be equal";

}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}