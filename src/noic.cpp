#include "noic.h"



template<int Cols>
PolynomialMatrix<1, Cols+1> add_rho_to_pk(PolynomialMatrix<1, Cols> pk, vector<uint8_t> rho){
    Polynomial poly_rho(PARAM_N);
    vector<int> b = BytesToBits(rho);
    for(int i=0; i<PARAM_N; i++){
        poly_rho(i) = b[i]*static_cast<int>(PARAM_Q/2);
    }
    PolynomialMatrix<1,Cols+1> pk_rho;
    for(int i=0; i<Cols; i++){
        pk_rho(0,i) = pk(0,i);
    }
    pk_rho(0,Cols) = poly_rho;
    return pk_rho;
}

template<int Cols>
pair<vector<uint8_t>,PolynomialMatrix<1, Cols>> split_rho_from_pk(PolynomialMatrix<1, Cols+1> pk_rho){
    Polynomial poly_rho= pk_rho(0,Cols);
    vector<int> b(PARAM_N); 
    vector<uint8_t> rho(PARAM_N/8); 
      
    for(int i=0; i<PARAM_N; i++){
        if(poly_rho(i) < static_cast<int>(PARAM_Q)/4 || poly_rho(i) > static_cast<int>(3*PARAM_Q)/4){
            b[i] = 0;
        }
        else{
            b[i] = 1;
        }
    }
    rho = BitsToBytes(b);
    PolynomialMatrix<1, Cols> pk;
    for(int i=0; i<Cols; i++){
        pk(0,i) = pk_rho(0,i);
    }
    return {rho,pk};
}




NOIC_PAKE::NOIC_PAKE(uint ssid, string password, vector<uint8_t> Epk)
: feistel([&]() {
      vector<uint8_t> input(password.begin(), password.end());

      // serializza ssid come 4 byte (little-endian)
      for (int i = 0; i < 4; ++i) {
          input.push_back((ssid >> (8 * i)) & 0xFF);
      }

      vector<uint8_t> hash = H(input);  // supponiamo ritorni vector<uint8_t>
      return vector<uint8_t>(hash.begin(), hash.begin() + 16);  // prendi i primi 16 byte
  }())
{
    
    this->ssid=ssid;
    this->password=password;
    this->z=vector<uint8_t>(32,0);


    if (RAND_bytes(this->r.data(), this->r.size()) != 1)throw std::runtime_error("RAND_bytes failed");

    if(!Epk.size()){
        this->r=vector<uint8_t>(96,0);    
        auto ret = kem.KeyGen();
        auto keygen_ret = ret.first;
        //this->z = ret.second;
        this->pk=get<0>(keygen_ret);
        this->sk=get<1>(keygen_ret);
        this->rho=get<2>(keygen_ret);

        vector<uint8_t> pk_bytes = PolynomialMatrixUtils::Encode_pm(this->pk);
        PolynomialMatrix<1, PARAM_D+1> pk_rho = add_rho_to_pk<PARAM_D>(this->pk,this->rho);
        auto [s, T] = this->feistel.encrypt<PARAM_D+1>(this->r,pk_rho);
        vector<uint8_t> t_bytes = PolynomialMatrixUtils::Encode_pm<1, PARAM_D+1>(T);
        this->pk_rho_bytes=t_bytes;
        this->Epk = concatVectors(s,t_bytes);
    }
    else {
        this->Epk = Epk;
        vector<uint8_t> s(Epk.begin(),Epk.begin() + 96);
        vector<uint8_t> t_bytes(Epk.begin() + 96,Epk.end());
        this->pk_rho_bytes=t_bytes;
        PolynomialMatrix<1, PARAM_D+1> T = PolynomialMatrixUtils::Decode_pm<1, PARAM_D+1>(t_bytes);
        auto [r, pk_rho] = this->feistel.decrypt<PARAM_D+1>(s,T);
        this->r=r;
        auto [rho,pk] = split_rho_from_pk<PARAM_D>(pk_rho);
        this->pk=pk;
        this->rho=rho;
    }

}


pair<vector<uint8_t>,vector<uint8_t>> NOIC_PAKE::auth(){

    auto ret = this->kem.Encaps(this->pk,this->rho);

    auto c_poly=ret.second;
    vector<uint8_t> K=ret.first;
    this->K=K;
    vector<uint8_t> c1 = PolynomialMatrixUtils::Encode_pm(c_poly.first);
    vector<uint8_t> c2 = PolynomialMatrixUtils::Encode_pm(c_poly.second);

    vector<uint8_t> c = concatVectors(c1,c2);

    vector<uint8_t>vec=vector<uint8_t>(password.begin(), password.end());
    vec.push_back(ssid);

    vector<uint8_t> bytes_m = concatVectors(vec,this->pk_rho_bytes,this->Epk,c,K);

    vector<uint8_t> auth = H(bytes_m);
/*     std::cout << "HASH (hex): ";
    for (std::size_t i = 0; i < auth.size(); ++i) {
        std::printf("%02X ", auth[i]);
    } 
    std::cout << std::endl; */
    return {c,auth};
}
bool NOIC_PAKE::check(vector<uint8_t> c, vector<uint8_t> auth){
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



    vector<uint8_t> serve_bytes = concatVectors(vec,this->pk_rho_bytes,this->Epk,c1_s,c2_s,K_first);

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
vector<uint8_t> NOIC_PAKE::derive(vector<uint8_t> c, vector<uint8_t> auth){

    vector<uint8_t> bytes = concatVectors(this->pk_rho_bytes,this->Epk,c,auth,this->K);
    bytes.push_back(this->ssid);
    return H(bytes);
}