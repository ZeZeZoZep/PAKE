#include "../include/lpke.h"    
#include "../include/trapdoor_handler.h"
#include "../include/r.h"
// ✅ Costruttore: inizializza la matrice pubblica `A` e la trapdoor `T`
LPKE::LPKE() {
    auto setup_result = LSetup();
    A = setup_result.first;
    T = setup_result.second;
}

// ✅ Distruttore (se non ci sono risorse da liberare, può essere vuoto)
LPKE::~LPKE() {}

// ✅ LSetup: Genera `A` e `T`
pair<PolynomialMatrix<PARAM_D, PARAM_M>, PolynomialMatrix<2 * PARAM_D, PARAM_D * PARAM_K>> LPKE::LSetup() const {
    return TrapdoorHandler::Trapgen();
}

// ✅ LKeyGen: Genera chiavi pubblica `pk` e segreta `sk`
pair<PolynomialMatrix<1, PARAM_M>, PolynomialMatrix<1, PARAM_D>> LPKE::LKeyGen(PolynomialMatrix<1, PARAM_M>& v) const {
    PolynomialMatrix<1, PARAM_D> s=TrapdoorHandler::generate_uniform_polymatrix<1, PARAM_D>();
    PolynomialMatrix<1, PARAM_M> e;
    for(int j=0; j<PARAM_M; j++){
        Polynomial poly(PARAM_N);
        poly.setZero();
        //poly[0]=P_random();
        for(int y=0; y<PARAM_N; y++){
            poly[y]=gaussian_random(0,0.3);
        } 
        e(0,j)=poly;
    }
    PolynomialMatrix<1, PARAM_M> b = s * this->A + e - v;

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
Cyphertext LPKE::LEnc(PolynomialMatrix<1, PARAM_M>& pk, vector<uint8_t>& m, PolynomialMatrix<1, PARAM_M>& v) const {
    Cyphertext ct;
    PolynomialMatrix<1, PARAM_M> p = PolynomialMatrix<1, PARAM_M>::Zero();
    p = pk + v;
    cout<< "3"<<endl;
    PolynomialMatrix<1, 1> m_poly;
    m_poly(0,0)=bits_times_q_over_2(m,this->q);
    cout<< m_poly(0,0)<<endl;

    cout<< "3"<<endl;

    PolynomialMatrix<1, 1> c; //m.size()*8 = lambda, 256 se m 32 byte

    PolynomialMatrix<PARAM_M, 1> e;


    //uint8_t beta_i=0;
    uint8_t bit=0;
    for(int k=0; k<PARAM_M; k++){

        Polynomial poly(PARAM_N);
        poly.setZero();

        for(int k=0; k<PARAM_N; k++){
            if(k%2==0){
                poly[k]=gaussian_random(0,0.3);
            }
        } 
        e(k,0)=poly;
    }
    
    PolynomialMatrix<1, 1> temp=p*e;
    cout<< "3"<<endl;
    PolynomialMatrix<PARAM_D, 1> u=this->A*e;

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


    PolynomialMatrix<1,1> d = ct.c - ct.u * sk;
    Polynomial temp = d(0,0);
    cout<< temp<<endl;
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
    for(int j=0; j<PARAM_M; j++){
        if(e(j).norm()>this->q/(8*std::sqrt(PARAM_N))){
            return true;
        }
    }
    return false;
}
