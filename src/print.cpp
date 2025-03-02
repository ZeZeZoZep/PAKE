#include "../include/print.h"

using namespace std;

void matrix_print(vector<uint16_t> matrix, uint16_t d,uint16_t h,uint16_t k){
    for(int i=0; i<d; i++){
        for(int j=0; j<h; j++){
            cout << "(" << i << ", " << j <<") : [";

            
            for(int c=0; c<k; c++){
                cout << matrix[i*j*c + j*c + c] << ", ";

            }
            cout << "]\n";
        }
    }
}
void poly_print(vector<uint16_t> poly, uint16_t k){


    cout <<"[";    
    for(int c=0; c<k; c++){
        cout << poly[c] << ", ";
    }
    cout << "]\n";


}