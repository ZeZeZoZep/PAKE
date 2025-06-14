cmake_minimum_required(VERSION 3.16)
project(Pake)
# Abilita il supporto per C e C++
enable_language(C CXX)

set(CMAKE_CXX_STANDARD 17)  # Usa C++17 o successivo
# Trova OpenSSL
find_package(OpenSSL REQUIRED)
# Trova Boost con i moduli necessari
find_package(Boost REQUIRED COMPONENTS system thread)
# Include le directory di Boost
include_directories(${Boost_INCLUDE_DIRS})

# Trova Eigen3
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR}) 

# Aggiungi la directory include/
include_directories(include)

# Specifica il file C da compilare separatamente
set(C_FILE src/random_mgs.c)  # Sostituisci con il file C effettivo

# Specifica i file C++ da compilare
set(CPP_FILES 
    src/timerset.cpp
    src/polynomial.cpp
    src/ntt.cpp
    src/int.cpp
    src/random.cpp
    src/babai.cpp
    src/trapdoor_handler.cpp
    src/lpke.cpp
    src/pke.cpp
    src/hash.cpp
    src/kem.cpp
    src/ideal_cipher.cpp
    src/ideal_cipher_pake.cpp
    src/noic.cpp

    #main.cpp
)
add_library(random_mgs OBJECT ${C_FILE})
target_compile_options(random_mgs PRIVATE -Wall -O3 -march=native -mtune=native -fomit-frame-pointer -ftree-vectorize -funsafe-math-optimizations -mfpmath=sse -mavx2 -ftree-vectorize -maes ) 

# Crea l'eseguibile e collega il file C compilato
#add_executable(my_project $<TARGET_OBJECTS:random_mgs> ${CPP_FILES})

# Aggiungi i test
#target_link_libraries(my_project OpenSSL::SSL OpenSSL::Crypto)

enable_testing()
add_subdirectory(test)

# ---- WebSocket Server ----
add_executable(pake_server server.cpp $<TARGET_OBJECTS:random_mgs> ${CPP_FILES})
target_link_libraries(pake_server Boost::system OpenSSL::SSL OpenSSL::Crypto)

# ---- WebSocket Client ----
add_executable(pake_client client.cpp $<TARGET_OBJECTS:random_mgs> ${CPP_FILES})
target_link_libraries(pake_client Boost::system OpenSSL::SSL OpenSSL::Crypto)


# ---- WebSocket Server ----
add_executable(noic_server server_noic.cpp $<TARGET_OBJECTS:random_mgs> ${CPP_FILES})
target_link_libraries(noic_server Boost::system OpenSSL::SSL OpenSSL::Crypto)

# ---- WebSocket Client ----
add_executable(noic_client client_noic.cpp $<TARGET_OBJECTS:random_mgs> ${CPP_FILES})
target_link_libraries(noic_client Boost::system OpenSSL::SSL OpenSSL::Crypto)