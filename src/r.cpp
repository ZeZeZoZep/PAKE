#include <iostream>

#include "../include/r.h"
// Definizione della funzione r^♯(x)
double r_sharp(double x, double q) {
    if (x >= -q / 4 && x < q / 4) {
        return 1.0;
    }
    return 0.0;
}

// Definizione della funzione r^b(x)
double r_b(double x, double T) {
    if (std::fabs(x) <= T) {
        return 1.0 / (2 * T);
    }
    return 0.0;
}

// Funzione per calcolare la probabilità Pr[R(x) = 1]
int R(double x, double q, double T) {
    double sum = 0.0;  // Somma per l'integrazione

    // Itera sull'intervallo [-q/2, q/2] con passo `step`
    for (double u = -q / 2; u < q / 2; u += 32) {
        double integrand = r_sharp(u, q) * r_b(x - u, T);
        sum += integrand; // Approssimazione numerica (somma dei rettangoli)
    }

    double probability = sum; // Probabilità Bernoulli
    double random_value = static_cast<double>(rand()) / RAND_MAX; // Numero casuale tra 0 e 1
    
    std::cout << "/double: " << x << " probability: " << probability << " /" << std::endl;
    
    // Applica la distribuzione di Bernoulli
    return (random_value < probability) ? 1 : 0;
}

/* int main() {
    std::srand(static_cast<unsigned>(std::time(0))); // Inizializza il generatore di numeri casuali
    
    double x = 10.0, q = 100.0, T = 5.0;
    int result = R(x, q, T);
    std::cout << "Risultato: " << result << std::endl;
    
    return 0;
} */