#ifndef CORECVS_BASIS_H
#define CORECVS_BASIS_H

#include <unordered_set>
#include <queue>

#include "polynom.h"

using std::unordered_set;

namespace corecvs
{

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
            : unordered_set<C, P>(polynoms) {};
    explicit Basis(const std::initializer_list<Polynom<C, P>>& polynoms)
            : unordered_set<C, P>(polynoms) {};

    using unordered_set<Polynom<C, P>>::size;
    using unordered_set<Polynom<C, P>>::insert;
    using unordered_set<Polynom<C, P>>::find;
    using unordered_set<Polynom<C, P>>::begin;
    using unordered_set<Polynom<C, P>>::end;
    using unordered_set<Polynom<C, P>>::cbegin;
    using unordered_set<Polynom<C, P>>::cend;
};

template<typename C, typename P>
bool operator==(const Basis<C, P>& lhs, const Basis<C, P>& rhs) {
    return static_cast<unordered_set<Polynom<C, P>>>(lhs) == static_cast<unordered_set<Polynom<C, P>>>(rhs);
}

template<typename C, typename P>
bool operator!=(const Basis<C, P>& lhs, const Basis<C, P>& rhs) { return !(lhs == rhs); }

template<typename C, typename P>
Polynom<C, P> makeSpolynom(const Polynom<C, P>& f, const Polynom<C, P>& g) {
    const auto fH = f.head(), gH = g.head();
    const auto l = lcm(fH, gH);
    return l / fH * f - l / gH * g;
}

template<typename C, typename P>
Basis<C, P> makeGroebner(const Basis<C, P>& basis) {
    std::unordered_set<Polynom<C, P>> res{basis};
    std::queue<Polynom<C, P>> todo{basis};
    std::vector<Polynom<C, P>> checked{basis.size()};

    while (!todo.empty()) {
        const auto curr = todo.front();
        todo.pop();

        for (const auto& c : checked) {
            if (!areRelativePrimes(curr, c)) {
                const auto s = makeSpolynom(curr, c);
                const auto f = reduceBy(s, res.begin(), res.end());

                if (!f.isZero() && res.find(f) == res.end()) {
                    res.insert(f);
                    todo.push(f);
                }
            }
        }

        checked.push_back(curr);
    }

    return Basis{res};
}

template<typename C, typename P>
Basis<C, P> minimizeGroebner(const Basis<C, P>& groebnerBasis) {
    std::unordered_set<Polynom<C, P>> minimized{};
    for (auto i = groebnerBasis.begin(), lst = groebnerBasis.end(); i != lst; ++i) {
        const auto& p = *i;
        bool isRedundant = false;
        for (auto j = std::next(i); j != lst; ++j) {
            const auto& q = *j;
            if (p.head().isDividableBy(q.head())) {
                isRedundant = true;
                break;
            }
        }

        if (!isRedundant) {
            minimized.insert(p);
        }
    }
    return Basis{minimized};
}

template<typename C, typename P>
Basis<C, P> reduceGroebner(const Basis<C, P>& groebnerBasis) {
    std::unordered_set<Polynom<C, P>> reduced{};
    for (auto i = groebnerBasis.begin(), lst = groebnerBasis.end(); i != lst; ++i) {
        auto p = *i;
        for (const auto& pm : p) { // each monom of polynom p
            for (auto j = std::next(i); j != lst; ++j) {
                const auto& q = *j;
                if (pm.isDividableBy(q)) {
                    p += pm % q - pm;
                }
            }
        }
        reduced.insert(p);
    }
    return Basis{reduced};
}

template<typename C, typename P>
Basis<C, P> minRedGroebner(const Basis<C, P>& groebnerBasis) {
    return reduceGroebner(minimizeGroebner(groebnerBasis));
}

template<typename C, typename P>
bool isMemberOf(const Polynom<C, P>& polynom, const Basis<C, P>& basis, bool isGroebner = false) {
    const auto groebner = isGroebner ? basis : makeGroebner(basis);
    return reduceBy(polynom, groebner.begin(), groebner.end()).isZero();
}

template<typename C, typename P>
bool areEquivalent(const Basis<C, P>& lhs, const Basis<C, P>& rhs) {
    return minRedGroebner(lhs) == minRedGroebner(rhs);
}

}

#endif //CORECVS_BASIS_H
