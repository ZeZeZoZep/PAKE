#include "../include/int.h"

using namespace std;

// Converte un array di bit (0,1) in un array di byte.
// L'array di bit deve avere una lunghezza multipla di 8.
vector<uint8_t> BitsToBytes(const vector<int>& bits) {
    if(bits.size() % 8 != 0) {
        throw runtime_error("La lunghezza dell'array di bit non è multipla di 8.");
    }
    size_t numBytes = bits.size() / 8;
    vector<uint8_t> B(numBytes, 0);
    // Per ogni bit: aggiunge b[i] * 2^(i mod 8) al byte corrispondente.
    for (size_t i = 0; i < bits.size(); i++) {
        size_t byteIndex = i / 8;      // indice del byte corrente
        size_t bitIndex = i % 8;         // posizione del bit nel byte (0 = LSB)
        B[byteIndex] += bits[i] * (1 << bitIndex);
    }
    return B;
}

// Converte un array di byte in un array di bit (0,1).
vector<int> BytesToBits(const vector<uint8_t>& bytes) {
    vector<int> bits;
    bits.resize(bytes.size() * 8, 0);
    // Per ogni byte, estrai i suoi 8 bit in ordine (LSB per primo)
    for (size_t i = 0; i < bytes.size(); i++) {
        uint8_t temp = bytes[i]; // copia per non modificare l'originale
        for (size_t j = 0; j < 8; j++) {
            bits[i * 8 + j] = temp % 2;
            temp /= 2;
        }
    }
    return bits;
}


// --- Algoritmo 5: ByteEncode ---
// Codifica un array di 256 interi, ognuno rappresentabile con d bit,
// in un array di byte. L'output è un array di 32*d byte (cioè 256*d bit convertiti in byte).
vector<uint8_t> ByteEncode(const vector<uint16_t>& F, int d, uint16_t q = 3329) {
    if(F.size() != 256) {
        throw runtime_error("L'array di input deve contenere 256 interi.");
    }
    if(d < 1 || d > 12) {
        throw runtime_error("Il parametro d deve essere compreso tra 1 e 12.");
    }
    // Se d < 12, m = 2^d; se d == 12, m = q.
    uint16_t m = (d < 12) ? (1 << d) : q;

    // Prepariamo un array di bit di lunghezza 256*d.
    vector<int> b(256 * d, 0);
    
    // Per ogni intero F[i], estrai i d bit in rappresentazione little-endian.
    for (int i = 0; i < 256; i++) {
        uint16_t a = F[i] % m;  // Assicuriamoci che a sia in Z_m.
        for (int j = 0; j < d; j++) {
            // Estrae il j-esimo bit (LSB al j=0)
            b[i * d + j] = a % 2;
            // Dividi a per 2 (o fai shift a destra)
            a = a / 2;  // Poiché a - (a mod 2) è sempre pari, si ha a = floor(a/2)
        }
    }
    // Converte l'array di bit in un array di byte.
    vector<uint8_t> B = BitsToBytes(b);
    return B;
}

// --- Algoritmo 6: ByteDecode ---
// Decodifica un array di byte (ottenuto da ByteEncode) in un array di 256 interi a d bit.
vector<uint16_t> ByteDecode(const vector<uint8_t>& B, int d, uint16_t q = 3329) {
    if(d < 1 || d > 12) {
        throw runtime_error("Il parametro d deve essere compreso tra 1 e 12.");
    }
    // Recupera l'array di bit dall'array di byte.
    vector<int> b = BytesToBits(B);
    // Controlla che il numero di bit sia 256*d.
    if(b.size() < 256u * d) {
        throw runtime_error("L'array di bit ottenuto non contiene abbastanza bit.");
    }
    
    // Se d < 12, m = 2^d; se d == 12, m = q.
    uint16_t m = (d < 12) ? (1 << d) : q;
    vector<uint16_t> F(256, 0);
    
    // Per ogni intero, ricostruisce il valore come somma pesata dei d bit.
    for (int i = 0; i < 256; i++) {
        uint16_t value = 0;
        for (int j = 0; j < d; j++) {
            // b[i*d + j] * 2^j
            value += b[i * d + j] * (1 << j);
        }
        // Prende il valore modulo m.
        F[i] = value % m;
    }
    return F;
}

