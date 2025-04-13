#ifndef HASH_H
#define HASH_H
#include <vector>
#include <openssl/evp.h>
#include <openssl/sha.h>
std::vector<uint8_t> J(const std::vector<uint8_t>& input);
std::vector<uint8_t> H(const std::vector<uint8_t>& input);
std::pair<std::vector<uint8_t>, std::vector<uint8_t>> G(const std::vector<uint8_t>& input);
#endif
