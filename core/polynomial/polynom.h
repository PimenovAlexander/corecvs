#ifndef CORECVS_POLYNOM_H
#define CORECVS_POLYNOM_H

/**
 * \file polynom.h
 * \date Dec 22, 2019
 * \author Alexander Bashkirov <bash.spbu@gmail.com>
 */

#include "monom.h"

#include <vector>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <cmath>

namespace corecvs
{

/**
 * Represents polynomial of several variables:
 * \f$ c_1 * x_1 ^ p_1_1 * ... * x_k ^ p_{k_1}_1 +
 *     c_2 * x_1 ^ p_1_2 * ... * x_l ^ p_{k_2}_2 +
 *       ...
 *     c_n * x_1 ^ p_1_n * ... * x_l ^ p_{k_n}_n $f\
 *
 * \invariat Implementation details: inner representation is a list
 *           of monomials sorted according to the corecvs::Monom::operator>
 *           i.e. the head of polynomial is the greatest monomial of all.
 *
 * @tparam C A polynomial coefficient type
 * @tparam P A polynomial power type
 */
template<typename C, typename P>
class Polynom : protected std::vector<Monom<C, P>>
{
public:
    Polynom() = default;
    Polynom(const Polynom& that) = default;
    Polynom(Polynom&& that) noexcept = default;
    Polynom& operator=(const Polynom& that) = default;
    Polynom& operator=(Polynom&& that) noexcept = default;

    Polynom(const Monom<C, P>& monom)
            : std::vector<Monom<C, P>>({monom}) {
        if (monom.isZero()) {
            *this = Polynom{};
        }
    };

    Polynom(C number)
            : Polynom({Monom<C, P>(number)}) {};

    explicit Polynom(std::vector<Monom<C, P>> monoms)
            : std::vector<Monom<C, P>>(
            (std::sort(monoms.begin(), monoms.end(), std::greater<Monom<C, P>>()), monoms)) {};

    explicit Polynom(std::initializer_list<Monom<C, P>> monoms)
            : std::vector<Monom<C, P>>(transformInitializerList(monoms)) {};

private:

    /**
     * Sorts the given initializer list of monomials.
     */
    static std::vector<Monom<C, P>> transformInitializerList(const std::initializer_list<Monom<C, P>>& monoms) {
        std::vector<Monom<C, P>> result{monoms};
        std::sort(result.begin(), result.end(), std::greater<Monom<C, P>>());
        return result;
    }

public:

    using std::vector<Monom<C, P>>::size;
    using iterator = typename std::vector<Monom<C, P>>::iterator;
    using const_iterator = typename std::vector<Monom<C, P>>::const_iterator;
    using std::vector<Monom<C, P>>::begin;
    using std::vector<Monom<C, P>>::end;
    using std::vector<Monom<C, P>>::cbegin;
    using std::vector<Monom<C, P>>::cend;

    /**
     * \return The greatest monomial according to the corecvs::Monom::operator>
     * \throw std::invalid_argument If this polynomial is zero
     */
    const Monom<C, P>& head() const {
        if (size() == 0) {
            throw std::invalid_argument("Polynomial is zero, there is no monomial to extract");
        }
        return (*this)[0];
    };

    inline bool isZero() const {
        return size() == 0;
    };

    using std::vector<Monom<C, P>>::operator[];

    Polynom operator+(const Polynom& that) const {
        int lsz = this->size(), rsz = that.size();
        std::vector<Monom<C, P>> result;
        result.reserve(std::max(lsz, rsz));

        for (auto lp = begin(), rp = that.begin(), le = end(), re = that.end(); lp != le || rp != re; ) {
            if (lp != le && rp != re && (*lp).isSimilarTo(*rp)) {
                const auto m = *(lp++) + *(rp++);
                if (!m.isZero()) {
                    result.push_back(m);
                }
            } else if (rp == re || (lp != le && *lp > *rp)) {
                result.push_back(*(lp++));
            } else {
                result.push_back(*(rp++));
            }
        }
        return Polynom{result};
    };

    inline Polynom operator-(const Polynom& that) const {
        return *this + that * static_cast<C>(-1);
    }

    Polynom& operator*=(const Monom<C, P>& monom) {
        if (monom.isZero()) {            //  improves the accuracy when using non-exact
            return (*this) = Polynom{};  //  machine types (like float and double)
        }                                //  for coefficients
        std::vector<Monom<C, P>> result;
        result.reserve(size());
        for (const auto& it : *this) {
            const auto t = it * monom;
            if (!t.isZero()) {
                result.push_back(t);
            }
        }
        return *this = Polynom{result};
    };

    Polynom operator*(const Polynom& that) const {
        Polynom result{};
        for (const auto& it : that) {
            const auto t = (*this) * it;
            if (!t.isZero()) {
                result += t;
            }
        }
        return result;
    };

    inline bool isDividableBy(const Polynom& that) const {
        return isZero() || (!that.isZero() && head().isDividableBy(that.head()));
    };

    inline bool isReducibleBy(const Polynom& that) const { return isDividableBy(that); }

    Polynom singleReduceBy(const Polynom& that) const {
        if (that.isZero()) {
            throw std::invalid_argument("Division by a zero polynomial");
        }
        if (isZero()) {
            return Polynom{};
        }
        return (*this) - head() / that.head() * that;
    }

    /**
     * Reduces this by given polynomials while it is possible.
     */
    template<typename Iter>
    Polynom reduceBy(Iter begin, Iter end) const {
        bool wasReduced = true;
        auto p = *this;
        while (!p.isZero() && wasReduced) {
            wasReduced = false;
            for (Iter i = begin; i != end; ++i) {
                auto q = *i;
                if (p.isReducibleBy(q)) {
                    p = p.singleReduceBy(q);
                    wasReduced = true;
                }
            }
        }
        return p;
    }

    Polynom operator/(const Polynom& that) const {
        bool wasReduced = true;
        auto p = *this;
        const auto thatHead = that.head();
        Polynom result{};
        while (!p.isZero() && wasReduced) {
            wasReduced = false;
            if (p.isReducibleBy(that)) {
                auto divisor = p.head() / thatHead;
                result = result + divisor;
                p = p - divisor * that;
                wasReduced = true;
            }
        }
        return result;
    }

    Polynom operator%(const Polynom& that) const {
        if (isZero()) {
            return *this;
        }
        std::vector<Polynom> tmp{that};
        return reduceBy(tmp.begin(), tmp.end());
    };

    /**
     * Format example: x1^2 + x1*x2 + x2*x3^2
     * Format example: 0
     */
    template<typename C1, typename P1>
    friend std::ostream& operator<<(std::ostream& os, const Polynom<C1, P1>& polynom);

    template<typename C1, typename P1>
    friend bool operator==(const Polynom<C1, P1>& lhs, const Polynom<C1, P1>& rhs);

    template<typename T>
    friend
    struct std::hash;
};


template<typename C, typename P>
inline Polynom<C, P>& operator+=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs + rhs; }

template<typename C, typename P>
inline Polynom<C, P>& operator-=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs - rhs; }

template<typename C, typename P>
inline Polynom<C, P> operator*(Polynom<C, P> polynom, const Monom<C, P>& monom) { return polynom *= monom; }

template<typename C, typename P, typename N>
inline Polynom<C, P> operator*(const Polynom<C, P>& polynom, N number) { return polynom * Monom<C, P>{number}; }

template<typename C, typename P, typename N>
inline Polynom<C, P> operator*(N number, const Polynom<C, P>& polynom) { return operator*(polynom, number); }

template<typename C, typename P>
inline Polynom<C, P> operator*(const Monom<C, P>& monom, const Polynom<C, P>& polynom) { return polynom * monom; }

template<typename C, typename P>
inline Polynom<C, P>& operator*=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs * rhs; }

template<typename C, typename P>
inline Polynom<C, P>& operator/=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs / rhs; }

template<typename C, typename P>
inline Polynom<C, P>& operator%=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs % rhs; }

template<typename C, typename P>
inline Polynom<C, P> operator^(Polynom<C, P> polynom, P power) { return polynom ^= power; }

/**
 * Implements efficient binary power algorithm.
 */
template<typename C, typename P>
Polynom<C, P>& operator^=(Polynom<C, P>& result, P power) {
    Polynom<C, P> t{result};
    result = Polynom<C, P>{static_cast<C>(1)};
    while (power > 0) {
        if (power & 1) {
            result *= t;
        }
        t *= t;
        power >>= 1;
    }
    return result;
}

template<typename C, typename P>
inline bool operator==(const Polynom<C, P>& lhs, const Polynom<C, P>& rhs) {
    return static_cast<const std::vector<corecvs::Monom<C, P>>&>(lhs) ==
           static_cast<const std::vector<corecvs::Monom<C, P>>&>(rhs);
}

template<typename C, typename P>
inline bool operator!=(const Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return !(lhs == rhs); }

template<typename C, typename P>
std::ostream& operator<<(std::ostream& os, const Polynom<C, P>& polynom) {
    if (polynom.isZero()) {
        os << "0";
    } else {
        for (auto i = polynom.begin(), preLast = std::prev(polynom.end()); i != preLast; i++) {
            os << *i << " + ";
        }
        os << *(std::prev(polynom.end()));
    }
    return os;
}

} // end corecvs

namespace std
{

template<typename C, typename P>
struct hash<corecvs::Polynom<C, P>>
{
    std::size_t operator()(const corecvs::Polynom<C, P>& p) const noexcept {
        std::size_t h = 17;
        const hash<corecvs::Monom<C, P>> monomHasher;
        for (auto& m : p) {
            h = (h + monomHasher(m)) * 31 % 10000019;
        }
        return h;
    };
};

} // end std


#endif //CORECVS_POLYNOM_H
