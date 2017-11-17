#include "core/polynomial/polynomial.h"

#include <iomanip>

double corecvs::Polynomial::polishRoot(double x0, int N) const
{
    int it = 0;
    double val = std::abs((*this)(x0));
//    double v0 = val;
    do
    {
        double df = 0.0, d2f = 0.0;
        double v1 = 1.0, v2 = 0.0;
        for (size_t i = 1; i <= degree(); ++i)
        {
            double c = (*this)[i];
            df += i * v1 * c;
            d2f+= i * (i - 1) * v2 * c;
            v2 = v1;
            v1 *= x0;
        }
        // d(0.5f^2)/dx=f'f
        // d2(0.5f^2)/dx2=f''f+f'f'
        double dx = -(df * val) / (d2f * val + df * df);
        double x1 = x0 + dx;
        double vn = std::abs((*this)(x0 + dx));
        if (vn >= val)
            break;
        val = vn;
        ++it;
        x0 = x1;
    } while (it < N);
//    double vfin = val;
//    std::cout << v0 / vfin << "x improvement" << std::endl;
    return x0;
}

const double* corecvs::Polynomial::data() const
{
    return &static_cast<const std::vector<double>&>(*this)[0];
}

double& corecvs::Polynomial::operator[] (const size_t &power)
{
    if (power >= size())
        resize(power + 1);
    return static_cast<std::vector<double>&>(*this)[power];
}

double corecvs::Polynomial::operator[] (const size_t &power) const
{
    return power >= size() ? 0 : static_cast<const std::vector<double>&>(*this)[power];
}

double  corecvs::Polynomial::operator() (const double &at) const
{
    size_t deg = degree();
    if (!size())
        return 0.0;
    double v = (*this)[deg];
    for (size_t i = deg; i > 0; --i)
    {
        v = (*this)[i - 1] + v * at;
    }
    return v;
}

size_t corecvs::Polynomial::degree() const
{
    return size() ? size() - 1 : 0;
}

corecvs::Polynomial corecvs::Polynomial::operator* (const double &v) const
{
    corecvs::Polynomial ans = *this;
    ans *= v;
    return ans;
}

corecvs::Polynomial& corecvs::Polynomial::operator*=(const double &v)
{
    for (auto& p: *this)
        p *= v;
    return *this;
}

corecvs::Polynomial corecvs::operator*(const double &lhs, const corecvs::Polynomial &rhs)
{
    return rhs * lhs;
}

corecvs::Polynomial  corecvs::Polynomial::operator* (const corecvs::Polynomial &rhs) const
{
    corecvs::Polynomial ans = *this;
    ans *= rhs;
    return ans;
}

corecvs::Polynomial& corecvs::Polynomial::operator*=(const corecvs::Polynomial &rhs)
{
    size_t degl = degree(), degr = rhs.degree();
    size_t total = degl + degr;
    std::vector<double> coeff(total + 1);
    for (size_t i = 0; i <= degl; ++i)
    {
        for (size_t j = 0; j <= degr; ++j)
        {
            coeff[i + j] += (*this)[i] * rhs[j];
        }
    }
    *this = coeff;
    return *this;
}

corecvs::Polynomial corecvs::Polynomial::operator- () const
{
    corecvs::Polynomial ret = *this;
    for (auto& c: ret)
        c *= -1.0;
    return ret;
}

corecvs::Polynomial  corecvs::Polynomial::operator+ (const corecvs::Polynomial &rhs) const
{
    corecvs::Polynomial ans = *this;
    ans += rhs;
    return ans;
}

corecvs::Polynomial& corecvs::Polynomial::operator+=(const corecvs::Polynomial &rhs)
{
    size_t deg = rhs.degree();
    for (size_t i = 0; i <= deg; ++i)
        (*this)[i] += rhs[i];
    return *this;
}

corecvs::Polynomial  corecvs::Polynomial::operator- (const corecvs::Polynomial &rhs) const
{
    corecvs::Polynomial ans = *this;
    ans -= rhs;
    return ans;
}

corecvs::Polynomial& corecvs::Polynomial::operator-=(const corecvs::Polynomial &rhs)
{
    size_t deg = rhs.degree();
    for (size_t i = 0; i <= deg; ++i)
        (*this)[i] -= rhs[i];
    return *this;
}

std::ostream& operator<<(std::ostream& os, const corecvs::Polynomial &p)
{
    size_t deg = p.degree();
    os << std::showpos;
    for (size_t i = 0; i <= deg; ++i)
    {
        os << p[i] << "*x^" << i << " ";
    }
    return os;
}

corecvs::Polynomial::Polynomial(const double &x0)
{
    push_back(x0);
}

corecvs::Polynomial::Polynomial(const double *coeff, size_t degree)
    : std::vector<double>(coeff, coeff + degree + 1)
{
}

corecvs::Polynomial::Polynomial(const std::vector<double> &coeff)
    : std::vector<double>(coeff)
{
}

corecvs::Polynomial::Polynomial(const std::initializer_list<double> &coeff)
    : std::vector<double>(coeff)
{
}

corecvs::Polynomial::Polynomial(const corecvs::Vector &coeff)
    : corecvs::Polynomial(&coeff.at(0), coeff.size() - 1)
{
}

corecvs::Polynomial corecvs::Polynomial::X()
{
    double c[] = {0.0, 1.0};
    return Polynomial(c, 1);
}

corecvs::Polynomial corecvs::Polynomial::FromRoots(const std::vector<double> &roots)
{
    corecvs::Polynomial ans = 1.0;
    for (auto& r: roots)
        ans *= corecvs::Polynomial::X() - r;
    return ans;
}

corecvs::Polynomial corecvs::Polynomial::Interpolate(const std::vector<double> &x, const std::vector<double> &Px)
{
    CORE_ASSERT_TRUE_S(x.size() == Px.size());
    CORE_ASSERT_TRUE_S(x.size());
    size_t /*N = x.size() - 1,*/ N1 = x.size();

    std::vector<double> coeff(N1);
    coeff[0] = Px[0];
    for (size_t i = 1; i < N1; ++i)
    {
        double nom   = Px[i];
        double denom = 1.0;
        for (size_t j = 0; j < i; ++j)
            denom *= x[i] - x[j];
        double prod = 1.0;
        for (size_t j = 0; j < i; ++j)
        {
            nom -= coeff[j] * prod;
            prod *= x[i] - x[j];
        }
        coeff[i] = nom / denom;
    }
    corecvs::Polynomial p, P = 1.0;
    for (size_t i = 0; i < N1; ++i)
    {
        p += P * coeff[i];
        P *= X() - x[i];
    }
    return p;
}

corecvs::PolynomialMatrix::PolynomialMatrix(int h, int w, const Polynomial& poly)
    : corecvs::AbstractBuffer<Polynomial, int32_t>(h, w, poly)
{

}

corecvs::Matrix corecvs::PolynomialMatrix::operator() (const double &x) const
{
    corecvs::Matrix result(h, w);
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            result.a(i, j) = element(i, j)(x);
        }
    }
    return result;
}

#ifdef WITH_BLAS
corecvs::Polynomial corecvs::PolynomialMatrix::det(const size_t requiredPower) const
{
    std::vector<double> evaluationPoints(requiredPower + 1);
    double each = 2.0 / requiredPower;
    double left = -1.0;
    for (auto& v: evaluationPoints)
    {
        v = left;
        left += each;
    }

    std::vector<double> evaluatedValues;
    for (auto& v: evaluationPoints)
    {
        evaluatedValues.push_back((*this)(v).det());
    }

    return corecvs::Polynomial::Interpolate(evaluationPoints, evaluatedValues);
}
#endif // WITH_BLAS
