#ifndef CORECVS_FINITEFIELD_H
#define CORECVS_FINITEFIELD_H

/**
 * \file finiteField.h
 * \date Dec 22, 2019
 * \author Alexander Bashkirov <bash.spbu@gmail.com>
 */

#include <array>
#include <ostream>

namespace corecvs
{

/**
 * Represents an element of the multiplicative group of integers modulo *prime* M
 * (which forms a finite field of M elements: 0, 1, ..., M - 1).
 *
 * \note Implicit conversions from numeric types are _forbidden_ to avoid
 *       some unpleasant bugs, such as
 *       \code
 *         int a = -1;
 *         FiniteField<unsigned int, 5> = a; // logically should be 6
 *         std::cout << v << std::endl;      // 3 ?! (platform dependent)
 *       \endcode
 *
 * \note Implicit conversions to types `long long` and `double` are provided.
 *
 * \tparam T Machine type representing elements. Can be `unsigned`.
 * \tparam M Modulo of the group. The following *conditions must be satisfied*:
 *           - M is prime
 *           - 2 * (M - 1) <= T::max_value
 *           - (M - 1) ^ 2 <= T::max_value
 */
template<typename T, T M>
class FiniteField
{
public:
    FiniteField() = default;
    FiniteField(const FiniteField& that) = default;
    FiniteField(FiniteField&& that) noexcept = default;
    FiniteField& operator=(FiniteField&& that) noexcept = default;
    FiniteField& operator=(const FiniteField& that) = default;

    explicit FiniteField(T init)
            : representative(init < 0 ? (init % M + M) : (init % M)) {};

    FiniteField& operator+=(const FiniteField& that) {
        representative += that.representative;
        if (representative >= M) {
            representative -= M;
        }
        return *this;
    };

    FiniteField& operator-=(const FiniteField& that) {
        if (representative < that.representative) {
            representative += M;
        }
        representative -= that.representative;
        return *this;
    };

    FiniteField& operator*=(const FiniteField& that) {
        representative = representative * that.representative % M;
        return *this;
    };

    FiniteField& operator/=(const FiniteField& that) {
        representative = representative * inverses[that.representative] % M;
        return *this;
    };

    inline bool operator==(const FiniteField& rhs) const {
        return representative == rhs.representative;
    }

    inline bool operator!=(const FiniteField& rhs) const {
        return representative != rhs.representative;
    }

    inline explicit operator double() const {
        return static_cast<double>(representative);
    }

    inline explicit operator long long() const {
        return static_cast<double>(representative);
    }

    friend std::ostream& operator<<(std::ostream& os, const FiniteField& field) {
        return os << field.representative;
    }

    template<typename H>
    friend struct hash;

private:
    /**
     * Linear computation of all inverses.
     *
     * \return r such as r[i] is a inverse element for i
     * \see [in russian] https://e-maxx.ru/algo/reverse_element#4
     */
    static std::array<T, M> initialization() {
        std::array<T, M> r;
        r[1] = 1;
        for (int i = 2; i < M; ++i) {
            r[i] = M - (M / i) * r[M % i] % M;
        }
        return r;
    }

    static const std::array<T, M> inverses;

    T representative;
};

template<typename T, T M>
const std::array<T, M> FiniteField<T, M>::inverses = FiniteField<T, M>::initialization();

template<typename T, T M>
inline FiniteField<T, M> operator+(FiniteField<T, M> lhs, const FiniteField<T, M>& rhs) { return lhs += rhs; }

template<typename T, T M>
inline FiniteField<T, M> operator-(FiniteField<T, M> lhs, const FiniteField<T, M>& rhs) { return lhs -= rhs; }

template<typename T, T M>
inline FiniteField<T, M> operator*(FiniteField<T, M> lhs, const FiniteField<T, M>& rhs) { return lhs *= rhs; }

template<typename T, T M>
inline FiniteField<T, M> operator/(FiniteField<T, M> lhs, const FiniteField<T, M>& rhs) { return lhs /= rhs; }

} // end of corecvs

namespace std
{

template<typename T, T M>
struct hash<corecvs::FiniteField<T, M>>
{
    size_t operator()(corecvs::FiniteField<T, M> e) const noexcept {
        return hash<T>()(e.representative);
    };
};

} // end of std

#endif //CORECVS_FINITEFIELD_H
