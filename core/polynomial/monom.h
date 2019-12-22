#ifndef CORECVS_MONOM_H
#define CORECVS_MONOM_H

/**
 * \file monom.h
 * \date Dec 22, 2019
 * \author Alexander Bashkirov <bash.spbu@gmail.com>
 */

#include <vector>
#include <algorithm>
#include <ostream>
#include <sstream>

namespace corecvs
{

/**
 * Represents monomial of several variables:
 * \f$ coefficient * x_1 ^ p_1 * ... * x_n ^ p_n $f\
 *
 * \warning Implementation caveat: when using with non-exact machine types
 *          like float and double for coefficient type, the monomial can be zero
 *          when its powers are not! E.g. `0.000001 * x1^1000000` will be treated like zero,
 *          if `0.000001` is smaller than corecvs::Monom::PRECISION
 *
 * \tparam C A monomial coefficient type
 * \tparam P A monomial power type
 */
template<typename C, typename P>
class Monom
{
public:
    /**
     * Small delta for coefficient non-exact machine type comparison.
     *
     * Probably it is not very good way to store it like this, may be refactored later.
     */
    constexpr static const double PRECISION = 1e-3;

    static inline bool equal(C c1, C c2) {
        return std::abs(static_cast<double>(c1 - c2)) < PRECISION;
    }

    Monom() = default;
    Monom(const Monom& that) = default;
    Monom(Monom&& that) noexcept = default;
    Monom& operator=(Monom&& that) noexcept = default;
    Monom& operator=(const Monom& that) = default;

    Monom(const C& coeff, const std::vector<P>& powers)
            : coeff(coeff)
            , powers(powers) {};

    explicit Monom(const C& coeff)
            : Monom(coeff, std::vector<P>{}) {}

    Monom(const C& coeff, const std::initializer_list<P>& powers)
            : coeff(coeff)
            , powers(powers) {};

    inline bool isZero() const { return equal(coeff, static_cast<C>(0)); };

private:

    /**
     * For the sake of efficiency after arithmetic operations
     * following zero powers are not dropped, so for example
     * the internal representation of powers may be of kind `0,1,1,0,0`.
     * This function returns a pointer to the last non-zero powers in the vector, e.g.
     *      |
     *      v
     * `0,1,1,0,0`
     *
     * \note Consider use it everywhere instead of `powers.end()`
     */
    typename std::vector<P>::const_iterator powersEnd() const {
        return powers.begin() + std::distance(
                std::find_if(powers.rbegin(), powers.rend(), [](const P& it) { return it != static_cast<P>(0); }),
                powers.rend()
        );
    }

    /**
     * Checks that ranges `[first1, last1)` and `[first2, last2)` are equal by-element.
     */
    template<typename InputIt1, typename InputIt2>
    static bool equalRanges(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (*first1 != *first2) {
                return false;
            }
        }
        return first1 == last1 && first2 == last2;
    }

public:

    /**
     * Checks that all powers are zero.
     */
    inline bool isConstant() const {
        return isZero() || powers.begin() == powersEnd();
    }

    /**
     * Checks that this and given monomials have the same variables and their powers lists.
     *
     * E.g. `true`  for \f$ -3 * x_1 ^ 1 * x_3 ^ 2 $f\ and \f$ 42 * x_1 ^ 1 * x_3 ^ 2 $f\,
     *      `false` for \f$ -3 * x_1 ^ 1 * x_3 ^ 2 $f\ and \f$ 42 * x_1 ^ 1 * x_2 * x_3 ^ 2 $f\,
     */
    bool isSimilarTo(const Monom& that) const {
        if (isZero()) {
            return that.isConstant();
        }
        return !that.isZero() && Monom::equalRanges(powers.begin(), powersEnd(), that.powers.begin(), that.powersEnd());
    };

    /**
     * \throws std::invalid_argument When this and given monomials are not similar
     */
    Monom& operator+=(const Monom& that) {
        if (!isSimilarTo(that)) {
            throw std::invalid_argument("It is impossible to sum non-similar monomials");
        }
        coeff += that.coeff;
        return *this;
    };

    /**
     * \throws std::invalid_argument When this and given monomials are not similar
     */
    Monom& operator-=(const Monom& that) {
        if (!isSimilarTo(that)) {
            throw std::invalid_argument("It is impossible to subtract non-similar monomial");
        }
        coeff -= that.coeff;
        return *this;
    };

    Monom& operator*=(const Monom& that) {
        if (that.isZero()) {
            return *this = Monom{};
        }
        coeff *= that.coeff;
        if (that.powersEnd() - that.powers.begin() > powers.size()) {
            powers.resize(that.powers.size(), 0);
        }
        std::transform(that.powers.begin(), that.powersEnd(), powers.begin(), powers.begin(), std::plus<P>{});
        return *this;
    };

    template<typename N>
    Monom& operator*=(N number) {
        if (equal(number, static_cast<C>(0))) {
            return *this = Monom{};
        }
        coeff *= number;
        return *this;
    };

    /**
     * Checks that this monomial can be divided by given monomial.
     */
    bool isDividableBy(const Monom& that) const {
        if (that.isZero()) {
            return false;
        }
        if (this->isZero()) {
            return true;
        }
        auto l = powers.begin(), le = powersEnd(), r = that.powers.begin(), re = that.powersEnd();
        for (; l != le && r != re; ++l, ++r) {
            if (*l < *r) {
                return false;
            }
        }
        return r == re;
    };

    /**
     * \throws std::invalid_argument When this monomial is not dividable by the given one
     */
    Monom& operator/=(const Monom& that) {
        if (!isDividableBy(that)) {
            throw std::invalid_argument("This monomial is not dividable by the given one");
        }
        if (isZero()) {
            return *this;
        }
        coeff /= that.coeff;
        std::transform(that.powers.begin(), that.powersEnd(), powers.begin(), powers.begin(),
                       [](P p1, P p2) { return p2 - p1; });
        return *this;
    };

    C getCoeff() const { return coeff; };
    const std::vector<P>& getPowers() const { return powers; };

    /**
     * Format example: '3x1^2*x3' for coeff = 3, powers == [2,0,1]
     */
    template<typename C1, typename P1>
    friend std::ostream& operator<<(std::ostream& os, const Monom<C1, P1>& monom);
    template<typename C1, typename P1>
    friend bool operator<(const Monom<C1, P1>& lhs, const Monom<C1, P1>& rhs);
    template<typename T>
    friend
    struct std::hash;

private:
    C coeff;
    std::vector<P> powers;
};

/**
 * \throws std::invalid_argument When this and given monomials are not similar
 */
template<typename C, typename P>
inline Monom<C, P> operator+(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs += rhs; }

/**
 * \throws std::invalid_argument When this and given monomials are not similar
 */
template<typename C, typename P>
inline Monom<C, P> operator-(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs -= rhs; }

template<typename C, typename P>
inline Monom<C, P> operator*(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs *= rhs; }

template<typename C, typename P, typename N>
inline Monom<C, P> operator*(Monom<C, P> monom, N number) { return monom *= number; }

template<typename C, typename P, typename N>
inline Monom<C, P> operator*(N number, const Monom<C, P>& monom) { return monom * number; }

/**
 * \throws std::invalid_argument When this monomial is not dividable by the given one
 */
template<typename C, typename P>
Monom<C, P> operator/(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs /= rhs; }

/**
 * \note For coefficients comparison uses corecvs::Monom::PRECISION
 */
template<typename C, typename P>
inline bool operator==(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
    return Monom<C, P>::equal(lhs.getCoeff(), rhs.getCoeff()) && lhs.isSimilarTo(rhs);
}

template<typename C, typename P>
inline bool operator!=(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return !(lhs == rhs); }

/*
 * Implements the monomial lexicographical order.
 * E.g. 0 < x2 < x1 < x1 * x2 < x1 * x2 ^ 2 < x1 ^ 2 < x1 ^ 2 * x2
 */
template<typename C, typename P>
inline bool operator<(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
    return lhs.isZero() ?
           !rhs.isConstant() :
           std::lexicographical_compare(lhs.powers.begin(), lhs.powersEnd(), rhs.powers.begin(), rhs.powersEnd());
}

template<typename C, typename P>
inline bool operator>(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return rhs < lhs; }

template<typename C, typename P>
inline bool operator<=(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return !(rhs < lhs); }

template<typename C, typename P>
inline bool operator>=(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return !(lhs < rhs); }

template<typename C, typename P>
std::ostream& operator<<(std::ostream& os, const Monom<C, P>& monom) {
    os << monom.coeff;
    int printedCount = 0;
    for (int i = 0; i < monom.powers.size(); ++i) {
        std::string intercalate = printedCount ? "*" : "";
        if (monom.powers[i] == 0) {
            continue;
        } else if (monom.powers[i] == 1) {
            ++printedCount;
            os << intercalate << 'x' << (i + 1);
        } else {
            ++printedCount;
            os << intercalate << 'x' << (i + 1) << '^' << monom.powers[i];
        }
    }
    return os;
}

/**
 * Computes the least common multiple of given monomials.
 *
 * \throws std::invalid_argument When one of the given monomials is zero
 */
template<typename C, typename P>
Monom<C, P> lcm(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
    if (lhs.isZero()) {
        throw std::invalid_argument("lhs is zero");
    }
    if (rhs.isZero()) {
        throw std::invalid_argument("rhs is zero");
    }
    std::vector<P> resPowers{lhs.getPowers().size() >= rhs.getPowers().size() ? lhs.getPowers() : rhs.getPowers()};

    for (int i = 0, sz = std::min(lhs.getPowers().size(), rhs.getPowers().size()); i < sz; ++i) {
        resPowers[i] = std::max(lhs.getPowers()[i], rhs.getPowers()[i]);
    }
    return Monom<C, P>{lhs.getCoeff() * rhs.getCoeff(), resPowers};
}

template<typename C, typename P>
inline bool areRelativePrimes(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
    return lcm(lhs, rhs) == lhs * rhs;
}

} // end corecvs

namespace std
{

template<typename C, typename P>
struct hash<corecvs::Monom<C, P>>
{
    size_t operator()(const corecvs::Monom<C, P>& m) const noexcept {
        size_t h = 17;
        const hash<P> pHash;
        h = (h + hash<long long int>()(static_cast<long long int>(m.getCoeff()))) * 31 % 10000019;
        if (m.isZero()) {
            return h;
        }
        for (auto i = m.powers.begin(), lst = m.powersEnd(); i < lst; ++i) {
            h = (h + pHash(*i)) * 31 % 10000019;
        }
        return h;
    };
};

} // end std

#endif //CORECVS_MONOM_H
