#ifndef IDEAL_CIPHER_H
#define IDEAL_CIPHER_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <boost/multiprecision/cpp_int.hpp>
#include "int.h"

using boost::multiprecision::cpp_int;

class IdealCipher {
private:
    std::vector<uint8_t> key;       // 128-bit AES key        

    std::vector<uint8_t> generateRandomKey() {
        std::vector<uint8_t> k(16);
        RAND_bytes(k.data(), k.size());
        return k;
    }
    static std::vector<uint8_t> aes_ecb_encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key);
    static std::vector<uint8_t> aes_ecb_decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& key);

public:
    IdealCipher(const std::vector<uint8_t>& key = {}){
        this->key = key.empty() ? generateRandomKey() : key;
        if (this->key.size() != 16)
            throw std::invalid_argument("AES-128 richiede chiave da 16 byte");
    }

    ~IdealCipher() {
    }
    static cpp_int bytes_to_cpp_int(const std::vector<uint8_t>& bytes);
    static std::vector<uint8_t> cpp_int_to_padded_bytes(const cpp_int& val, size_t min_len);
    vector<uint8_t> encrypt_bytes(const vector<uint8_t>& in_bytes);
    vector<uint8_t> decrypt_bytes(const vector<uint8_t>& in_bytes);
    cpp_int encrypt_cpp_int(const cpp_int& input);
    cpp_int decrypt_cpp_int(const cpp_int& cipher_int);
};

#endif // IDEAL_CIPHER_H

