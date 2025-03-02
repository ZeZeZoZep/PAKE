cmake_minimum_required(VERSION 3.16)
project(Pake)

set(CMAKE_CXX_STANDARD 17)  # Usa C++17 o successivo
# Trova OpenSSL
find_package(OpenSSL REQUIRED)
# Aggiungi la directory include/
include_directories(include)

# Aggiungi i file sorgenti
add_executable(my_project
    src/test.cpp
    src/ntt.cpp
    src/int.cpp
    src/random.cpp
)

# Aggiungi i test
target_link_libraries(my_project OpenSSL::SSL OpenSSL::Crypto)
enable_testing()
add_subdirectory(test)