#include <iostream>
#include <fstream>
#include <NTL/matrix.h>
#include <NTL/vector.h>
#include <NTL/LLL.h>
using namespace std;
using namespace NTL;

// Babai's Nearest Plane Algorithm
void babaiNearestPlane(const Mat<ZZ>& B, const Vec<ZZ>& target, Vec<ZZ>& closest) {
    long n = B.NumCols();
    
    // Inizializza il vettore più vicino con il target
    closest = target;

    // Algoritmo di Babai (scansione inversa)
    for (long i = n - 1; i >= 0; i--) {
        ZZ dot;  
        ZZ norm;
        InnerProduct(dot,B[i], closest);
        InnerProduct(norm,B[i], B[i]);
        if (norm != 0) {
            ZZ rounded_coeff = (dot + (norm / 2)) / norm; // Arrotondamento
            for (long k = 0; k < n; k++) {
                closest[k] -= rounded_coeff * B[i][k]; // Sottrai il contributo
            }
        }
    }
}

int main() {
    long n = 3; // Dimensione del reticolo

    // Carica la base ridotta da file
    Mat<ZZ> B;
    B.SetDims(n, n);

    ifstream file("reduced_lattice.txt");
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            file >> B[i][j];
        }
    }
    file.close();

    cout << "Base ridotta caricata:\n" << B << endl;

    // Definizione del vettore target
    Vec<ZZ> target;
    target.SetLength(n);
    target[0] = 7;
    target[1] = 8;
    target[2] = 9;

    cout << "Vettore target: " << target << endl;

    // Calcolo del vettore più vicino nel reticolo
    Vec<ZZ> closest;
    closest.SetLength(n);
    babaiNearestPlane(B, target, closest);

    // Stampa del risultato
    cout << "Vettore più vicino nel reticolo:\n" << closest << endl;

    return 0;
}
