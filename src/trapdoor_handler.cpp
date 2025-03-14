#include "../include/trapdoor_handler.h"

using namespace std;
using namespace Eigen;


pair<PolynomialMatrix<PARAM_D, PARAM_M>,PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K>> TrapdoorHandler::Trapgen() {
    // 1. Generiamo la matrice a_0 in modo uniforme in R_q^{d × d}
    PolynomialMatrix<PARAM_D, PARAM_D> A_hat = generate_uniform_polymatrix<PARAM_D, PARAM_D>();
    cout << "Matrice a (a0):\n" << A_hat << "\n\n";

    // 3. Generiamo la matrice T ∈ R^{2d×dk} con distribuzione χ
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> T = generate_gaussian_constant_polymatrix<2*PARAM_D,PARAM_D*PARAM_K>();
    PolynomialMatrix<PARAM_D, PARAM_D> I = generate_identity_polymatrix<PARAM_D, PARAM_D>();
    PolynomialMatrix<PARAM_D, 2*PARAM_D> A_first;
    A_first << I , A_hat;
    PolynomialMatrix<PARAM_D, PARAM_D*PARAM_K> G = generate_gadget_polymatrix<PARAM_D, PARAM_D*PARAM_K>();

    // 4. Generiamo g come un vettore riga deterministico (g ∈ R_q^{1×k})
    // 5. Calcoliamo il secondo blocco di `a`
    PolynomialMatrix<PARAM_D,PARAM_D*PARAM_K> A1 = (G - A_first * T);

    // 6. Costruiamo `a` concatenando `a_0` e `a_1`
    PolynomialMatrix<PARAM_D, PARAM_M> A;
    A << A_first, A1;
    return {A,T};
}
/* int main() {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> a = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    // Stampiamo i risultati
    cout << "Matrice a (R_q^{2l × k}):\n" << a << "\n\n";
    cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";

    return 0;
} */
