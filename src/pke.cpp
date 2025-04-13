#include "pke.h"
PKE::PKE() {
}

// ✅ Distruttore (se non ci sono risorse da liberare, può essere vuoto)
PKE::~PKE() {}

tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> PKE::KeyGen(const vector<uint8_t>& seed) {
    vector<uint8_t> rho(seed.begin(), seed.begin() + 32);
    vector<uint8_t> sigma(seed.begin() + 32, seed.end());

    uint8_t N = 0;

    PolynomialMatrix<PARAM_D, PARAM_D> A;
    for(int i=0; i<PARAM_D; i++){
        for(int j=0; j<PARAM_D; j++){
            A(i,j)=SampleNTT(rho, j, i);
        }
    }
    PolynomialMatrix<1, PARAM_D> s;
    for(int i=0; i<PARAM_D; i++){
        s(0,i)=SamplePolyCBD(PRF(PARAM_ETA1, rho, N), PARAM_ETA1);
        N++;
    }
    PolynomialMatrix<1, PARAM_D> e;
    for(int i=0; i<PARAM_D; i++){
        e(0,i)=SamplePolyCBD(PRF(PARAM_ETA1, rho, N), PARAM_ETA1);
        N++;
    }
    s.toNTT();
    //e.toNTT();

    PolynomialMatrix<1, PARAM_D> t = s*A + e; // t = A·s + e ∈ R_q^k
    // TODO: add encoding of public and secret key
    return {t,s,rho};
}
pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, 1>> PKE::Encrypt(PolynomialMatrix<1, PARAM_D>t,const vector<uint8_t>& rho,const vector<uint8_t>& m, const vector<uint8_t>& r) {
    // TODO: add decoding of public and secret key
    uint8_t N = 0;

    PolynomialMatrix<PARAM_D, PARAM_D> A;
    for(int i=0; i<PARAM_D; i++){
        for(int j=0; j<PARAM_D; j++){
            A(i,j)=SampleNTT(rho, j, i);
        }
    }
    PolynomialMatrix<1, PARAM_D> y;
    for(int i=0; i<PARAM_D; i++){
        y(0,i)=SamplePolyCBD(PRF(PARAM_ETA1, rho, N), PARAM_ETA1);
        N++;
    }
    PolynomialMatrix<1, PARAM_D> e1;
    for(int i=0; i<PARAM_D; i++){
        e1(0,i)=SamplePolyCBD(PRF(PARAM_ETA2, rho, N), PARAM_ETA2);
        N++;
    }
    PolynomialMatrix<1, 1> e2;
    e2(0,0) = SamplePolyCBD(PRF(PARAM_ETA2, rho, N), PARAM_ETA2);
    y.toNTT();
    PolynomialMatrix<1, PARAM_D> u = y*A  + e1;

    vector<uint16_t> temp = Decompress(ByteDecode(m,PARAM_D,PARAM_Q),PARAM_D,PARAM_Q);
    PolynomialMatrix<PARAM_D,1> y_transposed;
    for(int i=0; i<PARAM_D; i++)y_transposed(i,0)=y(0,i); 
    PolynomialMatrix<1,1> mu;
    Polynomial poly;
    
    for(int i=0; i<PARAM_N; i++)poly[i]=temp[i];
    mu(0,0)=poly;
    PolynomialMatrix<1, 1> v = t*y_transposed + e2 + mu;

/* 
    vector<uint16_t> u_encoded;   
    vector<uint16_t> v_encoded;
    for(int i=0; i<PARAM_N; i++){
        u_encoded[i]=v
        v_encoded[i]=
    }
 */
    
    return {u,v};
}
pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>> PKE::Decrypt(PolynomialMatrix<1, PARAM_D> u, PolynomialMatrix<1, PARAM_D> v,PolynomialMatrix<1, PARAM_D> s) {
    // TODO: add decoding of public and secret key
 
    //vector<uint16_t> temp = Decompress(ByteDecode(m,PARAM_D,PARAM_Q),PARAM_D,PARAM_Q);

    PolynomialMatrix<1,1> mu;
    Polynomial poly;
    
   /*  for(int i=0; i<PARAM_N; i++)poly[i]=temp[i];
    mu(0,0)=poly;
    PolynomialMatrix<PARAM_D,1> v_trasn
    PolynomialMatrix<1, PARAM_D> v = u*y + e2 + mu; */

/* 
    vector<uint16_t> u_encoded;   
    vector<uint16_t> v_encoded;
    for(int i=0; i<PARAM_N; i++){
        u_encoded[i]=v
        v_encoded[i]=
    }
 */
    
    return {u,v};
}