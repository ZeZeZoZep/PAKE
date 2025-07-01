#include "../include/lpke.h"    
#include "../include/trapdoor_handler.h"
#include "../include/r.h"
// ✅ Costruttore: inizializza la matrice pubblica `A` e la trapdoor `T`
LPKE::LPKE() {
    auto setup_result = LSetup();
    A = setup_result.first;
    T = setup_result.second;
}
#include <iostream>
#include <vector>

using namespace std;



// ✅ Distruttore (se non ci sono risorse da liberare, può essere vuoto)
LPKE::~LPKE() {}

// ✅ LSetup: Genera `A` e `T`
pair<PolynomialMatrix<PARAM_D, PARAM_M>, PolynomialMatrix<2 * PARAM_D, PARAM_D * PARAM_K>> LPKE::LSetup() const {
    return TrapdoorHandler::Trapgen();
}

// ✅ LKeyGen: Genera chiavi pubblica `pk` e segreta `sk`
pair<PolynomialMatrix<1, PARAM_M>, PolynomialMatrix<1, PARAM_D>> LPKE::LKeyGen(PolynomialMatrix<1, PARAM_M>& v,vector<uint8_t> seed) const {
    //PolynomialMatrix<1, PARAM_D> s=TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();
    PolynomialMatrix<1, PARAM_D> s;
    uint8_t bit=0;
    if(seed.size()!=32){
        seed.resize(32);
        if (RAND_bytes(seed.data(), seed.size()) != 1) throw std::runtime_error("RAND_bytes failed");
        cout<< "Generating random seed"<<endl;
    }
    uint8_t N = 0;
    for(int i=0; i<PARAM_D; i++){
        s(0,i)=SamplePolyCBD(PRF(15, seed, N), 15);
        N++;
        for(int k=0; k<PARAM_N; k++) s(0,i)[k]>1664 ? s(0,i)[k]=-s(0,i)[k]+PARAM_Q : 0;
    }
    PolynomialMatrix<1, PARAM_M> e;
    for(int i=0; i<PARAM_M; i++){
        e(0,i)=SamplePolyCBD(PRF(2, seed, N), 2);
        N++;
    }
/*     PolynomialMatrix<1, PARAM_M> e;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=gaussian_random(0,0.3);
        } 
        e(0,j)=poly;
    } */
   
    PolynomialMatrix<1, PARAM_M> b = s * this->A + e - v;
    cout<< "pene pene"<<endl;
    return {b,s};
}
Polynomial bits_times_q_over_2(const std::vector<uint8_t>& message, uint16_t q) {
    
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
// ✅ LEnc: Cifra un messaggio `m` con la chiave pubblica `pk`
Cyphertext LPKE::LEnc(PolynomialMatrix<1, PARAM_M>& pk, vector<uint8_t>& m, PolynomialMatrix<1, PARAM_M>& v, vector<uint8_t> seed) const {
    Cyphertext ct;
    PolynomialMatrix<1, PARAM_M> p = PolynomialMatrix<1, PARAM_M>::Zero();
    p = pk + v;

    PolynomialMatrix<1, 1> m_poly;
    m_poly(0,0)=bits_times_q_over_2(m,this->q);
    //cout<< m_poly(0,0)<<endl;


    PolynomialMatrix<1, 1> c; //m.size()*8 = lambda, 256 se m 32 byte

    PolynomialMatrix<PARAM_M, 1> y;
    
    //uint8_t beta_i=0;
    uint8_t bit=0;
    if(seed.size()!=32){
        seed.resize(32);
        if (RAND_bytes(seed.data(), seed.size()) != 1) throw std::runtime_error("RAND_bytes failed");
        cout<< "Generating random seed"<<endl;
    }
    uint8_t N = 0;

    for(int k=0; k<PARAM_M; k++){
        y(k,0)=SamplePolyCBD(PRF(15, seed, N++),15);//poly;
        for(int i=0; i<PARAM_N; i++) y(k,0)[i]>1664 ? y(k,0)[i]=-y(k,0)[i]+PARAM_Q : 0;
    }

    PolynomialMatrix<PARAM_D, 1> e1;
    for(int i=0; i<PARAM_D; i++){
        e1(i,0)=SamplePolyCBD(PRF(2, seed, N), 2);
        N++;
    }

    PolynomialMatrix<1, 1> e2;
    e2(0,0)=SamplePolyCBD(PRF(2, seed, N), 2);

    
    cout<< "1"<<endl;
    PolynomialMatrix<1, 1> temp=p*y+e2;
    cout<< "2"<<endl;
    PolynomialMatrix<PARAM_D, 1> u=this->A*y+e1;
    cout<< "3"<<endl;
    c = temp + m_poly;

    ct.u=u;
    ct.c=c;
    return ct;
}

std::vector<uint8_t> decode_to_bytes(Polynomial d, uint16_t q) {
    if (d.size() != 256)
        throw std::invalid_argument("Il vettore d deve contenere 256 elementi.");

    std::vector<uint8_t> out(32, 0);  // 256 bit → 32 byte
    uint16_t lower = q / 4;
    uint16_t upper = (3 * q) / 4;

    for (int i = 0; i < 256; ++i) {
        uint8_t bit = (d(i) >= lower && d(i) <= upper) ? 1 : 0;
        out[i / 8] |= (bit << (i % 8));  // LSB first
    }

    return out;
}
// ✅ LDec: Decifra un `Cyphertext` con la chiave segreta `sk`
vector<uint8_t> LPKE::LDec(PolynomialMatrix<1, PARAM_D>& sk, Cyphertext& ct) const {


    PolynomialMatrix<1,1> d = ct.c - sk * ct.u;
    Polynomial temp = d(0,0);
    //cout<< temp<<endl;
    return decode_to_bytes(temp,this->q);
}
double LPKE::compute_norm(PolynomialMatrix<1, PARAM_M>& e){
    double norm=0;
    for(int j=0; j<PARAM_M; j++){
        norm+=e(j).norm();
    }
    cout<<"norm "<< norm<<std::endl;
    return norm;

}

// ✅ IsLossy: Controlla se `pk` è generato da una `T` lossy
bool LPKE::IsLossy(PolynomialMatrix<2 * PARAM_D, PARAM_D * PARAM_K>& T, PolynomialMatrix<1, PARAM_M>& pk, PolynomialMatrix<1, PARAM_M>& v) const {
    PolynomialMatrix<1, PARAM_M> p = PolynomialMatrix<1, PARAM_M>::Zero();

    p = pk + v;

    auto ret = TrapdoorHandler::Invert(p,T);
    PolynomialMatrix<1, PARAM_D> s = ret.first;
    PolynomialMatrix<1, PARAM_M> e = p-s*A;
    for(int j=0; j<PARAM_M; j++){
        for(int y=0; y<PARAM_N; y++){
            if(e(0,j)[y]>PARAM_Q/2)e(0,j)[y]=e(0,j)[y]-PARAM_Q;
        }
    }
    //cout<< e <<std::endl;
    int1024_t norm=0;
    for(int j=0; j<PARAM_M; j++){
        norm+=e(j).norm();
    } 
    if(norm > static_cast<int1024_t>(this->q*PARAM_N/(8*std::sqrt(PARAM_M)))) return true;
    return false;
}
