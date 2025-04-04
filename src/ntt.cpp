#include "../include/ntt.h"
#include <vector>
using namespace std;
using namespace Eigen;

// Funzione per invertire i bit
uint16_t bitrev7(uint16_t x) {
    uint16_t r = 0;
    for (int i = 0; i < 7; i++) {
        r = (r << 1) | (x & 1);
        x >>= 1;
    }
    return r;
}

// Recupera il valore ζ₂BitRev7(i)+1 modulo q
uint16_t getZeta2(uint16_t i) {
    uint16_t rev = bitrev7(i);
    return (zetas[rev] + 1) % q;
}

// Base-case multiplication con Eigen
Vector2i baseCaseMultiply(Vector2i f, Vector2i g, uint16_t zeta) {
    uint32_t t = ((uint32_t) f[1] * g[1]) % q;
    int h0 = ((uint32_t) f[0] * g[0] + (uint32_t) zeta * t) % q;
    int h1 = ((uint32_t) f[0] * g[1] + (uint32_t) f[1] * g[0]) % q;
    return Vector2i(h0, h1);
}

// Somma due rappresentazioni NTT con Eigen
Polynomial addNTTs(const Polynomial &f, const Polynomial &g) {
    Polynomial h = f + g;
    for (int i = 0; i < h.size(); i++) {
        h(i) = (h(i) % q + q) % q;  // Assicura valori positivi
    }
    return h;
}

// Sottrazione tra NTT con Eigen
Polynomial subNTTs(const Polynomial &f, const Polynomial &g) {
    Polynomial h = f - g;
    for (int i = 0; i < h.size(); i++) {
        h(i) = (h(i) % q + q) % q;  // Assicura valori positivi
    }
    return h;
}

// Moltiplicazione NTT con Eigen
Polynomial multiplyNTTs(const Polynomial &f, const Polynomial &g) {
    Polynomial h(N);
    for (int i = 0; i < N / 2; i++) {
        uint16_t zeta = getZeta2(i);
        Vector2i result = baseCaseMultiply(f.segment<2>(2 * i), g.segment<2>(2 * i), zeta);
        h.segment<2>(2 * i) = result;
    }
    for (int i = 0; i < h.size(); i++) {
        h(i) = (h(i) % q + q) % q;  // Assicura che il valore sia corretto
    }
    return h;
}

// Algoritmo NTT con Eigen
Polynomial ntt(Polynomial f) {
    int i = 1;
    for (int len = 128; len >= 2; len /= 2) {
        for (int start = 0; start < N; start += 2 * len) {
            uint16_t zeta = zetas[i++] % q;
            for (int j = start; j < start + len; j++) {
                uint16_t t = ((uint32_t) zeta * f[j + len]) % q;
                f[j + len] = (f[j] + q - t) % q;
                f[j] = (f[j] + t) % q;
            }
        }
    }
    return f;
}

// Algoritmo Inverse NTT con Eigen
Polynomial inv_ntt(Polynomial f_hat) {
    Polynomial f = f_hat;
    int i = 127;
    for (int len = 2; len <= 128; len *= 2) {
        for (int start = 0; start < N; start += 2 * len) {
            uint16_t zeta = zetas[i--] % q;
            for (int j = start; j < start + len; j++) {
                uint16_t t = f[j];
                f[j] = (t + f[j + len]) % q;
                int diff = (f[j + len] >= t) ? f[j + len] - t : f[j + len] + q - t;
                f[j + len] = ((uint32_t) zeta * diff) % q;
            }
        }
    }
    for (int j = 0; j < N; j++) {
        f[j] = ((uint32_t) f[j] * 3303) % q;
    }
    return f;
}

// SHAKE256 con OpenSSL
void shake256(const vector<uint8_t>& input, vector<uint8_t>& output) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_shake256(), NULL);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinalXOF(ctx, output.data(), output.size());
    EVP_MD_CTX_free(ctx);
}

// Campionamento SampleNTT con Eigen
Polynomial SampleNTT(const vector<uint8_t>& seed, uint8_t idx1, uint8_t idx2) {
    vector<uint8_t> B(34);
    copy(seed.begin(), seed.end(), B.begin());
    B[32] = idx1;
    B[33] = idx2;

    unsigned char buffer[3];
    Polynomial a(N);
    size_t j = 0;

    while (j < N) {
        vector<uint8_t> shake_output(3);
        shake256(B, shake_output);
        uint16_t d1 = shake_output[0] + 256 * (shake_output[1] & 0x0F);
        uint16_t d2 = (shake_output[1] >> 4) + 16 * shake_output[2];

        if (d1 < q) a[j++] = d1;
        if (j < N && d2 < q) a[j++] = d2;

        B[32] ^= shake_output[0];
        B[33] ^= shake_output[1];
    }
    a.isNTT=true;
    return a;
}

// Campionamento SamplePolyCBD con Eigen
Polynomial SamplePolyCBD(const vector<uint8_t>& B, int eta) {
    if (B.size() != 64u * eta) throw runtime_error("L'array B deve avere lunghezza 64*eta byte.");

    Polynomial f(N);
    for (int i = 0; i < N; i++) {
        int x = 0, y = 0;
        int start = 2 * eta * i;

        for (int j = 0; j < eta; j++) x += (B[start + j] & 1);
        for (int j = 0; j < eta; j++) y += (B[start + eta + j] & 1);

        int diff = x - y;
        if (diff < 0) diff += q;
        f[i] = diff % q;
    }
    return f;
}