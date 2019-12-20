#ifndef CORECVS_POLYNOM_H
#define CORECVS_POLYNOM_H

#include "monom.h"

#include <vector>
#include <ostream>
#include <iterator>


namespace corecvs
{

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
            : std::vector<Monom<C, P>>({monom}) {};

    Polynom(C number)
            : std::vector<Monom<C, P>>({Monom<C, P>(number)}) {};

    explicit Polynom(std::vector<Monom<C, P>> monoms)
            : std::vector<Monom<C, P>>((std::sort(monoms.begin(), monoms.end(), std::greater<Monom<C, P>>()), monoms)) {};

    explicit Polynom(std::initializer_list<Monom<C, P>> monoms)
            : std::vector<Monom<C, P>>(transformInitializerList(monoms)) {};

private:

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

    // undefined when size == 0
    const Monom<C, P>& head() const { return this->operator[](0); };

    bool isZero() const { return size() == 0; };

    using std::vector<Monom<C, P>>::operator[];

    Polynom operator+(const Polynom& that) const {
        int lsz = this->size(), rsz = that.size();
        std::vector<Monom<C, P>> sum;
        sum.reserve(std::max(lsz, rsz));

        for (int lp = 0, rp = 0; lp < lsz || rp < rsz;) {
            if (lp < lsz && rp < rsz && (*this)[lp].isSimilarTo(that[rp])) {
                const auto m = (*this)[lp++] + that[rp++];
                if (!m.isZero())
                    sum.push_back(m);
            } else if (rp == rsz || (lp < lsz && (*this)[lp] > that[rp])) {
                sum.push_back((*this)[lp++]);
            } else {
                sum.push_back(that[rp++]);
            }
        }
        return Polynom(sum);
    };

    Polynom operator-(const Polynom& that) const { return (*this) + (that * -1); }

    Polynom& operator*=(const Monom<C, P>& monom) {
        if (monom.isZero())
            return (*this) = Polynom{};

        for (auto& it : *this)
            it *= monom;
        return *this;
    };

    Polynom operator*(const Polynom& that) const {
        Polynom res{};
        for (const auto& it : that)
            res += (*this) * it;
        return res;
    };

    bool isDividableBy(const Polynom& that) const {
        return isZero() || (!that.isZero() && head().isDividableBy(that.head()));
    };

    bool isReducibleBy(const Polynom& that) const { return isDividableBy(that); }

    Polynom singleReduceBy(const Polynom& that) const {
        if (isZero())
            return Polynom{};
        return (*this) - head() / that.head() * that;
    }

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
        auto thatHead = that.head();
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
        if (isZero())
            return *this;
        std::vector<Polynom> tmp{that};
        return reduceBy(tmp.begin(), tmp.end());
    };

    template<typename C1, typename P1>
    friend std::ostream& operator<<(std::ostream& os, const Polynom<C1, P1>& polynom);
    template<typename C1, typename P1>
    friend bool operator==(const Polynom<C1, P1>& lhs, const Polynom<C1, P1>& rhs);
    template<typename T>
    friend struct std::hash;
};


template<typename C, typename P>
Polynom<C, P>& operator+=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs + rhs; }

template<typename C, typename P>
Polynom<C, P>& operator-=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs - rhs; }

template<typename C, typename P>
Polynom<C, P> operator*(Polynom<C, P> polynom, const Monom<C, P>& monom) { return polynom *= monom; }

template<typename C, typename P, typename N>
Polynom<C, P> operator*(const Polynom<C, P>& polynom, N number) { return polynom * Monom<C, P>{number}; }

template<typename C, typename P, typename N>
Polynom<C, P> operator*(N number, const Polynom<C, P>& polynom) { return operator*(polynom, number); }

template<typename C, typename P>
Polynom<C, P> operator*(const Monom<C, P>& monom, const Polynom<C, P>& polynom) { return polynom * monom; }

template<typename C, typename P>
Polynom<C, P>& operator*=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs * rhs; }

template<typename C, typename P>
Polynom<C, P>& operator/=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs / rhs; }

template<typename C, typename P>
Polynom<C, P>& operator%=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs % rhs; }

template<typename C, typename P>
Polynom<C, P> operator^(Polynom<C, P> polynom, P power) { return polynom ^= power; }

template<typename C, typename P>
Polynom<C, P>& operator^=(Polynom<C, P>& result, P power) {
    Polynom<C, P> t{result};
    result = 1;
    while (power > 0) {
        if (power & 1)
            result *= t;
        t *= t;
        power >>= 1;
    }
    return result;
}

template<typename C, typename P>
bool operator==(const Polynom<C, P>& lhs, const Polynom<C, P>& rhs) {
    return static_cast<const std::vector<corecvs::Monom<C, P>>&>(lhs) ==
           static_cast<const std::vector<corecvs::Monom<C, P>>&>(rhs);
}

template<typename C, typename P>
bool operator!=(const Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return !(lhs == rhs); }

template<typename C, typename P>
std::ostream& operator<<(std::ostream& os, const Polynom<C, P>& polynom) {
    if (polynom.isZero())
        os << "0";
    else {
        for (auto i = polynom.begin(), preLast = std::prev(polynom.end()); i != preLast; i++)
            os << *i << " + ";
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
        for(auto& m : p)
            h = (h + monomHasher(m)) * 31 % 10000019;
        return h;
    };
};

} // end std


#endif //CORECVS_POLYNOM_H
