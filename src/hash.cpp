#include "../include/hash.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <iostream>

std::vector<uint8_t> J(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> output(32);  // 32 byte = 256 bit
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_shake256(), NULL);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinalXOF(ctx, output.data(), output.size());
    EVP_MD_CTX_free(ctx);
    return output;
}
std::vector<uint8_t> H(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> output(32); // SHA3-256 → 32 byte
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha3_256(), NULL);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinal_ex(ctx, output.data(), nullptr);
    EVP_MD_CTX_free(ctx);
    return output;
}
std::pair<std::vector<uint8_t>, std::vector<uint8_t>> G(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> digest(64);  // SHA3-512 → 64 byte
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha3_512(), NULL);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinal_ex(ctx, digest.data(), nullptr);
    EVP_MD_CTX_free(ctx);

    std::vector<uint8_t> a(digest.begin(), digest.begin() + 32);
    std::vector<uint8_t> b(digest.begin() + 32, digest.end());
    return {a, b};
}
