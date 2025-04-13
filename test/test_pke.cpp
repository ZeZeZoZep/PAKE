#include <gtest/gtest.h>
#include "../include/pke.h"
#include "../include/random.h"
#include <openssl/rand.h>
PolynomialMatrix<1, PARAM_D> pk;
PolynomialMatrix<1, PARAM_D> sk;
PKE pke;
vector<uint8_t> rho;
std::vector<uint8_t> seed(32);


TEST(PKETest, Keygen) {
    if (RAND_bytes(seed.data(), seed.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> ret = pke.KeyGen(seed);

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

TEST(PKETest, Enc) {

    std::vector<uint8_t> result(256);
    
    std::random_device rd;  // generatore di entropia hardware-based (se disponibile)
    std::mt19937 gen(rd()); // Mersenne Twister
    std::uniform_int_distribution<> dis(0, 255);

    for (std::size_t i = 0; i <256; ++i) {
        result[i] = static_cast<uint8_t>(dis(gen));
    }
    auto ret = pke.Encrypt(pk,rho,result,result);
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
    EXPECT_EQ(true,true)<< "on "<< PARAM_D*PARAM_N<< " retrieved integers for sk";

}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}