#ifndef FEISTEL_H
#define FEISTEL_H
#include <vector>
#include <cstdint>


#include <openssl/evp.h>
#include <openssl/rand.h>
#include <boost/multiprecision/cpp_int.hpp>
#include "int.h"
#include "ntt.h"
#include "common.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"

#include "timerset.h"
using namespace std;
using boost::multiprecision::cpp_int;

class Feistel {
private:
    vector<uint8_t> key;

    void shake256(const vector<uint8_t>& input, vector<uint8_t>& output) {
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_shake256(), NULL);
        EVP_DigestUpdate(ctx, input.data(), input.size());
        EVP_DigestFinalXOF(ctx, output.data(), output.size());
        EVP_MD_CTX_free(ctx);
    }

    template<int Cols>
    PolynomialMatrix<1, Cols> H1(vector<uint8_t> r){
        vector<uint16_t> input_16, res_16, compressed;
        vector<uint8_t> input = concatVectors(r,this->key);
        vector<uint8_t> output(384*Cols);

        shake256(input, output);
        for(int i=0;i<Cols;i++){
            input.insert(input.begin(), output.begin()+i*384,output.begin()+(i+1)*384 );
            input_16=ByteDecode(input, 12, 4096);
            compressed = Compress(input_16,12,PARAM_Q);
            res_16.insert(res_16.end(), compressed.begin(),compressed.end());
        }
        return PolynomialMatrixUtils::Decode<1, Cols>(res_16);
    }

    template<int Cols>
    vector<uint8_t> H2(PolynomialMatrix<1, Cols> T){
        vector<uint16_t> compressed;
        vector<uint16_t> input_16 = PolynomialMatrixUtils::Encode<1, Cols>(T);
        compressed = Compress(input_16,12,PARAM_Q);

        vector<uint8_t> tot_encode;

        for(int i=0;i<Cols;i++){
            vector<uint16_t> temp(compressed.begin() + i * PARAM_N, compressed.begin() + (i + 1) * PARAM_N);
            vector<uint8_t> encode = ByteEncode(temp, 12, PARAM_Q);
            tot_encode.insert(tot_encode.end(), encode.begin(),encode.end());
        }
        vector<uint8_t> input = concatVectors(tot_encode,this->key);
        vector<uint8_t> output(96);
        shake256(input, output);
        return output;
    }
    vector<uint8_t> XOR(vector<uint8_t> a, vector<uint8_t> b){
        vector<uint8_t> res;
            if (a.size() != b.size()) {
        throw std::invalid_argument("I vettori devono avere la stessa dimensione per fare XOR.");
        }

        std::vector<uint8_t> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] ^ b[i];
        }

        return result;
    }

public:
    Feistel(vector<uint8_t> key){
        this->key=key;
    } 
    ~Feistel(){
        key.clear();
    }

    template<int Cols>
    pair<vector<uint8_t>, PolynomialMatrix<1, Cols>> encrypt(vector<uint8_t> r, PolynomialMatrix<1,Cols> M){
        TimerSet ts("2Feistel", "Encrypt");
        ts.start("Total");
        PolynomialMatrix<1, Cols> R = H1<Cols>(r);
        PolynomialMatrix<1, Cols> T = M + R; //group operation
        vector<uint8_t> t = H2<Cols>(T);
        vector<uint8_t> s = XOR(r,t);
        ts.stop("Total");
        ts.commit(PARAM_D);
        return {s, T};


    }

    template<int Cols>
    pair<vector<uint8_t>, PolynomialMatrix<1, Cols>> decrypt(vector<uint8_t> s, PolynomialMatrix<1,Cols> T){
        TimerSet ts("2Feistel", "Decrypt");
        ts.start("Total");
        vector<uint8_t> t = H2<Cols>(T);
        vector<uint8_t> r = XOR(s,t);
        PolynomialMatrix<1, Cols> R = H1<Cols>(r);
        PolynomialMatrix<1, Cols> M = T - R; //group operation
        ts.stop("Total");
        ts.commit(PARAM_D);
        return {r, M};
    }
    
};

#endif