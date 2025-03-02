#ifndef INT_H
#define INT_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>

std::vector<int> BytesToBits(const std::vector<uint8_t>& bytes);
std::vector<uint8_t> BitsToBytes(const std::vector<int>& bits);
std::vector<uint8_t> ByteEncode(const std::vector<uint16_t>& F, int d, uint16_t q);
std::vector<uint16_t> ByteDecode(const std::vector<uint8_t>& B, int d, uint16_t q);
#endif // INT_H
