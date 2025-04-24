#include "kem.h"
KEM::KEM() {}

KEM::~KEM() {}


tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>> 
KEM::KeyGen_internal(
    vector<uint8_t>& d,
    vector<uint8_t>& z
){
    auto ret = pke.KeyGen(d);
    this->pk=get<0>(ret);
    this->sk=get<1>(ret);
    this->rho=get<2>(ret);
    return ret;
}

pair<vector<uint8_t>,pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, 1>>> 
KEM::Encaps_internal(
    PolynomialMatrix<1, PARAM_D> pk,
    vector<uint8_t>& rho,
    vector<uint8_t>& m
){
    //cout<<"Start enc"<<endl;
    vector<uint16_t> pk_encoded=PolynomialMatrixUtils::Encode(pk);
    vector<uint16_t> array;
    vector<uint8_t> ek,enc;
    for(int i=0; i<PARAM_D; i++){
        vector<uint16_t> array(pk_encoded.begin()+i*PARAM_N,pk_encoded.begin()+(i+1)*PARAM_N);
        vector<uint8_t> enc = ByteEncode(array,12,PARAM_Q);
        ek.insert(ek.end(),enc.begin(),enc.end());
    }

    ek.insert(ek.end(),rho.begin(),rho.end());
    //cout<<"ek computed"<<endl;

    vector<uint8_t> temp(32);
    vector<uint8_t> h_ek=H(ek);

    temp.insert(temp.begin(),m.begin(),m.end());
    temp.insert(temp.end(),h_ek.begin(),h_ek.end());

    auto ret1=G(temp);
    //cout<<"K computed"<<endl;

    auto ret = pke.Encrypt(pk,rho,m,ret1.second);
    return {ret1.first,ret};
}

vector<uint8_t>
KEM::Decaps_internal(
    PolynomialMatrix<1, PARAM_D> sk,
    PolynomialMatrix<1, PARAM_D> pk,
    vector<uint8_t>& rho,
    vector<uint8_t>& z, 
    PolynomialMatrix<1, PARAM_D> c1,
    PolynomialMatrix<1, 1> c2 
){
    //cout<<"Start Dec"<<endl;

    vector<uint8_t> m = pke.Decrypt(c1,c2,sk);
/*     std::cout << "[Server] Rpke.Encrypt computed: ";
    for (std::size_t i = 0; i < 32; ++i) {
        std::printf("%02X ", m[i]);
    }
    cout<<endl; */
    //cout<<"m computed"<<endl;

    vector<uint16_t> pk_encoded=PolynomialMatrixUtils::Encode(pk);
    vector<uint16_t> array;
    vector<uint8_t> ek,enc;
    for(int i=0; i<PARAM_D; i++){
        vector<uint16_t> array(pk_encoded.begin()+i*PARAM_N,pk_encoded.begin()+(i+1)*PARAM_N);
        vector<uint8_t> enc = ByteEncode(array,12,PARAM_Q);
        ek.insert(ek.end(),enc.begin(),enc.end());
    }
    ek.insert(ek.end(),rho.begin(),rho.end());
    //cout<<"ek computed"<<endl;
    vector<uint8_t> temp(32);
    vector<uint8_t> h_ek=H(ek);
    temp.insert(temp.begin(),m.begin(),m.end());
    temp.insert(temp.end(),h_ek.begin(),h_ek.end());

    auto ret1=G(temp);
    vector<uint8_t> K=ret1.first;
    //cout<<"K computed"<<endl;


    vector<uint16_t> c1_encoded=PolynomialMatrixUtils::Encode(c1);
    vector<uint8_t> c1_bytes;
    for(int i=0; i<PARAM_D; i++){
        vector<uint16_t> array(c1_encoded.begin()+i*PARAM_N,c1_encoded.begin()+(i+1)*PARAM_N);
        vector<uint8_t> enc = ByteEncode(array,12,PARAM_Q);
        c1_bytes.insert(c1_bytes.end(),enc.begin(),enc.end());
    }
    vector<uint8_t> c2_bytes = ByteEncode(PolynomialMatrixUtils::Encode(c2),12,PARAM_Q);
    c1_bytes.insert(c1_bytes.end(),c2_bytes.begin(),c2_bytes.end());
    //cout<<"c computed"<<endl;

    vector<uint8_t> temp2(32);
    temp2.insert(temp2.begin(),z.begin(),z.end());
    temp2.insert(temp2.end(),c1_bytes.begin(),c1_bytes.end());
    //cout<<"z || c computed"<<endl;

    vector<uint8_t> K_bar=J(temp2);
    //cout<<"K_bar computed"<<endl;
    
    auto ret = pke.Encrypt(pk,rho,m,ret1.second);
    //cout<<"pke.Encrypt computed"<<endl;

    if(pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, 1>>{c1,c2}!=ret){
        cout<<"if ciphertexts do not match, “implicitly reject”"<<endl;
        K=K_bar; 
    }

    return K;
}

pair<tuple<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, PARAM_D>,vector<uint8_t>>,vector<uint8_t>> 
KEM::KeyGen(){
    vector<uint8_t> d(32),z(32);
    if (RAND_bytes(d.data(), d.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    if (RAND_bytes(z.data(), z.size()) != 1) throw std::runtime_error("RAND_bytes failed");
    return {KeyGen_internal(d,z),z};
}
pair<vector<uint8_t>,pair<PolynomialMatrix<1, PARAM_D>,PolynomialMatrix<1, 1>>> 
KEM::Encaps(
    PolynomialMatrix<1, PARAM_D> pk,
    vector<uint8_t>& rho
){
    vector<uint8_t> m(32);
    if (RAND_bytes(m.data(), m.size()) != 1) throw std::runtime_error("RAND_bytes failed");
/*     cout<<"pke.Encrypt computed"<< m.data() <<endl;
    std::cout << "[Server] Rpke.Encrypt computed: ";
    for (std::size_t i = 0; i < 32; ++i) {
        std::printf("%02X ", m[i]);
    } 
    std::cout << std::endl; */
    return Encaps_internal(pk,rho,m);
}
vector<uint8_t>
KEM::Decaps(
    PolynomialMatrix<1, PARAM_D> sk,
    PolynomialMatrix<1, PARAM_D> pk,
    vector<uint8_t>& rho,
    vector<uint8_t>& z, 
    PolynomialMatrix<1, PARAM_D> c1,
    PolynomialMatrix<1, 1> c2 
){
    return Decaps_internal(sk,pk,rho,z,c1,c2);
}
