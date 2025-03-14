#include "../include/random.h"
#include "../include/random_mgs.h"
#include "../include/common.h"
#include <iostream>
#include <map>
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



uint16_t uniform_q_random(int q) {
    static random_device rd;  // Seed casuale
    static mt19937 gen(rd()); // Generatore Mersenne Twister
    static uniform_real_distribution<double> dist(0.0, static_cast<double>(q)); // Distribuzione uniforme
    
    return static_cast<int16_t>(round(dist(gen))); // Arrotonda al numero intero più vicino
}

//==============================================================================
// Samples from distribution D_{c,sigma}, ie                                              
// Samples an element in Z with probability proportionnal to e^{-(c-x)^2/2*(sigma^2)}    
//==============================================================================
signed int gaussian_random(RR_t c, RR_t sigma)
	{
	//printf("SampleZ with sigma = %Lf, c = %Lf\n", sigma, c);
	
	return algorithmF(c, sigma);
	}

/* int main(){
    int num_samples = 10000;
    std::map<int, int> histogram;

    for (int i = 0; i < num_samples; ++i) {
        int sample = gaussian_random(0, PARAM_SIGMA);
        histogram[sample]++;
    }

    std::cout << "\nHistogram of algorithmF results:" << std::endl;
    for (const auto& [value, count] : histogram) {
        if (count > num_samples / 200) { // Show only significant counts
            std::cout << value << ": " << std::string(count / 50, '*') << std::endl;
        }
    }
    return 0;
} */