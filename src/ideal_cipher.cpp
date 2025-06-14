#include "ideal_cipher.h"

#include "common.h"
#include "timerset.h"

using namespace std;
using namespace boost::multiprecision;
std::vector<uint8_t> IdealCipher::cpp_int_to_padded_bytes(const cpp_int& val, size_t min_len) {
    std::vector<uint8_t> bytes;
    cpp_int tmp = val;

    while (tmp > 0) {
        bytes.insert(bytes.begin(), static_cast<uint8_t>(tmp % 256));
        tmp /= 256;
    }

    // Aggiungi zeri in testa per padding a multipli di 16
    while (bytes.size() < min_len || bytes.size() % 16 != 0)
        bytes.insert(bytes.begin(), 0x00);

    return bytes;
}
cpp_int IdealCipher::bytes_to_cpp_int(const std::vector<uint8_t>& bytes) {
    cpp_int val = 0;
    for (uint8_t byte : bytes) {
        val <<= 8;
        val += byte;
    }
    return val;
}
std::vector<uint8_t> IdealCipher::aes_ecb_encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> ciphertext(plaintext.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int len;
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

std::vector<uint8_t> IdealCipher::aes_ecb_decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> plaintext(ciphertext.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL);
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int len;
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

vector<uint8_t> IdealCipher::encrypt_bytes(const vector<uint8_t>& in_bytes) {
    TimerSet ts("ID", "Encrypt");
    ts.start("Total");
    auto ret = aes_ecb_encrypt(in_bytes, this->key);
    ts.stop("Total");
    ts.commit(PARAM_D);
    return ret;
}
vector<uint8_t> IdealCipher::decrypt_bytes(const vector<uint8_t>& in_bytes) {
    TimerSet ts("ID", "Decrypt");
    ts.start("Total");
    auto ret = aes_ecb_decrypt(in_bytes, this->key);
    ts.stop("Total");
    ts.commit(PARAM_D);
    return ret;
    
}

cpp_int IdealCipher::encrypt_cpp_int(const cpp_int& input) {
    auto in_bytes = cpp_int_to_padded_bytes(input, 16); // padding a multipli di 16
    auto cipher_bytes = aes_ecb_encrypt(in_bytes, this->key);
    return bytes_to_cpp_int(cipher_bytes);
}

cpp_int IdealCipher::decrypt_cpp_int(const cpp_int& cipher_int) {
    auto cipher_bytes = cpp_int_to_padded_bytes(cipher_int, 16);  // stessa lunghezza usata in encrypt
    auto plain_bytes = aes_ecb_decrypt(cipher_bytes, this->key);
    return bytes_to_cpp_int(plain_bytes);
}