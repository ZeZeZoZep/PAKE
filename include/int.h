#ifndef INT_H
#define INT_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;
using namespace boost::multiprecision;

std::vector<int> BytesToBits(const std::vector<uint8_t>& bytes);
std::vector<uint8_t> BitsToBytes(const std::vector<int>& bits);
std::vector<uint8_t> ByteEncode(const std::vector<uint16_t>& F, int d, uint16_t q);
std::vector<uint16_t> ByteDecode(const std::vector<uint8_t>& B, int d, uint16_t q);
vector<uint16_t> bigint_to_vector_baseq(const cpp_int& value, uint16_t q, size_t expected_length);
cpp_int vector_to_bigint_baseq(const vector<uint16_t>& vec, uint16_t q);
#endif // INT_H
