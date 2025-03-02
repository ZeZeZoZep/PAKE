#include "../include/ntt.h"
#include "../include/int.h"

using namespace std;

//const int N = 256;      // Dimensione del polinomio
//const uint16_t q = 3329; // Modulo (tipico nelle applicazioni NTT in crittografia)
// Array dei fattori zeta (128 valori) come da te fornito.
/* const uint16_t zetas[128] = {
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
}; */

//
// Funzione helper: bit reversal su 7 bit.
// Data un valore x (0 <= x < 128), restituisce il valore ottenuto invertendo l’ordine dei 7 bit.
//
uint16_t bitrev7(uint16_t x) {
    uint16_t r = 0;
    for (int i = 0; i < 7; i++) {
        r = (r << 1) | (x & 1);
        x >>= 1;
    }
    return r;
}

//
// Funzione che restituisce ζ₂BitRev7(i)+1, ossia:
//      getZeta2(i) = zetas[ bitrev7(i) ] + 1   (ridotto modulo q)
//
uint16_t getZeta2(uint16_t i) {
    uint16_t rev = bitrev7(i);
    return (zetas[rev] + 1) % q;
}

//
// Base-case multiplication.
// Dati due "coppie" (f₀, f₁) e (g₀, g₁) (componenti di una rappresentazione NTT) e un fattore zeta,
// calcola il prodotto in modo tale da restituire una coppia (h₀, h₁) secondo la formula:
//
//      t  = f₁ ⋅ g₁    (mod q)
//      h₀ = f₀ ⋅ g₀ + zeta ⋅ t    (mod q)
//      h₁ = f₀ ⋅ g₁ + f₁ ⋅ g₀      (mod q)
//
pair<uint16_t, uint16_t> baseCaseMultiply(uint16_t f0, uint16_t f1,
                                           uint16_t g0, uint16_t g1,
                                           uint16_t zeta) {
    uint32_t t = ((uint32_t) f1 * g1) % q;
    uint16_t h0 = (((uint32_t) f0 * g0) + ((uint32_t) zeta * t)) % q;
    uint16_t h1 = (((uint32_t) f0 * g1) + ((uint32_t) f1 * g0)) % q;
    return {h0, h1};
}
// Somma due rappresentazioni NTT, eseguendo l'operazione coefficiente per coefficiente modulo q
vector<uint16_t> addNTTs(const vector<uint16_t>& f, const vector<uint16_t>& g) {
    if (f.size() != N || g.size() != N) {
        throw invalid_argument("I vettori di input devono avere 256 elementi.");
    }
    
    vector<uint16_t> h(N, 0);

    // Somma coefficiente per coefficiente, con riduzione modulo q
    for (int i = 0; i < N; i++) {
        h[i] = (f[i] + g[i]) % q;
    }

    return h;
}
vector<uint16_t> subNTTs(const vector<uint16_t>& f, const vector<uint16_t>& g) {
    if (f.size() != N || g.size() != N) {
        throw invalid_argument("I vettori di input devono avere 256 elementi.");
    }
    
    vector<uint16_t> h(N, 0);

    // Somma coefficiente per coefficiente, con riduzione modulo q
    for (int i = 0; i < N; i++) {
        h[i] = (f[i] - g[i]) % q;
    }

    return h;
}
//
// Algorithm 11 MultiplyNTTs(f, g)
// Moltiplica (nel ring \(T_q\)) due rappresentazioni NTT (vettori di 256 coefficienti)
// utilizzando la base-case multiplication su coppie e un fattore zeta ottenuto in bit-reversal.
//
vector<uint16_t> multiplyNTTs(const vector<uint16_t>& f, const vector<uint16_t>& g) {
    if (f.size() != N || g.size() != N) {
        throw invalid_argument("I vettori di input devono avere 256 elementi.");
    }
    vector<uint16_t> h(N, 0);
    for (int i = 0; i < 128; i++) {
        // Recupera il fattore zeta per il blocco corrente: ζ2BitRev7(i) + 1
        uint16_t zeta = getZeta2(i);
        // Esegue la base-case multiplication sul blocco di due coefficienti:
        auto result = baseCaseMultiply(f[2 * i], f[2 * i + 1],
                                       g[2 * i], g[2 * i + 1],
                                       zeta);
        h[2 * i]     = result.first;
        h[2 * i + 1] = result.second;
    }
    return h;
}
// ========================================================================
// Algorithm 9: NTT(f)
// Calcola in-place la rappresentazione NTT del polinomio f ∈ ℤ₍q₎[X]/(X^256+…)
// ========================================================================
vector<uint16_t> ntt(const vector<uint16_t>& f) {
    // Copia del polinomio (l’algoritmo lavora in place)
    vector<uint16_t> F = f;
    int i = 1;
    // Loop: per len = 128, 64, 32, …, 2
    for (int len = 128; len >= 2; len /= 2) {
        // Per ogni blocco di 2*len coefficienti
        for (int start = 0; start < N; start += 2 * len) {
            // Otteniamo lo zeta (in ordine bit-reversal) e lo riduciamo modulo q
            uint16_t zeta = zetas[i] % q;
            i++;
            // Per ogni coppia all’interno del blocco:
            for (int j = start; j < start + len; j++) {
                // t = zeta * F[j+len] modulo q
                uint16_t t = (uint32_t)zeta * F[j + len] % q;
                // f[j+len] = f[j] - t modulo q
                F[j + len] = (F[j] + q - t) % q;
                // f[j] = f[j] + t modulo q
                F[j] = (F[j] + t) % q;
            }
        }
    }
    return F;
}

// ========================================================================
// Algorithm 10: Inverse NTT (NTT⁻¹)
// Calcola il polinomio f ∈ ℤ₍q₎ a partire dalla sua rappresentazione NTT.
// ========================================================================
vector<uint16_t> inv_ntt(const vector<uint16_t>& f_hat) {
    // Copia dell’array in input
    vector<uint16_t> f = f_hat;
    int i = 127;
    // Loop: per len = 2, 4, 8, …, 128
    for (int len = 2; len <= 128; len *= 2) {
        for (int start = 0; start < N; start += 2 * len) {
            // Otteniamo lo zeta dalla tabella (in ordine inverso)
            uint16_t zeta = zetas[i] % q;
            i--;
            for (int j = start; j < start + len; j++) {
                uint16_t t = f[j];
                // f[j] = t + f[j+len] modulo q
                f[j] = (t + f[j + len]) % q;
                // f[j+len] = zeta * (f[j+len] - t) modulo q
                // Per evitare numeri negativi, eseguiamo la sottrazione in ℤ₍q₎:
                int diff = f[j + len] >= t ? f[j + len] - t : f[j + len] + q - t;
                f[j + len] = (uint32_t)zeta * diff % q;
            }
        }
    }
    // Moltiplica ogni coefficiente per 3303 modulo q
    // (3303 è l'inverso di 128 modulo q: 128 * 3303 ≡ 1 mod q)
    for (int j = 0; j < N; j++) {
        f[j] = (uint32_t)f[j] * 3303 % q;
    }
    return f;
}
// Funzione per generare SHAKE256 (XOF)
void shake256(const unsigned char *input, size_t inlen, unsigned char *output, size_t outlen) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();  // Crea un nuovo contesto
    EVP_DigestInit_ex(ctx, EVP_shake256(), NULL);  // Inizializza SHAKE256
    EVP_DigestUpdate(ctx, input, inlen);  // Assorbe i dati
    EVP_DigestFinalXOF(ctx, output, outlen);  // Estrae i byte richiesti
    EVP_MD_CTX_free(ctx);  // Libera la memoria
}

// Implementazione di SampleNTT basata su OpenSSL SHAKE256
vector<uint16_t> SampleNTT(const vector<uint8_t>& seed, uint8_t idx1, uint8_t idx2, uint16_t q) {
    if (seed.size() != 32) {
        throw runtime_error("Il seed deve essere lungo 32 byte.");
    }
    
    // 1. Costruiamo l'array B di 34 byte: i 32 byte del seed seguiti dai due indici.
    vector<uint8_t> B(34);
    copy(seed.begin(), seed.end(), B.begin());
    B[32] = idx1;
    B[33] = idx2;

    // 2. SHAKE256: genera byte infiniti, da cui estraiamo blocchi da 3 byte
    unsigned char buffer[3];  // Contiene 3 byte alla volta
    vector<uint16_t> a;
    a.reserve(256);
    
    size_t j = 0;
    while (j < 256) {
        shake256(B.data(), B.size(), buffer, 3);  // Estrarre 3 byte
        
        // Calcolo di d1 e d2
        uint16_t d1 = buffer[0] + 256 * (buffer[1] & 0x0F);
        uint16_t d2 = (buffer[1] >> 4) + 16 * buffer[2];

        // Se d1 è minore di q, lo accettiamo
        if (d1 < q) {
            a.push_back(d1);
            j++;
        }
        // Se d2 è minore di q e non abbiamo ancora 256 coefficienti, lo accettiamo
        if (j < 256 && d2 < q) {
            a.push_back(d2);
            j++;
        }
        
        // Modifica il seed per generare nuovi valori (simuliamo uno stream XOF continuo)
        B[32] ^= buffer[0];  // XOR con il primo byte estratto
        B[33] ^= buffer[1];  // XOR con il secondo byte estratto
    }
    
    return a;
}

// Algorithm 8: SamplePolyCBD(B, eta)
// Prende in input un array B di 64*eta byte e restituisce un vettore f di 256 coefficienti in Z_q.
// Per ogni coefficiente f[i]:
//   - Si calcola x come la somma dei primi eta bit (presi da B) per quel coefficiente.
//   - Si calcola y come la somma dei successivi eta bit.
//   - Infine, f[i] = (x - y) mod q.
vector<uint16_t> SamplePolyCBD(const vector<uint8_t>& B, int eta, uint16_t q) {
    // Verifica che la lunghezza di B sia quella attesa: 64 * eta byte.
    if (B.size() != 64u * eta) {
        throw runtime_error("L'array B deve avere lunghezza 64*eta byte.");
    }
    
    // Converte B in un array di bit.
    vector<int> b = BytesToBits(B);
    // Dato che B ha 64*eta byte, b avrà 64 * eta * 8 = 512 * eta bit.
    // Per ogni coefficiente usiamo 2*eta bit e, essendo 256 coefficienti, occorrono 256*2*eta = 512*eta bit.
    if (b.size() < 256u * 2 * eta) {
        throw runtime_error("Non ci sono abbastanza bit in B.");
    }
    
    vector<uint16_t> f(256, 0);
    // Per ogni coefficiente f[i]
    for (int i = 0; i < 256; i++) {
        int x = 0, y = 0;
        // L'indice di partenza per i bit relativi a f[i]:
        int start = 2 * eta * i;
        // Calcola x come la somma dei primi eta bit.
        for (int j = 0; j < eta; j++) {
            x += b[start + j];
        }
        // Calcola y come la somma dei successivi eta bit.
        for (int j = 0; j < eta; j++) {
            y += b[start + eta + j];
        }
        int diff = x - y;
        // Se diff è negativo, lo portiamo nel range di Z_q aggiungendo q.
        if (diff < 0)
            diff += q;
        // Anche se diff è compreso tra -eta e eta, il modulo q lascia invariato il valore se positivo,
        // oppure lo trasforma in q - |diff| se negativo.
        f[i] = diff % q;
    }
    return f;
}


