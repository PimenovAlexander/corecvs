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

    explicit Polynom(std::vector<Monom<C, P>> monoms)
            : std::vector<Monom<C, P>>((std::sort(monoms.begin(), monoms.end()), monoms)) {};

    explicit Polynom(std::initializer_list<Monom<C, P>> monoms)
            : std::vector<Monom<C, P>>(transformInitializerList(monoms)) {};

private:

    static std::vector<Monom<C, P>> transformInitializerList(const std::initializer_list<Monom<C, P>>& monoms) {
        std::vector<Monom<C, P>> result {monoms};
        std::sort(result.begin(), result.end());
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
        std::vector<Monom<C, P>> sum(std::max(lsz, rsz));

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

    Polynom reduceBy(const Polynom& that) {
        return *this + head() / that.head() * (-1);
    }

    template<typename Iter>
    Polynom reduceBy(Iter begin, Iter end) {
        bool wasNotReduced = false;
        auto p = *this;
        while (!p.isZero() && !wasNotReduced) {
            wasNotReduced = true;
            while (begin != end) {
                auto q = *begin;
                if (p.isReducibleBy(q)) {
                    p.reduceBy(q);
                    wasNotReduced = false;
                }
                ++begin;
            }
        }
        return p;
    }

    Polynom operator/(const Polynom& that) {
        if (isZero())
            return *this;

        std::vector<Polynom> tmp{1, that};
        return reduceBy(tmp.begin(), tmp.end());
    }

    Polynom operator%(const Polynom& that) {
        return isZero() ? *this : *this - that * (*this / that);
    };

    friend std::ostream& operator<<(std::ostream& os, const Polynom& polynom);
};


template<typename C, typename P>
Polynom<C, P>& operator+=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs + rhs; }

template<typename C, typename P>
Polynom<C, P>& operator-=(Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return lhs = lhs - rhs; }

template<typename C, typename P>
Polynom<C, P> operator*(Polynom<C, P> polynom, const Monom<C, P>& monom) { return polynom *= monom; }

template<typename C, typename P, typename N>
Polynom<C, P> operator*(const Polynom<C, P>& polynom, N number) { return polynom * Monom<C, P>(number); }

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
bool operator==(const Polynom<C, P>& lhs, const Polynom<C, P>& rhs) {
    return static_cast<std::vector<corecvs::Monom<C, P>>>(*lhs) == static_cast<std::vector<corecvs::Monom<C, P>>>(*rhs);
}

template<typename C, typename P>
bool operator!=(const Polynom<C, P>& lhs, const Polynom<C, P>& rhs) { return !(lhs == rhs); }

template<typename C, typename P>
std::ostream& operator<<(std::ostream& os, const Polynom<C, P>& polynom) {
    std::copy(polynom.begin(), polynom.end() - 1, std::ostream_iterator<std::string>{os, " + "});
    os << *(polynom.end() - 1);
    return os;
}

} // end corecvs

namespace std
{

template<typename C, typename P>
struct hash<corecvs::Polynom<C, P>>
{
    std::size_t operator()(const corecvs::Polynom<C, P>& p) const noexcept {
        return hash<vector<corecvs::Monom<C, P>>>()(static_cast<vector<corecvs::Monom<C, P>>>(p));
    };
};

} // end std


#endif //CORECVS_POLYNOM_H
