#ifndef NTT_H
#define NTT_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <Eigen/Dense>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "../include/polynomial.h"
#include "../include/int.h"
// Forward declaration della classe Polynomial (evita dipendenze circolari)

const int N = 256;      // Dimensione del polinomio
const uint16_t q = 3329; //modulo degli interi usati come coefficenti
const uint16_t zetas[128] = {
    1, 1729, 2580, 3289, 2642, 630, 1897, 848,
    1062, 1919, 193, 797, 2786, 3260, 569, 1746,
    296, 2447, 1339, 1476, 3046, 56, 2240, 1333,
    1426, 2094, 535, 2882, 2393, 2879, 1974, 821,
    289, 331, 3253, 1756, 1197, 2304, 2277, 2055,
    650, 1977, 2513, 632, 2865, 33, 1320, 1915,
    2319, 1435, 807, 452, 1438, 2868, 1534, 2402,
    2647, 2617, 1481, 648, 2474, 3110, 1227, 910,
    17, 2761, 583, 2649, 1637, 723, 2288, 1100,
    1409, 2662, 3281, 233, 756, 2156, 3015, 3050,
    1703, 1651, 2789, 1789, 1847, 952, 1461, 2687,
    939, 2308, 2437, 2388, 733, 2337, 268, 641,
    1584, 2298, 2037, 3220, 375, 2549, 2090, 1645,
    1063, 319, 2773, 757, 2099, 561, 2466, 2594,
    2804, 1092, 403, 1026, 1143, 2150, 2775, 886,
    1722, 1212, 1874, 1029, 2110, 2935, 885, 2154
};
using namespace std;
using namespace Eigen;
uint16_t bitrev7(uint16_t x);
uint16_t getZeta2(uint16_t i);
std::pair<uint16_t, uint16_t> baseCaseMultiply(uint16_t f0, uint16_t f1,
    uint16_t g0, uint16_t g1,
    uint16_t zeta);
Polynomial subNTTs(const Polynomial& f, const Polynomial& g) ;
Polynomial addNTTs(const Polynomial& f, const Polynomial& g) ;
Polynomial multiplyNTTs(const Polynomial& f, const Polynomial& g) ;
Polynomial ntt(Polynomial f);
Polynomial inv_ntt(Polynomial f_hat);
void shake256(const vector<uint8_t>& input, vector<uint8_t>& output);
Polynomial SampleNTT(const vector<uint8_t>& seed, uint8_t idx1, uint8_t idx2);
Polynomial SamplePolyCBD(const vector<uint8_t>& B, int eta);
#endif