#ifndef CORECVS_MONOM_H
#define CORECVS_MONOM_H

#include <vector>
#include <algorithm>
#include <ostream>
#include <sstream>

namespace corecvs
{

template<typename C, typename P>
class Monom
{
public:
    constexpr static const double PRECISION = 1e-4;

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

    bool isZero() const { return abs(coeff) < PRECISION; };

    bool isSimilarTo(const Monom& that) const {
        if (std::abs(coeff) < Monom<C, P>::PRECISION && std::abs(that.coeff) < Monom<C, P>::PRECISION)
            return true;
        // drop following nulls
        auto lhs_end_ind = std::distance(
                std::find_if(powers.rbegin(), powers.rend(), [](const P& it) { return it != 0; }), powers.rend());
        auto rhs_end_ind = std::distance(
                std::find_if(that.powers.rbegin(), that.powers.rend(), [](const P& it) { return it != 0; }),
                that.powers.rend());

        return lhs_end_ind == rhs_end_ind &&
               std::equal(powers.begin(), powers.begin() + lhs_end_ind, that.powers.begin());
    };

    // adds similar monoms only!
    Monom& operator+=(const Monom& that) {
        coeff += that.coeff;
        return *this;
    };

    // subtracts similar monoms only!
    Monom& operator-=(const Monom& that) {
        coeff -= that.coeff;
        return *this;
    };

    Monom& operator*=(const Monom& that) {
        coeff *= that.coeff;
        if (that.powers.size() > powers.size())
            powers.resize(that.powers.size(), 0);
        std::transform(that.powers.begin(), that.powers.end(), powers.begin(), powers.begin(), std::plus<P>{});
        return *this;
    };

    template<typename N>
    Monom& operator*=(N number) {
        coeff *= number;
        return *this;
    };

    bool isDividableBy(const Monom& that) const {
        if (this->isZero())
            return true;
        if (that.isZero() || (powers.size() < that.powers.size()))
            return false;
        for (int i = 0, rsz = that.powers.size(); i < rsz; i++) {
            if (powers[i] < that.powers[i])
                return false;
        }
        return true;
    };

    // divides dividable monoms only!
    Monom& operator/=(const Monom& that) {
        coeff /= that.coeff;
        for (int i = 0, sz = std::min(powers.size(), that.powers.size()); i < sz; ++i)
            powers[i] -= that.powers[i];
        return *this;
    };

    C getCoeff() const { return coeff; };
    const std::vector<P>& getPowers() const { return powers; };

    /* format example: '3x1^2*x3' for coeff = 3, powers == [2,0,1] */
    template<typename C1, typename P1>
    friend std::ostream& operator<<(std::ostream& os, const Monom<C1, P1>& monom);
    template<typename T>
    friend struct std::hash;

private:
    C coeff;
    std::vector<P> powers;
};


// adds similar monoms only!
template<typename C, typename P>
Monom<C, P> operator+(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs += rhs; }

// subtracts similar monoms only!
template<typename C, typename P>
Monom<C, P> operator-(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs -= rhs; }

template<typename C, typename P>
Monom<C, P> operator*(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs *= rhs; }

template<typename C, typename P, typename N>
Monom<C, P> operator*(Monom<C, P> monom, N number) { return monom *= number; }

template<typename C, typename P, typename N>
Monom<C, P> operator*(N number, const Monom<C, P>& monom) { return monom * number; }

// divides dividable monoms only!
template<typename C, typename P>
Monom<C, P> operator/(Monom<C, P> lhs, const Monom<C, P>& rhs) { return lhs /= rhs; }

template<typename C, typename P>
bool operator==(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
    if (std::abs(lhs.getCoeff() - rhs.getCoeff()) >= Monom<C, P>::PRECISION)
        return false;
    return lhs.isSimilarTo(rhs);
}

template<typename C, typename P>
bool operator!=(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return !(lhs == rhs); }

template<typename C, typename P>
bool operator<(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return lhs.getPowers() < rhs.getPowers(); }

template<typename C, typename P>
bool operator>(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return rhs < lhs; }

template<typename C, typename P>
bool operator<=(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return !(rhs < lhs); }

template<typename C, typename P>
bool operator>=(const Monom<C, P>& lhs, const Monom<C, P>& rhs) { return !(lhs < rhs); }

template<typename C, typename P>
std::ostream& operator<<(std::ostream& os, const Monom<C, P>& monom) {
    os << monom.coeff;
    for (int i = 0; i < monom.powers.size(); ++i) {
        std::string intercalate = i ? "*" : "";
        if (monom.powers[i] == 0)
            continue;
        else if (monom.powers[i] == 1)
            os << intercalate << 'x' << (i + 1);
        else
            os << intercalate << 'x' << (i + 1) << '^' << monom.powers[i];
    }
    return os;
}

// undefined when one of the arguments is zero
template<typename C, typename P>
Monom<C, P> lcm(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
    std::vector<P> resPowers{lhs.getPowers().size() >= rhs.getPowers().size() ? lhs.getPowers() : rhs.getPowers()};

    for (int i = 0, sz = std::min(lhs.getPowers().size(), rhs.getPowers().size()); i < sz; ++i)
        resPowers[i] = std::max(lhs.getPowers()[i], rhs.getPowers()[i]);
    return Monom<C, P>{lhs.getCoeff() * rhs.getCoeff(), resPowers};
}

template<typename C, typename P>
bool areRelativePrimes(const Monom<C, P>& lhs, const Monom<C, P>& rhs) {
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
        if (m.isZero())
            return h;
        // drop following nulls
        auto end_ind = std::distance(
                std::find_if(m.powers.rbegin(), m.powers.rend(), [](const P& it) { return it != 0; }), m.powers.rend());
        for (auto i = m.powers.begin(), lst = i + end_ind; i < lst; ++i)
            h = (h + pHash(*i)) * 31 % 10000019;
        return h;
    };
};

} // end std

#endif //CORECVS_MONOM_H
