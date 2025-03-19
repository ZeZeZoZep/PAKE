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
    PolynomialMatrix<1, PARAM_M> p = PolynomialMatrix<1, PARAM_M>::Zero();
    p = pk + v;
    vector<uint8_t> beta(m.size());
    vector<PolynomialMatrix<PARAM_D, 1>> c(m.size()*m.size());
    PolynomialMatrix<PARAM_M, 1> e_ij;

    for(int i=0; i<m.size()*8; i++){
        uint8_t beta_i=0;
        uint8_t bit=0;
        for(int j=0; j<m.size()*8; j++){
            for(int k=0; k<PARAM_M; k++){

                Polynomial poly(PARAM_N);
                poly.setZero();

                for(int k=0; k<PARAM_N; k++){
                    poly[k]=P_random();
                } 
                e_ij(k,0)=poly;
            }
            PolynomialMatrix<PARAM_D, 1> c_ij=this->A*e_ij;

            PolynomialMatrix<1, 1> temp=p*e_ij;
            bit = R(static_cast<double>(temp[0].sum()), PARAM_N*this->q, pow(PARAM_N*this->q,2/3));

            c.push_back(c_ij);
            beta_i ^= bit < (j % m.size());
        }
        beta[i]=beta_i;
    }
    ct.c=c;
    ct.beta = beta; // Conserva il messaggio per riferimento 

    return ct;
}

// ✅ LDec: Decifra un `Cyphertext` con la chiave segreta `sk`
vector<uint8_t> LPKE::LDec(PolynomialMatrix<1, PARAM_D>& sk, Cyphertext& ct) const {
    vector<uint8_t> m(ct.beta.size());

    for(int i=0; i<ct.beta.size()*8; i++){
        uint8_t m_i=0;
        uint8_t bit=0;
        for(int j=0; j<ct.beta.size()*8; j++){
            PolynomialMatrix<1, 1> temp=sk*ct.c[i*ct.c.size()+j];
            bit = R(static_cast<double>(temp[0].sum()), PARAM_N*this->q, pow(PARAM_N*this->q,2/3));
            m_i ^= bit < (j % m.size());
        }
        m[i]=m_i;
    }
    return m;
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
