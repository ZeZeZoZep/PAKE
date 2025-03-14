#include <gtest/gtest.h>
#include "trapdoor_handler.h"
// Test generazione
TEST(PolynomialTest, Addition1) {
    auto ret = TrapdoorHandler::Trapgen();
    PolynomialMatrix<PARAM_D, PARAM_M> a = ret.first;
    PolynomialMatrix<2*PARAM_D,PARAM_D*PARAM_K> R = ret.second;
    // Stampiamo i risultati
    cout << "Matrice a (R_q^{2l × k}):\n" << a << "\n\n";
    cout << "Trapdoor R (R_q^{l × k}):\n" << R << "\n";
    EXPECT_GT(a.rows(), 0) << "A_hat ha zero righe!";
    EXPECT_GT(a.cols(), 0) << "A_hat ha zero colonne!";
    EXPECT_GT(R.rows(), 0) << "R ha zero righe!";
    EXPECT_GT(R.cols(), 0) << "R ha zero colonne!";
}
// Esegue tutti i test
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}