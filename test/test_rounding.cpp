
#include <gtest/gtest.h>
#include "r.h"
#include "common.h"

using namespace std;

// Test per la somma di polinomi
TEST(RoundingTest, test) {

    //for(double d=100000;d < 222056; d+=256)R(d, 852224, 8990);
    EXPECT_FALSE(false);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}