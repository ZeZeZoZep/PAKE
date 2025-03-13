#include "../include/polynomial.h"
#include "../include/ntt.h"

#include <iostream>

// Classe che estende VectorXi
class Polynomial : public Eigen::VectorXi {
private:
    int q= PARAM_Q;

public:
    // Usa i costruttori della classe base
    using Eigen::VectorXi::VectorXi;
    bool isNTT=false;
    

    // Implementazione di toNTT()
    Polynomial Polynomial::toNTT() const {
        if(!this->isNTT){
            Polynomial ret=ntt(*this);
            ret.isNTT=true;
            return ret;
        }else{
            return *this;
        }
    }

    // Implementazione di fromNTT()
    Polynomial Polynomial::fromNTT() const {
        if(this->isNTT){
            Polynomial ret=inv_ntt(*this);
            ret.isNTT=false;
            return ret;
        }else{
            return *this;
        }
    }
    // Operatore di somma tra due polinomi con riduzione modulo `mod`
    Polynomial operator+(const Polynomial& other) const {
        if (this->size() != other.size()) {
            throw std::invalid_argument("I polinomi devono avere la stessa dimensione per essere sommati!");
        }
        Polynomial result(this->size());
        for (int i = 0; i < this->size(); i++) {
            result[i] = (((*this)[i] + other[i]) % this->q + this->q) % this->q;
        }
        return result;
    }
    // Operatore di differenza tra due polinomi con riduzione modulo `mod`
    Polynomial operator-(const Polynomial& other) const {
        if (this->size() != other.size()) {
            throw std::invalid_argument("I polinomi devono avere la stessa dimensione per essere sommati!");
        }
        Polynomial result(this->size());
        for (int i = 0; i < this->size(); i++) {
            result[i] = (((*this)[i] - other[i]) % this->q + this->q) % this->q;
        }
        return result;
    }
    // Sovrascrive l'operatore * per usare la moltiplicazione in NTT tra polinomi
    Polynomial operator*(const Polynomial& other) const {
        if (this->size() != other.size()) {
            throw std::invalid_argument("I polinomi devono avere la stessa dimensione per la moltiplicazione NTT!");
        }
        
        // Esegui la moltiplicazione NTT
        Polynomial result(this->size());
        result = multiplyNTTs(*this, other);
        result.isNTT=true;
        return result;
    }
    // Sovrascrive l'operatore % per il modulo elemento per elemento
    Polynomial operator%(int mod) const {
        Polynomial result(this->size());
        for (int i = 0; i < this->size(); i++) {
            result[i] = (*this)[i] % mod;
        }
        return result;
    }
};