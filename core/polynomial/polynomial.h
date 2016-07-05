#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "vector.h"
#include "abstractBuffer.h"
#include "matrix.h"

#include <vector>

namespace corecvs
{
/*
 * This class stores polynomials.
 * \alpha_0+\alpha_1 x + \dots + \alpha_n x^n
 * is stored as \alpha_0 \dots \alpha_n
 */
class Polynomial : protected std::vector<double>
{
public:
    double&  operator[](const size_t &power);
    double   operator[](const size_t &power) const;
    const double* data() const;
    /*
     * Horner-scheme evaluator
     * Note, that this is not machine-accurate evaluator
     * TODO: replace with machine-accurate evaluator (for example -- Luc Paquet Precise evaluation of a polynomial at a point given in staggered
     * correction format [Journal of Computational and Applied Mathematics 50 (1994)]
     */
    double         operator()(const double &at   ) const;

    // TODO: This stuff should be out-of-class
    Polynomial  operator* (const double &v) const;
    Polynomial& operator*=(const double &v);
    Polynomial  operator* (const Polynomial &rhs) const;
    Polynomial& operator*=(const Polynomial &rhs);
    Polynomial  operator+ (const Polynomial &rhs) const;
    Polynomial& operator+=(const Polynomial &rhs);
    Polynomial  operator- (const Polynomial &rhs) const;
    Polynomial& operator-=(const Polynomial &rhs);
    Polynomial  operator- () const;


    size_t  degree() const;

    Polynomial(const double &x0 = 0.0);
    Polynomial(const double *coeff, size_t degree);
    Polynomial(const corecvs::Vector &coeff);
    Polynomial(const std::vector<double> &coeff);
    Polynomial(const std::initializer_list<double> &coeff);

    static Polynomial X();
    static Polynomial FromRoots(const std::vector<double> &roots);
    static Polynomial Interpolate(const std::vector<double> &x, const std::vector<double> &Px);

    template<typename V>
    void accept(V& visitor)
    {
        visitor.visit((std::vector<double>&)*this, "coeff");
    }
};

Polynomial operator*(const double &lhs, const Polynomial &rhs);

class PolynomialMatrix : public corecvs::AbstractBuffer<Polynomial, int32_t>
{
public:
    PolynomialMatrix(int h = 0, int w = 0, const Polynomial& poly = 0.0);
    corecvs::Matrix operator() (const double &x) const;
#ifdef WITH_BLAS
    Polynomial det(const size_t requiredPower) const;
#endif
    Polynomial& a(int y, int x) { return element(y, x); }
    Polynomial  a(int y, int x) const { return element(y, x); }
};

template<typename L, typename R>
typename std::enable_if<
        (std::is_same<L, PolynomialMatrix>::value && std::is_same<R, PolynomialMatrix>::value) ||
        (std::is_same<L, corecvs::Matrix >::value && std::is_same<R, PolynomialMatrix>::value) ||
        (std::is_same<L, PolynomialMatrix>::value && std::is_same<R, corecvs::Matrix >::value),
    PolynomialMatrix>::type
operator*(const L &lhs, const R &rhs)
{
    CORE_ASSERT_TRUE_S(lhs.w == rhs.h);
    PolynomialMatrix ret(lhs.h, rhs.w);
    for (int i = 0; i < lhs.h; ++i)
        for (int j = 0; j < rhs.w; ++j)
            for (int k = 0; k < lhs.w; ++k)
                ret.a(i, j) += lhs.a(i, k) * rhs.a(k, j);
    return ret;
}
template<typename L, typename R>
typename std::enable_if<
        (std::is_same<L, PolynomialMatrix>::value && std::is_same<R, PolynomialMatrix>::value) ||
        (std::is_same<L, corecvs::Matrix >::value && std::is_same<R, PolynomialMatrix>::value) ||
        (std::is_same<L, PolynomialMatrix>::value && std::is_same<R, corecvs::Matrix >::value),
    PolynomialMatrix>::type
operator+(const L &lhs, const R &rhs)
{
    CORE_ASSERT_TRUE_S(lhs.w == rhs.w && lhs.h == rhs.h);
    PolynomialMatrix ret(lhs.h, lhs.w);
    for (int i = 0; i < lhs.h; ++i)
        for (int j = 0; j < lhs.w; ++j)
            ret.a(i, j) = lhs.a(i, j) + rhs.a(i, j);
    return ret;
}
template<typename L, typename R>
typename std::enable_if<
        (std::is_same<L, PolynomialMatrix>::value && std::is_same<R, PolynomialMatrix>::value) ||
        (std::is_same<L, corecvs::Matrix >::value && std::is_same<R, PolynomialMatrix>::value) ||
        (std::is_same<L, PolynomialMatrix>::value && std::is_same<R, corecvs::Matrix >::value),
    PolynomialMatrix>::type
operator-(const L &lhs, const R &rhs)
{
    CORE_ASSERT_TRUE_S(lhs.w == rhs.w && lhs.h == rhs.h);
    PolynomialMatrix ret(lhs.h, lhs.w);
    for (int i = 0; i < lhs.h; ++i)
        for (int j = 0; j < lhs.w; ++j)
            ret.a(i, j) = lhs.a(i, j) - rhs.a(i, j);
    return ret;
}

}



std::ostream& operator<<(std::ostream&, const corecvs::Polynomial& p);


#endif
