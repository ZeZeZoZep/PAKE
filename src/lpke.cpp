#include "../include/lpke.h"    
#include "../include/trapdoor_handler.h"
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
            poly[y]=P_random();
        } 
        e(0,j)=poly;
    }
    PolynomialMatrix<1, PARAM_M> b = s * this->A + e - v;

    return {b,s};
}

// ✅ LEnc: Cifra un messaggio `m` con la chiave pubblica `pk`
Cyphertext LPKE::LEnc(PolynomialMatrix<1, PARAM_M>& pk, vector<uint8_t>& m, PolynomialMatrix<1, PARAM_M>& v) const {
    Cyphertext ct;

/*     // Genera `s` casuale
    PolynomialMatrix<1, PARAM_M> s;
    for (int i = 0; i < PARAM_M; ++i) {
        //s(0, i) = SampleNTT();
    }

    // Cifra `c0 = s * A + v`
    //ct.c.push_back(s * A + v);

    // Cifra `c1 = s * pk + Encode(m)`
    PolynomialMatrix<1, PARAM_M> encoded_m;
    for (size_t i = 0; i < m.size(); ++i) {
        encoded_m(0, i) = Polynomial(PARAM_N);
        encoded_m(0, i)[0] = m[i]; // Codifica semplice (da migliorare)
    }

    ct.c.push_back(s * pk + encoded_m);
    ct.beta = m; // Conserva il messaggio per riferimento */

    return ct;
}

// ✅ LDec: Decifra un `Cyphertext` con la chiave segreta `sk`
Cyphertext LPKE::LDec(PolynomialMatrix<1, PARAM_D>& sk, Cyphertext& ct) const {
    Cyphertext decrypted_ct;
/* 
    // Decifra `m' = c1 - sk * c0`
    PolynomialMatrix<1, PARAM_M> decrypted_msg = ct.c[1] - (sk * ct.c[0]);

    // Estrai il messaggio (convertilo in uint8_t)
    for (int i = 0; i < PARAM_M; ++i) {
        decrypted_ct.beta.push_back(decrypted_msg(0, i)[0]);
    }
 */
    return decrypted_ct;
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
        if(e(j).norm()>this->q/(8*sqrt(PARAM_N))){
            return true;
        }
    }
    return false;
}
