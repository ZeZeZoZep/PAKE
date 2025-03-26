# Aggiungi GoogleTest (se non già presente nel sistema)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Includi la directory con gli header
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(../include)

# Aggiungi i test
add_executable(test_polynomial test_polynomial.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp  )
target_link_libraries(test_polynomial random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_polynomial_matrix test_polynomial_matrix.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp)
target_link_libraries(test_polynomial_matrix random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)


add_executable(test_trapdoor test_trapdoor.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp)
target_link_libraries(test_trapdoor random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)


add_executable(test_lpke test_lpke.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp ../src/lpke.cpp ../src/r.cpp)
target_link_libraries(test_lpke random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_rounding test_rounding.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp  ../src/r.cpp)
target_link_libraries(test_rounding random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

# Registra i test con CTest
add_test(NAME Polynomial COMMAND test_polynomial)
add_test(NAME PolynomialMatrix COMMAND test_polynomial_matrix)
add_test(NAME Trapgen COMMAND test_trapdoor)
add_test(NAME LPKE COMMAND test_lpke)
add_test(NAME Rounding COMMAND test_rounding)