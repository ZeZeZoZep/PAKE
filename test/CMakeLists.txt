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

add_executable(test_polynomial_matrix test_polynomial_matrix.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/ideal_cipher.cpp ../src/timerset.cpp)
target_link_libraries(test_polynomial_matrix random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

#add_executable(test_trapdoor test_trapdoor.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp)
#target_link_libraries(test_trapdoor random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

#add_executable(test_lpke test_lpke.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp ../src/lpke.cpp ../src/r.cpp  ../src/hash.cpp ../src/ideal_cipher.cpp)
#target_link_libraries(test_lpke random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

#add_executable(test_rounding test_rounding.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp  ../src/r.cpp)
#target_link_libraries(test_rounding random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_pke test_pke.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/random.cpp ../src/hash.cpp ../src/pke.cpp ../src/timerset.cpp)
target_link_libraries(test_pke random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_kem test_kem.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/random.cpp ../src/hash.cpp ../src/pke.cpp ../src/kem.cpp ../src/timerset.cpp)
target_link_libraries(test_kem random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_2feistel test_2feistel.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/random.cpp ../src/hash.cpp ../src/pke.cpp ../src/kem.cpp ../src/timerset.cpp)
target_link_libraries(test_2feistel random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_ideal_cipher test_ideal_cipher.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/ideal_cipher.cpp ../src/timerset.cpp)
target_link_libraries(test_ideal_cipher gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_pake test_pake.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/ideal_cipher.cpp ../src/random.cpp ../src/hash.cpp ../src/pke.cpp ../src/kem.cpp ../src/ideal_cipher_pake.cpp ../src/timerset.cpp)
target_link_libraries(test_pake random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

add_executable(test_noic test_noic.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/random.cpp ../src/ideal_cipher.cpp ../src/hash.cpp ../src/pke.cpp ../src/kem.cpp ../src/noic.cpp ../src/timerset.cpp)
target_link_libraries(test_noic random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)
#add_executable(test_rom_pake test_rom_pake.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/ideal_cipher.cpp ../src/hash.cpp ../src/pke.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp ../src/lpke.cpp)
#target_link_libraries(test_rom_pake random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

#add_executable(test_karatsuba test_karatsuba.cpp ../src/int.cpp ../src/ntt.cpp ../src/polynomial.cpp ../src/trapdoor_handler.cpp ../src/random.cpp ../src/babai.cpp ../src/lpke.cpp ../src/r.cpp  ../src/hash.cpp)
#target_link_libraries(test_karatsuba random_mgs gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)


# Registra i test con CTest
add_test(NAME Polynomial COMMAND test_polynomial)
add_test(NAME PolynomialMatrix COMMAND test_polynomial_matrix)
#add_test(NAME Trapgen COMMAND test_trapdoor)
#add_test(NAME LPKE COMMAND test_lpke)
#add_test(NAME Rounding COMMAND test_rounding)
add_test(NAME PKE COMMAND test_pke)
add_test(NAME KEM COMMAND test_kem)
add_test(NAME IdealCipher COMMAND test_ideal_cipher)
add_test(NAME 2Feistel COMMAND test_2feistel)
add_test(NAME PAKE COMMAND test_pake)
add_test(NAME NoIC COMMAND test_noic)
#add_test(NAME ROMPAKE COMMAND test_rom_pake)
#add_test(NAME Karatsuba COMMAND test_karatsuba)