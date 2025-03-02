#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// ✅ Specialized Babai Algorithm for the given Sk
pair<uint,vector<int>> decodeS(const vector<int> &b, int k, int q) {
    uint s=0;
    vector<int> e(k);
    for(int i=k-1; i>=0; i--){
        unsigned int x=(unsigned int)round(b[i]-pow(2,i)*s) % q;
            // Estrai il bit x_{k-1} (il bit più significativo)
        unsigned int x_k_minus_1 = (x >> (k - 1)) & 1;

        // Estrai il bit x_{k-2}
        unsigned int x_k_minus_2 = (x >> (k - 2)) & 1;

        // Calcola x_{k-1} ⊕ x_{k-2}
        unsigned int result = x_k_minus_1 ^ x_k_minus_2;
        //cout<<x <<"  " <<x_k_minus_1<<"  "<<x_k_minus_2<<"  "<<result<<endl;
        s=s+pow(2,k-1-i)*result;
        int temp=x;
        //while(!(temp>=-q/4 && temp<q/4)) if (temp>0) temp-=q/2; else temp += q/2;
        e[i]=temp;
    }
    pair<int,vector<int>> ret;
    ret.first=s;
    ret.second= e;
    return ret;
}
pair<uint,vector<int>> decodeS_alt(const vector<int> &b, int k) {
    uint s=0;
    int q = 4096;
    vector<int> e(k);
    for(int i=k-1; i>=0; i--){
        unsigned int x=(unsigned int)round(b[i]-pow(2,i)*s) % q;
            // Estrai il bit x_{k-1} (il bit più significativo)
        unsigned int x_k_minus_1 = (x >> (k - 1)) & 1;

        // Estrai il bit x_{k-2}
        unsigned int x_k_minus_2 = (x >> (k - 2)) & 1;
        
        // Calcola x_{k-1} ⊕ x_{k-2}
        unsigned int result = x_k_minus_1 ^ x_k_minus_2;
        cout<<x <<"  " <<x_k_minus_1<<"  "<<x_k_minus_2<<"  "<<result<<endl;    
        s=s+pow(2,k-1-i)*result;
        int temp=x;
        //while(!(temp>=-q/4 && temp<q/4)) if (temp>0) temp-=q/2; else temp += q/2;
        e[i]=temp;
    }
    pair<int,vector<int>> ret;
    ret.first=s;
    ret.second= e;
    return ret;
}
void printBits(unsigned int num) {
    for (int i = 31; i >= 0; --i) { // Supponendo un intero a 32 bit
        std::cout << ((num >> i) & 1);
        if (i % 8 == 0) std::cout << " "; // Separazione per leggibilità
    }
    std::cout << std::endl;
}
// ✅ Main function
int main() {
    int k = 12;
    int q = 3329;
    int s = 5;
    for(int x=s; x<q; x++){
        vector<int> e={1,-1,1,-1,1,-1,1,-1,1,-1,1,-1};
        // Target vector b (example)
        vector<int> b(k);
        for (int i = 0; i < k; i++) {
            b[i] = (1 << i) *x % q ;//+ e[i]; // Example b = g * s with no error
        }

        // Run the specialized Babai algorithm
        auto ret = decodeS(b, k, q);
        int s_recovered = ret.first;
        vector<int> e_recovered = ret.second;
        // Print the result
        
        if(x!=s_recovered){
            cout << "Original s: " << x << endl;
            printBits(x);

            cout << "Recovered s: " << s_recovered << endl;
            printBits(s_recovered);
/*             auto ret = decodeS_alt(b, k);
            cout << "Recovered alt s: " << ret.first << endl;
            printBits(ret.first); */
            cout << "Recovered e: "<< endl;
            for (int &v : e_recovered) {
                cout << v << " ";
            }
            cout << endl;
            break;
        }

    }
    return 0;
}
