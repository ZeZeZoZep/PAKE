#ifndef INT_H
#define INT_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <initializer_list>
using namespace std;
using namespace boost::multiprecision;

std::vector<int> BytesToBits(const std::vector<uint8_t>& bytes);
std::vector<uint8_t> BitsToBytes(const std::vector<int>& bits);
std::vector<uint8_t> ByteEncode(const std::vector<uint16_t>& F, int d, uint16_t q);
std::vector<uint16_t> ByteDecode(const std::vector<uint8_t>& B, int d, uint16_t q);

template<typename... Vectors>
std::vector<uint8_t> concatVectors(const std::vector<uint8_t>& first, const Vectors&... rest) {
    std::vector<uint8_t> result;
    
    // Calcolo la dimensione totale
    size_t totalSize = first.size() + (rest.size() + ...);
    result.reserve(totalSize);
    
    // Inserisco i dati
    result.insert(result.end(), first.begin(), first.end());
    (result.insert(result.end(), rest.begin(), rest.end()), ...);
    
    return result;
};
#endif // INT_H
