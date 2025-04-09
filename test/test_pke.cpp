#include <gtest/gtest.h>
#include "../include/pke.h"
#include "../include/random.h"
#include <openssl/rand.h>
TEST(PKETest, Keygen) {
    PKE pke;

    std::vector<uint8_t> seed(32);
    if (RAND_bytes(seed.data(), seed.size()) != 1) throw std::runtime_error("RAND_bytes failed");

    tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> ret = pke.KeyGen(seed);
    PolynomialMatrix<1, PARAM_D> pk=get<0>(ret);
    PolynomialMatrix<1, PARAM_D> sk=get<1>(ret);

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}