// A utility function to add two polynomials
#include <gtest/gtest.h>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/rand.h>
#include "ideal_cipher.h"
#include "int.h"
#include "polynomial_matrix.h"
#include "polynomial_matrix_utils.h"
#include "random.h"
#include "lpke.h"
#include "hash.h"

vector<int> encode_poly(Polynomial p){
    vector<int> ret;
    for(int i=0; i<p.size(); i++){
        ret.push_back(p[i]);
    }
    return ret;
}
Polynomial decode_poly(vector<int> p){
    Polynomial ret(256);
    for(int i=0; i<p.size(); i++){
        ret[i]=p[i]%PARAM_Q;
    }
    cout<<ret<<endl;
    return ret;
}

vector<int> add_poly(const vector<int>& a, const vector<int>& b) {
    size_t n = max(a.size(), b.size());
    vector<int> res(n, 0);
    for (size_t i = 0; i < n; ++i) {
        if (i < a.size()) res[i] += a[i];
        if (i < b.size()) res[i] += b[i];
    }
    return res;
}

// A utility function to subtract polynomial b from a
vector<int> sub_poly(const vector<int>& a, const vector<int>& b) {
    size_t n = max(a.size(), b.size());
    vector<int> res(n, 0);
    for (size_t i = 0; i < n; ++i) {
        res[i] = (i < a.size() ? a[i] : 0) - (i < b.size() ? b[i] : 0);
    }
    return res;
}

// Shift polynomial by k positions (multiply by X^k)
vector<int> shift_poly(const vector<int>& a, size_t k) {
    vector<int> res(k, 0);
    res.insert(res.end(), a.begin(), a.end());
    return res;
}

// Karatsuba multiplication
vector<int> karatsuba(vector<int> a, vector<int> b) {
/*     size_t n = max(a.size(), b.size());
    if (n <= 2) { // base case: schoolbook
        vector<int> res(a.size() + b.size() - 1, 0);
        for (size_t i = 0; i < a.size(); ++i) {
            for (size_t j = 0; j < b.size(); ++j) {
                res[i + j] += a[i] * b[j];
            }
        }
        return res;
    }
    size_t k = n / 2;

    vector<int> a_low(a.begin(), a.begin() + min(k, a.size()));
    vector<int> a_high(a.begin() + min(k, a.size()), a.end());
    vector<int> b_low(b.begin(), b.begin() + min(k, b.size()));
    vector<int> b_high(b.begin() + min(k, b.size()), b.end());

    vector<int> p0 = karatsuba(a_low, b_low);
    vector<int> p2 = karatsuba(a_high, b_high);
    vector<int> sum_a = add_poly(a_low, a_high);
    vector<int> sum_b = add_poly(b_low, b_high);
    vector<int> p1 = karatsuba(sum_a, sum_b);
    p1 = sub_poly(sub_poly(p1, p0), p2);

    vector<int> result = add_poly(shift_poly(p2, 2 * k), shift_poly(p1, k));
    result = add_poly(result, p0); */
    vector<int> result(256);
    for(int i=0; i<b.size(); i++){
        a[i] = (a[i] + PARAM_Q) % PARAM_Q;
        b[i] = (b[i] + PARAM_Q) % PARAM_Q;
        result[i]=b[i]*a[i];
        result[i] = (result[i] + PARAM_Q) % PARAM_Q;

    }

    return result;
}
Polynomial bits_times_q_over_20(const std::vector<uint8_t>& message, uint32_t q) {
    
    Polynomial result(message.size() * 8);

    for (int j = 0; j < message.size(); j++) {
        uint8_t byte = message[j];
        for (int i = 7; i >= 0; --i) { // Dalla MSB alla LSB
            uint8_t bit = (byte >> i) & 1;
            result(i + j*8)=bit * (q / 2);
        }
    }

    return result;
}

PolynomialMatrix<1, 1> m_poly;
PolynomialMatrix<PARAM_M, 1> A1;
PolynomialMatrix<PARAM_M,1> A2;
PolynomialMatrix<PARAM_D, 1> u;
LPKE lpke;
PolynomialMatrix<1, PARAM_M> v;
PolynomialMatrix<1, PARAM_M> pk;
PolynomialMatrix<1, PARAM_D> sk;
PolynomialMatrix<1, PARAM_M> e;
PolynomialMatrix<1, 1> c;
vector<uint8_t> m={1,2,3,4};
PolynomialMatrix<1, PARAM_M> p;
Cyphertext ct;
TEST(KaratsubaTest, Multiplication_odd) {
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
    sk=TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();

    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=gaussian_random(0,0.3);
        } 
        e(0,j)=poly;
    }
    pk= sk * lpke.A + e - v;

    m=H(m);
    p = PolynomialMatrix<1, PARAM_M>::Zero();
    p = pk + v;


    m_poly(0,0)=bits_times_q_over_20(m,PARAM_Q);

    for(int j=0; j<PARAM_M; j++){
        Polynomial poly1(PARAM_N),poly2(PARAM_N);
        poly1.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly1[y]=gaussian_random(0,0.3);
            if(poly1[y]<0)poly1[y]=poly1[y]*(-1);
        } 
        //A1(j,0)=poly1;
        A1(j,0)=SamplePolyCBD(PRF(PARAM_ETA1, m, j), PARAM_ETA1);
    }
    

    PolynomialMatrix<1, 1> temp=p*A1;
    u=lpke.A*A1;

    c = temp + m_poly;

    PolynomialMatrix<1,1> prod=c - sk * u - m_poly;
    EXPECT_EQ(sk * u ,temp-e*A1);
    EXPECT_EQ(temp-sk * u,e*A1);

    //cout<<prod<<endl;
    for(int y=0; y<PARAM_N; y++){ 
        if(prod(0,0)[y]>PARAM_Q/2 )prod(0,0)[y]=(prod(0,0)[y]-PARAM_Q)*-1;
    }  
/*     for(int y=0; y<PARAM_N; y+=2){ 
        prod(0,0)[y]-=PARAM_Q/8;
    } */
    int errors=0;
    for(int y=1; y<PARAM_N; y+=2){ 
        prod(0,0)[y]>=PARAM_Q/4?errors++:0;
    }
    cout<<prod<<endl;
    EXPECT_GT(errors,256);
        //EXPECT_EQ(errors,0);
    ct.u=u;
    ct.c=c;
    vector<uint8_t> m2 = lpke.LDec(sk,ct);
    EXPECT_NE(m,m2) << "they should be equal";
}
 TEST(KaratsubaTest, Multiplication_even) {
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly2(PARAM_N);
        poly2.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            if(y%2==0)poly2[y]=gaussian_random(0,4);
        } 

        A1(j,0)=poly2;
    }

    PolynomialMatrix<1, 1> temp=p*A1;
    u=lpke.A*A1;

    c = temp + m_poly;

    PolynomialMatrix<1,1> prod=c - sk * u - m_poly;
    EXPECT_EQ(sk * u,temp-e*A1);
    int errors=0;
    for(int y=0; y<PARAM_N; y++){ 
        if(prod(0,0)[y]>PARAM_Q/2 )prod(0,0)[y]=(prod(0,0)[y]-PARAM_Q)*-1;
    } 
/*     for(int y=0; y<PARAM_N; y++){ 
        if(prod(0,0)[y]>PARAM_Q/4 prod(0,0)[y]>)?errors++:0;
    } 
    */
    EXPECT_EQ(errors,1); 
    cout<<prod<<endl;
    ct.u=u;
    ct.c=c;
    vector<uint8_t> m2 = lpke.LDec(sk,ct);
    EXPECT_EQ(m,m2) << "they should be equal";

} 

TEST(KaratsubaTest, Multiplication_karatsuba) {
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly2(PARAM_N);
        poly2.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly2[y]=gaussian_random(0,2);
        } 

        A1(j,0)=poly2;
    }

    PolynomialMatrix<1, 1> temp;


    vector<int>result(256);
    for (int j = 0; j < PARAM_M; ++j) {
            //cout<<karatsuba(encode_poly(p(0,j)),encode_poly(A1(j,0))).size()<<endl;
            result = add_poly( result, karatsuba(encode_poly(p(0,j)),encode_poly(A1(j,0)))); 
    }
    cout<<result.size()<<endl;
    temp(0,0)=decode_poly(result);
    for(int y=0; y<PARAM_D; y++){ 
        vector<int> result3(256);
        for (int j = 0; j < PARAM_M; ++j) {
                result3 = add_poly( result3, karatsuba(encode_poly(lpke.A(y,j)),encode_poly(A1(j,0)))); 
        }
        cout<<result3.size()<<endl;
        u(y,0)=decode_poly(result3);
    }
    
    //PolynomialMatrix<1, 1> temp=p*A1;
    //u=lpke.A*A1;
    
    c = temp + m_poly;
    vector<int>result2(256,0);
    for (int j = 0; j < PARAM_D; ++j) {
        cout<<"ciao"<<endl;
        result2 = add_poly( result2, karatsuba(encode_poly(sk(0,j)),encode_poly(u(j,0)))); 
    }
    PolynomialMatrix<1, 1> temp2;
    temp2(0,0)=decode_poly(result2);

    PolynomialMatrix<1,1> prod=c - m_poly - temp2;
    cout<<prod<<endl;
    //EXPECT_EQ(sk * u,temp-e*A1);
    int errors=0;
/*     for(int y=0; y<PARAM_N; y++){ 
        if(prod(0,0)[y]>PARAM_Q/2)prod(0,0)[y]=(prod(0,0)[y]-PARAM_Q)*-1;
    }  */
/*     for(int y=0; y<PARAM_N; y++){ 
        if(prod(0,0)[y]>PARAM_Q/4 prod(0,0)[y]>)?errors++:0;
    } 
    */
    EXPECT_EQ(errors,1); 
    //cout<<prod<<endl;
    //ct.u=u;
    //ct.c=c;
    //vector<uint8_t> m2 = lpke.LDec(sk,ct);
    //EXPECT_EQ(m,m2) << "they should be equal";

} 
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}