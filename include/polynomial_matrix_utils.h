#ifndef POLYNOMIAL_MATRIX_UTILS_H
#define POLYNOMIAL_MATRIX_UTILS_H
#pragma once
#include <vector>
#include <Eigen/Dense>
#include "ideal_cipher.h"
#include "polynomial_matrix.h" 


using namespace boost::multiprecision;
class PolynomialMatrixUtils {
private:
    static cpp_int vector_to_bigint_baseq(const vector<uint16_t>& vec, uint16_t q) {
        cpp_int result = 0;
        cpp_int base = 1;
    
        for (size_t i = 0; i < vec.size(); ++i) {
            result += cpp_int(vec[i]) * base;
            base *= q;
        }
    
        return result;
    }
    static vector<uint16_t> bigint_to_vector_baseq(const cpp_int& value, uint16_t q, size_t expected_length) {
        cpp_int val = value;
        vector<uint16_t> vec;
    
        for (size_t i = 0; i < expected_length; ++i) {
            vec.push_back(static_cast<uint16_t>(val % q));
            val /= q;
        }
    
        return vec;
    }
public:
    // Flatten PolynomialMatrix → vector<uint16_t>
    template<int Rows, int Cols>
    static vector<uint16_t> Encode(const PolynomialMatrix<Rows, Cols>& mat) {
        std::vector<uint16_t> result;
        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Cols; ++j) {
                const auto& poly = mat(i, j);
                result.reserve(result.size() + poly.size());
                for (int k = 0; k < poly.size(); ++k) {
                    result.push_back(static_cast<uint16_t>(poly[k]));
                }
            }
        }
        return result;
    }

    // Unflatten vector<uint16_t> → PolynomialMatrix
    template<int Rows, int Cols>
    static PolynomialMatrix<Rows, Cols> Decode(const std::vector<uint16_t>& input) {
        const int expected = Rows * Cols * PARAM_N;
        if (input.size() != static_cast<size_t>(expected)) {
            cerr << "input: " <<input.size() << endl;
            cerr << "expected: " <<expected << endl;
            throw std::invalid_argument("Input size does not match matrix structure");
        }

        PolynomialMatrix<Rows, Cols> mat;
        int idx = 0;

        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Cols; ++j) {
                Polynomial poly(PARAM_N);
                for (int k = 0; k < PARAM_N; ++k) {
                    poly(k) = static_cast<int>(input[idx++]);
                }
                mat(i, j) = poly;
            }
        }

        return mat;
    }
    template<int Rows, int Cols>
    static vector<uint8_t> Encode_pm(const PolynomialMatrix<Rows, Cols>& input) {
        vector<uint16_t> numbers_array = PolynomialMatrixUtils::Encode(input);
        cpp_int big_number = PolynomialMatrixUtils::vector_to_bigint_baseq(numbers_array, PARAM_Q);
        vector<uint8_t> bytes_array = IdealCipher::cpp_int_to_padded_bytes(big_number,16);
        return bytes_array;
    }
    template<int Rows, int Cols>
    static PolynomialMatrix<Rows, Cols> Decode_pm(const std::vector<uint8_t>& input) {
        cpp_int big_number=IdealCipher::bytes_to_cpp_int(input);
        vector<uint16_t> numbers_array = PolynomialMatrixUtils::bigint_to_vector_baseq(big_number, PARAM_Q, Rows*Cols*PARAM_N);
        PolynomialMatrix<Rows, Cols> polynomial_matrix = PolynomialMatrixUtils::Decode<Rows, Cols>(numbers_array);
        return polynomial_matrix;
    }

    
};

#endif // POLYNOMIAL_MATRIX_UTILS_H
