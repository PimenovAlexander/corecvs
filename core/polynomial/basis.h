#ifndef CORECVS_BASIS_H
#define CORECVS_BASIS_H

/**
 * \file basis.h
 * \date Dec 22, 2019
 * \author Alexander Bashkirov <bash.spbu@gmail.com>
 */

#include <unordered_set>
#include <queue>
#include <ostream>

#include "polynom.h"

using std::unordered_set;

namespace corecvs
{

/**
 * A set of polynomials, which forms a basis of some ideal.
 *
 * \tparam C A polynomial coefficient type
 * \tparam P A polynomial power type
 * \see corecvs::Polynom
 */
template<typename C, typename P>
class Basis : protected unordered_set<Polynom<C, P>>
{
public:
    Basis() = default;
    Basis(const Basis& that) = default;
    Basis(Basis&& that) noexcept = default;
    Basis& operator=(const Basis& that) = default;
    Basis& operator=(Basis&& that) noexcept = default;

    explicit Basis(const unordered_set<Polynom<C, P>>& polynoms)
            : unordered_set<Polynom<C, P>>(polynoms) {};
    explicit Basis(const std::initializer_list<Polynom<C, P>>& polynoms)
            : unordered_set<Polynom<C, P>>(polynoms) {};

    using iterator = typename unordered_set<Polynom<C, P>>::iterator;
    using const_iterator = typename unordered_set<Polynom<C, P>>::const_iterator;
    using unordered_set<Polynom<C, P>>::size;
    using unordered_set<Polynom<C, P>>::insert;
    using unordered_set<Polynom<C, P>>::find;
    using unordered_set<Polynom<C, P>>::begin;
    using unordered_set<Polynom<C, P>>::end;
    using unordered_set<Polynom<C, P>>::cbegin;
    using unordered_set<Polynom<C, P>>::cend;

    template<typename C1, typename P1>
    friend bool operator==(const Basis<C1, P1>& lhs, const Basis<C1, P1>& rhs);
    template<typename C1, typename P1>
    friend std::ostream& operator<<(std::ostream& os, const Basis<C1, P1>& basis);
};

/**
 * Implements by-element equality comparison.
 * For check that two basises represent the same ideal use corecvs::areEquivalent
 *
 * \see corecvs::areEquivalent
 */
template<typename C, typename P>
inline bool operator==(const Basis<C, P>& lhs, const Basis<C, P>& rhs) {
    return static_cast<const unordered_set<Polynom<C, P>>&>(lhs) ==
           static_cast<const unordered_set<Polynom<C, P>>&>(rhs);
}

/**
 * Format: "{polynom1, ..., polynom_N}"
 */
template<typename C, typename P>
std::ostream& operator<<(std::ostream& os, const Basis<C, P>& basis) {
    os << "{";
    std::copy(basis.begin(), std::prev(basis.end()), std::ostream_iterator<Polynom<C, P>>(os, ", "));
    os << *std::prev(basis.end());
    os << "}";
    return os;
}

template<typename C, typename P>
inline bool operator!=(const Basis<C, P>& lhs, const Basis<C, P>& rhs) { return !(lhs == rhs); }

template<typename C, typename P>
Polynom<C, P> makeSPolynom(const Polynom<C, P>& f, const Polynom<C, P>& g) {
    const auto fH = f.head(), gH = g.head();
    const auto l = lcm(fH, gH);
    return l / fH * f - l / gH * g;
}

/**
 * Computes Groebner basis of the given basis ideal by the Buchberger algorithm.
 *
 * \note Uses _monomial order_ defined by corecvs::Monom::operator<
 *
 * \note Buchberger algorithm has very hard complexity
 *       https://en.wikipedia.org/wiki/Buchberger%27s_algorithm#Complexity
 *
 * \warning With _unprecise_ machine types such as float and double
 *          representing polynomials coefficients an algorithm may *diverge*.
 *          Be careful and use _exact_ types such as corecvs::FiniteField
 *
 * \see corecvs::FiniteField
 * \see [in russian] https://www.mccme.ru/free-books/dubna/arjantsev.pdf
 */
template<typename C, typename P>
Basis<C, P> makeGroebner(const Basis<C, P>& basis) {
    std::unordered_set<Polynom<C, P>> res{basis.begin(), basis.end()};
    std::queue<Polynom<C, P>> todo{std::deque<Polynom<C, P>>{basis.begin(), basis.end()}};
    std::vector<Polynom<C, P>> checked{};
    checked.reserve(basis.size());

    while (!todo.empty()) {
        const auto curr = todo.front();
        todo.pop();
        for (const auto& c : checked) {
            if (!areRelativePrimes(curr.head(), c.head())) {
                const auto s = makeSPolynom(curr, c);
                const auto f = s.reduceBy(res.begin(), res.end());

                if (!f.isZero() && res.find(f) == res.end()) {
                    res.insert(f);
                    todo.push(f);
                }
            }
        }
        checked.push_back(curr);
    }
    return Basis<C, P>{res};
}

/**
 * Minimizes given *Groebner* basis to the minimal amount of elements
 * (preserving property of being Groebner basis),
 * by possibly discarding some polynomials in the given Groebner basis.
 *
 * \note Only *Groebner* basis as an argument makes sense.
 */
template<typename C, typename P>
Basis<C, P> minimizeGroebner(const Basis<C, P>& groebnerBasis) {
    std::unordered_set<Polynom<C, P>> minimized{};
    for (auto i = groebnerBasis.begin(), lst = groebnerBasis.end(); i != lst; ++i) {
        const auto& p = *i;
        bool isRedundant = false;
        for (auto j = groebnerBasis.begin(); j != lst; ++j) {
            if (j != i) {
                if (p.head().isDividableBy((*j).head())) {
                    isRedundant = true;
                    break;
                }
            }
        }
        if (!isRedundant) {
            minimized.insert(p);
        }
    }
    return Basis<C, P>{minimized};
}

/**
 * Possibly simplifies each polynomial of the *Groebner basis*.
 *
 * \note Only *Groebner* basis as an argument makes sense.
 */
template<typename C, typename P>
Basis<C, P> reduceGroebner(const Basis<C, P>& groebnerBasis) {
    std::unordered_set<Polynom<C, P>> reduced{};
    for (auto i = groebnerBasis.begin(), lst = groebnerBasis.end(); i != lst; ++i) {
        auto p{*i};

        tryMore:
        for (const auto& _pm : p) { // each monom of polynom p
            const Polynom<C, P> pm{_pm};
            for (auto j = groebnerBasis.begin(); j != lst; ++j) {
                if (j != i) {
                    const auto& q = *j;
                    if (pm.isDividableBy(q)) {
                        p += pm % q - pm;
                        goto tryMore;
                    }
                }
            }
        }
        reduced.insert(p);
    }
    return Basis<C, P>{reduced};
}

/**
 * Normalizes each polynomial of the given basis,
 * dividing it by the head monomial coefficient.
 */
template<typename C, typename P>
Basis<C, P> normalize(const Basis<C, P>& basis) {
    std::unordered_set<Polynom<C, P>> result{};
    for (const auto& p : basis) {
        if (!p.isZero()) {
            result.insert(p / p.head().getCoeff());
        }
    }
    return Basis<C, P>{result};
}

/**
 * Computes minimal reduced Groebner basis.
 *
 * \note There exists only *one* normalized minimal reduced Groebner basis
 *       with respect to free coefficient multiplier.
 */
template<typename C, typename P>
inline Basis<C, P> makeMinRedGroebner(const Basis<C, P>& basis) {
    return reduceGroebner(minimizeGroebner(makeGroebner(basis)));
}

/**
 * Computes normalized minimal reduced Groebner basis.
 *
 * \note There exists only *one* normalized minimal reduced Groebner basis.
 * \see corecvs::areEquivalent
 */
template<typename C, typename P>
inline Basis<C, P> makeNormalizedMinRedGroebner(const Basis<C, P>& basis) {
    return normalize(reduceGroebner(minimizeGroebner(makeGroebner(basis))));
}

/**
 * Checks whether given polynomial belongs to the ideal of the given basis.
 *
 * \param isGroebner (default false) If true than considers given basis as a Groebner one,
 *                   otherwise it computes Groebner basis by the given one
 */
template<typename C, typename P>
inline bool isMemberOf(const Polynom<C, P>& polynom, const Basis<C, P>& basis, bool isGroebner = false) {
    const auto groebner = isGroebner ? basis : makeGroebner(basis);
    return reduceBy(polynom, groebner.begin(), groebner.end()).isZero();
}

/**
 * Checks whether given basises represent the same ideal.
 */
template<typename C, typename P>
inline bool areEquivalent(const Basis<C, P>& lhs, const Basis<C, P>& rhs) {
    return makeNormalizedMinRedGroebner(lhs) == makeNormalizedMinRedGroebner(rhs);
}

}

#endif //CORECVS_BASIS_H
