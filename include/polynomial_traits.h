#ifndef POLYNOMIAL_TRAITS_H
#define POLYNOMIAL_TRAITS_H

#include <Eigen/Core>
#include "polynomial.h"

namespace Eigen {
template<>
struct NumTraits<Polynomial> : GenericNumTraits<Polynomial> {
    typedef Polynomial Real;
    typedef Polynomial NonInteger;
    typedef Polynomial Nested;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 1,
        MulCost = 1
    };

    static inline Polynomial epsilon() { return Polynomial(1); }   // Evita errore su epsilon()
    static inline Polynomial dummy_precision() { return Polynomial(1); }
    static inline Polynomial highest() { return Polynomial(1); }
    static inline Polynomial lowest() { return Polynomial(0); }
    static inline Polynomial infinity() { return Polynomial(1); }
    static inline Polynomial quiet_NaN() { return Polynomial(0); }

    // Disabilitiamo log10()
    static constexpr int digits10() { return 0; } // Eigen non deve usare log10()
};
} // namespace Eigen

#endif // POLYNOMIAL_TRAITS_H

