#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
void matrix_print(std::vector<uint16_t> matrix, uint16_t d,uint16_t h,uint16_t k);
void poly_print(std::vector<uint16_t> poly, uint16_t k);
#endif