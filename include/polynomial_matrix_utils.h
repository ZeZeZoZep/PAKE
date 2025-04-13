#ifndef POLYNOMIAL_MATRIX_UTILS_H
#define POLYNOMIAL_MATRIX_UTILS_H

#include <vector>
#include <Eigen/Dense>
#include "polynomial_matrix.h" 

class PolynomialMatrixUtils {
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
};

#endif // POLYNOMIAL_MATRIX_UTILS_H
