#include "ideal_cipher_pake.h"


ID_PAKE::ID_PAKE(uint ssid, string password, vector<uint8_t> Epk){
    this->ssid=ssid;
    this->password=password;
    this->z=vector<uint8_t>(32,0);
    vector<uint8_t>vec=vector<uint8_t>(password.begin(), password.end());
    vec.push_back(ssid);
    vec=H(vec);

    this->ideal_cipher=IdealCipher(vector<uint8_t>(vec.begin(),vec.begin() + 16));

    if(!Epk.size()){    
        auto ret = kem.KeyGen();
        auto keygen_ret = ret.first;
        //this->z = ret.second;
        this->pk=get<0>(keygen_ret);
        this->sk=get<1>(keygen_ret);
        this->rho=get<2>(keygen_ret);
        vector<uint8_t> pk_bytes = PolynomialMatrixUtils::Encode_pm(this->pk);
        pk_bytes.insert(pk_bytes.end(),this->rho.begin(),this->rho.end());
        this->Epk = ideal_cipher.encrypt_bytes(pk_bytes);
    }
    else {
        this->Epk = Epk;
        vector<uint8_t> bytes= ideal_cipher.decrypt_bytes(Epk);
        vector<uint8_t> pk_bytes;
        pk_bytes.insert(pk_bytes.begin(),bytes.begin(),bytes.end() - 32);
        this->rho.insert(this->rho.begin(),bytes.end()- 32,bytes.end());
        this->pk=PolynomialMatrixUtils::Decode_pm<1, PARAM_D>(pk_bytes);   
    }

}


pair<vector<uint8_t>,vector<uint8_t>> ID_PAKE::auth(){

    auto ret = this->kem.Encaps(this->pk,this->rho);

    auto c_poly=ret.second;
    vector<uint8_t> K=ret.first;
    this->K=K;
    vector<uint8_t> c1 = PolynomialMatrixUtils::Encode_pm(c_poly.first);
    vector<uint8_t> c2 = PolynomialMatrixUtils::Encode_pm(c_poly.second);
/*     std::cout << "HASH (hex): ";
    for (std::size_t i = 0; i < c2.size(); ++i) {
        std::printf("%02X ", c2[i]);
    } 
    std::cout << std::endl;
 */
    vector<uint8_t> c = concatVectors(c1,c2);

    vector<uint8_t>vec=vector<uint8_t>(password.begin(), password.end());
    vec.push_back(ssid);

    vector<uint8_t> bytes_m = concatVectors(vec,this->Epk,c,K);

    vector<uint8_t> auth = H(bytes_m);
/*     std::cout << "HASH (hex): ";
    for (std::size_t i = 0; i < auth.size(); ++i) {
        std::printf("%02X ", auth[i]);
    } 
    std::cout << std::endl; */
    return {c,auth};
}
bool ID_PAKE::check(vector<uint8_t> c, vector<uint8_t> auth){
    vector<uint8_t> c1_bytes(c.begin(),c.end()-384);
    vector<uint8_t> c2_bytes(c.end()-384,c.end());// c2 is 384 bytes long
/*          std::cout << "HASH (hex): ";
    for (std::size_t i = 0; i < c2_bytes.size(); ++i) {
        std::printf("%02X ", c2_bytes[i]);
    } 
    std::cout << std::endl; */
    PolynomialMatrix<1, PARAM_D> c1 = PolynomialMatrixUtils::Decode_pm<1, PARAM_D>(c1_bytes);
    PolynomialMatrix<1, 1> c2 = PolynomialMatrixUtils::Decode_pm<1, 1>(c2_bytes);

    // TODO risolvere problemi di encoding, pulire gli input delle funzioni kem, il vettore z
    vector<uint8_t> K_first=kem.Decaps(this->sk,this->pk,this->rho,this->z,c1,c2);


    vector<uint8_t> c1_s = PolynomialMatrixUtils::Encode_pm(c1);
    vector<uint8_t> c2_s = PolynomialMatrixUtils::Encode_pm(c2);

    vector<uint8_t>vec=vector<uint8_t>(this->password.begin(), this->password.end());
    vec.push_back(this->ssid);

    vector<uint8_t> serve_bytes = concatVectors(vec,this->Epk,c1_s,c2_s,K_first);

    vector<uint8_t> auth2 = H(serve_bytes);
/*     std::cout << "HASH (hex): ";
    for (std::size_t i = 0; i < auth.size(); ++i) {
        std::printf("%02X ", auth[i]);
    } 
    std::cout << std::endl; */
    if (auth==auth2){
        this->K=K_first;
        return true;
    }
    this->K={};
    return false;
}
vector<uint8_t> ID_PAKE::derive(vector<uint8_t> c, vector<uint8_t> auth){

    vector<uint8_t> bytes = concatVectors(this->Epk,c,auth,this->K);
    bytes.push_back(this->ssid);
    return H(bytes);
}