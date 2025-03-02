#include "../include/random.h"
using namespace std;
uint16_t P_random() {
    static random_device rd;  // Seed per il generatore
    static mt19937 gen(rd()); // Mersenne Twister
    static uniform_real_distribution<double> dist(0.0, 1.0); // Distribuzione uniforme [0,1]

    double rand_num = dist(gen);

    if (rand_num < 0.5) {
        return 0;   // Probabilità 1/2
    } else if (rand_num < 0.75) {
        return 1;   // Probabilità 1/4
    } else {
        return 2;  // Probabilità 1/4
    }
}

// Funzione per generare numeri casuali da una distribuzione gaussiana
int16_t gaussian_random(double mean, double std_dev) {
    static random_device rd;  // Seed casuale
    static mt19937 gen(rd()); // Generatore Mersenne Twister
    normal_distribution<double> dist(mean, std_dev); // Distribuzione normale
    
    return static_cast<int16_t>(round(dist(gen))); // Arrotonda al numero intero più vicino
}

uint16_t uniform_q_random(int q) {
    static random_device rd;  // Seed casuale
    static mt19937 gen(rd()); // Generatore Mersenne Twister
    static uniform_real_distribution<double> dist(0.0, static_cast<double>(q)); // Distribuzione uniforme
    
    return static_cast<int16_t>(round(dist(gen))); // Arrotonda al numero intero più vicino
}