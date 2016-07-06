#ifndef ASSERTS
#define ASSERTS
#endif

#include "gtest/gtest.h"

#include "global.h"
#include "polynomial.h"
#include "polynomialSolver.h"

#include <random>

using namespace corecvs;

const int DEFAULT_SEED = 777;
const int RNG_RETRIES = 131072;
const double REL_SOL_TOLERANCE = 1e-5;
const double COEFF_LIMIT = 1e2;
const double MAX_SCALE_LIMIT = 1e2;
#define M(i, j, c) m.a(i, j) = c;
#define L(i, j, c) l.a(i, j) = c;
#define MR(i, j, c) mr.a(i, j)= c;

TEST(PolynomialMatrixTest, testMul)
{
    corecvs::PolynomialMatrix m(3, 4);
    corecvs::Polynomial poly(1.0);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            m.a(i, j) = poly;
            poly *= corecvs::Polynomial::X();
        }
    }
    corecvs::Matrix l(4, 3);
    L(0, 0, 1.0); L(0, 1, 2.0); L(0, 2, 3.0);
    L(1, 0, 4.0); L(1, 1, 6.0); L(1, 2, 8.0);
    L(2, 0,10.0); L(2, 1,11.0); L(2, 2, 9.0);
    L(3, 0,-1.0); L(3, 1, 1.0); L(3, 2, 2.0);

    auto MM = l * m;
    bool validType = std::is_same<decltype(MM), corecvs::PolynomialMatrix>::value;
    ASSERT_TRUE(validType);
    ASSERT_EQ(MM.h, l.h);
    ASSERT_EQ(MM.w, m.w);

    auto x = corecvs::Polynomial::X();
    auto x2 = x * x;
    auto x3 = x * x2;
    auto x4 = x2 * x2;
    auto x5 = x4 * x;
    auto x6 = x4 * x2;
    auto x7 = x4 * x3;
    auto x8 = x4 * x4;
    auto x9 = x4 * x5;
    auto x10= x5 * x5;
    auto x11= x6 * x5;
    corecvs::PolynomialMatrix mr(4, 4);

    MR(0, 0, 3*x8 + 2*x4 + 1);   MR(0, 1, 3*x9 + 2*x5 + x);     MR(0, 2, 3*x10 + 2*x6 + x2);     MR(0, 3, 3*x11 + 2*x7 + x3);
    MR(1, 0, 8*x8 + 6*x4 + 4);   MR(1, 1, 8*x9 + 6*x5 + 4*x);   MR(1, 2, 8*x10 + 6*x6 + 4*x2);   MR(1, 3, 8*x11 + 6*x7 + 4*x3);
    MR(2, 0, 9*x8 + 11*x4 + 10); MR(2, 1, 9*x9 + 11*x5 + 10*x); MR(2, 2, 9*x10 + 11*x6 + 10*x2); MR(2, 3, 9*x11 + 11*x7 + 10*x3);
    MR(3, 0, 2*x8 + x4 - 1);     MR(3, 1, 2*x9 + x5 - x);       MR(3, 2, 2*x10 + x6 - x2);       MR(3, 3, 2*x11 + x7 - x3);

    auto diff = MM - mr;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            auto p = diff.a(i, j);
            for (size_t i = 0; i <= p.degree(); ++i)
                ASSERT_NEAR(p[i], 0.0, 1e-9);
        }
    }
}

TEST(PolynomialMatrixTest, testBasics)
{
    corecvs::PolynomialMatrix m(3, 3, corecvs::Polynomial::X());
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (double x = -1.0; x <= 1.0; x += 0.01)
                ASSERT_NEAR(x, m.element(i, j)(x), 1e-9);
        }
    }
    // XXX: basic test for copies
    m.element(0, 0) = m.element(1, 1) = m.element(2, 2) = 1.0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (double x = -1.0; x <= 1.0; x += 0.01)
                ASSERT_NEAR(i == j ? 1.0 : x, m.element(i, j)(x), 1e-9);
        }
    }
}

auto X = corecvs::Polynomial::X();

TEST(PolynomialMatrixTest, testEvaluate)
{
    corecvs::PolynomialMatrix m(2, 2, 0.0);
    M(0, 0, 1.0); M(0, 1,         X);
    M(1, 0, X*X); M(1, 1, X*X+X+1.0);
    for (double x = -10.0; x < 10.0; x += 0.01)
    {
        auto E = m(x);
        ASSERT_NEAR(E.a(0, 0),       1.0, 1e-9);
        ASSERT_NEAR(E.a(0, 1),         x, 1e-9);
        ASSERT_NEAR(E.a(1, 0),       x*x, 1e-9);
        ASSERT_NEAR(E.a(1, 1), x*x+x+1.0, 1e-9);
    }
}

#ifdef WITH_BLAS
TEST(PolynomialMatrixTest, testDet)
{
    corecvs::PolynomialMatrix m(3, 3, corecvs::Polynomial::X());
    // All rows lineary-dependent => 0-degree poly should be equal to 0.0
    auto poly = m.det(0);
    ASSERT_NEAR(poly[0], 0.0, 1e-9);

    M(0, 0,         X); M(0, 1,   X * X); M(0, 2, X * X * X);
    M(1, 0,       1.0); M(1, 1, X + 1.0); M(1, 2,      10.0);
    M(2, 0, X*X*X*X*X); M(2, 1,     2.0); M(2, 2,         X);
    /*         X X^2 X^3
     * det (   1 X+1  10 ) = -x^9 -x^8 +10x^7 +2x^3 + x^2 - 20x
     *       X^5   2   X
     */
//  std::cout << m << std::endl;
    auto P = m.det(9);
    cout << P << std::endl;
    ASSERT_NEAR(P[0],  0.0, 1e-9);
    ASSERT_NEAR(P[1],-20.0, 1e-9);
    ASSERT_NEAR(P[2],  1.0, 1e-9);
    ASSERT_NEAR(P[3],  2.0, 1e-9);
    ASSERT_NEAR(P[4],  0.0, 1e-9);
    ASSERT_NEAR(P[5],  0.0, 1e-9);
    ASSERT_NEAR(P[6],  0.0, 1e-9);
    ASSERT_NEAR(P[7], 10.0, 1e-9);
    ASSERT_NEAR(P[8], -1.0, 1e-9);
    ASSERT_NEAR(P[9], -1.0, 1e-9);

    /*
     * More interesting example (coeffs at x^6 are vanished)
     *       X   X*X X*X*X
     * det ( X*X X*X     X ) = 2x^5-5x^4+4x^3-x^2
     *       1     1  3-2X
     */

    M(0, 0,   X); M(0, 1, X*X); M(0, 2,        X*X*X);
    M(1, 0, X*X); M(1, 1, X*X); M(1, 2,            X);
    M(2, 0, 1.0); M(2, 1, 1.0); M(2, 2, X*(-2.0)+3.0);
    P = m.det(5);
    std::cout << P << std::endl;
    ASSERT_NEAR(P[0], 0.0, 1e-9);
    ASSERT_NEAR(P[1], 0.0, 1e-9);
    ASSERT_NEAR(P[2],-1.0, 1e-9);
    ASSERT_NEAR(P[3], 4.0, 1e-9);
    ASSERT_NEAR(P[4],-5.0, 1e-9);
    ASSERT_NEAR(P[5], 2.0, 1e-9);
}
#endif


TEST(PolynomialTest, testInterpolation)
{
    auto X = corecvs::Polynomial::X();
    auto P = (X - 1.0) * (X - 2.0) * (X - 3.0);
    std::vector<double> v, x = {-1.0, -1.0 / 3.0, 1.0 / 3.0, 1.0};
    for (auto& xx: x)
    {
        v.push_back(P(xx));
        std::cout << *v.rbegin() << " ";
    }
    std::cout << std::endl;
    std::cout << "P: " << P << std::endl;
    auto P2 = corecvs::Polynomial::Interpolate(x, v);
    std::cout << "PI: " << P2 << std::endl;
    auto PD = P2 - P;
    for (double xx = -10.0; xx <= 10.0; xx += 0.001)
        ASSERT_NEAR(PD(xx), 0.0, 1e-6);

}

TEST(PolynomialTest, testBasics)
{
    auto polyX = corecvs::Polynomial::X();
    ASSERT_TRUE(polyX.degree() == 1);
    ASSERT_TRUE((polyX * 2.0).degree() == 1);
    ASSERT_TRUE((polyX * corecvs::Polynomial::X()).degree() == 2);
    ASSERT_TRUE((polyX + 2.0).degree() == 1);
    ASSERT_TRUE((polyX - 2.0).degree() == 1);
    auto poly = (corecvs::Polynomial::X() - 1.0) *
                (corecvs::Polynomial::X() - 2.0) *
                (corecvs::Polynomial::X() - 3.0);
    std::cout << "Poly created: " << poly << std::endl;
    ASSERT_TRUE(poly.degree() == 3);
    ASSERT_NEAR(poly(1.0), 0.0, 1e-9);
    ASSERT_NEAR(poly(2.0), 0.0, 1e-9);
    ASSERT_NEAR(poly(3.0), 0.0, 1e-9);
    ASSERT_NEAR(poly(4.0), 6.0, 1e-9);
    auto mpoly = -poly;
    mpoly += poly;
    for (size_t i = 0; i <= mpoly.degree(); ++i)
        ASSERT_NEAR(mpoly[i], 0.0, 1e-9);
}

TEST(PolynomialTest, testFromRoots)
{
    std::vector<double> roots = {1.0, 2.0, 3.0, -5.0, 6.0, 7.0};
    auto poly = corecvs::Polynomial::FromRoots(roots);
    for (auto& r: roots)
        ASSERT_NEAR(poly(r), 0.0, 1e-9);
    ASSERT_FALSE(std::abs(poly(-1.0)) < 0.5);
}

TEST(PolynomialSolversTest, testPow1)
{
    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> unif(-COEFF_LIMIT, COEFF_LIMIT);

    //const int POW = 1;
    double coeff[2], roots[1];
    size_t rootsN;

    for (int i = 0; i < RNG_RETRIES; ++i)
    {
        coeff[0] = unif(rng);
        coeff[1] = unif(rng);

        rootsN = corecvs::PolynomialSolver::solve_imp<1>(coeff, roots);
        if (rootsN)
        {
            ASSERT_NEAR(coeff[0] + coeff[1] * roots[0], 0.0, REL_SOL_TOLERANCE * std::max(std::abs(coeff[0]), std::abs(coeff[1])));
        }
    }
    coeff[0] = 1e-11;
    coeff[1] = 1e-10;
    rootsN = corecvs::PolynomialSolver::solve_imp<1>(coeff, roots);
    ASSERT_TRUE(rootsN == 1);
    ASSERT_NEAR(corecvs::PolynomialSolver::evaluate(coeff, roots[0], 1), 0.0, REL_SOL_TOLERANCE);

    coeff[0] = 1e11;
    coeff[1] = 1e10;
    rootsN = corecvs::PolynomialSolver::solve_imp<1>(coeff, roots);
    ASSERT_TRUE(rootsN == 1);
    ASSERT_NEAR(corecvs::PolynomialSolver::evaluate(coeff, roots[0], 1), 0.0, REL_SOL_TOLERANCE);
}

TEST(PolynomialSolversTest, testPow2)
{
    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> unif(-COEFF_LIMIT, COEFF_LIMIT);

    //const int POW = 2;
    double coeff[3], roots[2];
    size_t rootsN;

    for (int i = 0; i < RNG_RETRIES; ++i)
    {
        coeff[0] = unif(rng);
        coeff[1] = unif(rng);
        coeff[2] = unif(rng);
        double mc = std::max(std::abs(coeff[0]), std::max(std::abs(coeff[1]), std::abs(coeff[2])));

        rootsN = corecvs::PolynomialSolver::solve_imp<2>(coeff, roots);
        if (rootsN)
        {
            ASSERT_NEAR(corecvs::PolynomialSolver::evaluate(coeff, roots[0], 2), 0.0, mc * REL_SOL_TOLERANCE);
            if (rootsN > 1)
                ASSERT_NEAR(corecvs::PolynomialSolver::evaluate(coeff, roots[1], 2), 0.0, mc * REL_SOL_TOLERANCE);
        }
    }
}

void coeffByRoots(std::vector<double> &coeff, std::vector<double> &roots)
{
    coeff.clear();
    coeff.resize(roots.size() + 2);
    coeff[0] = 1.0;
    for (auto& r: roots)
    {
        for (size_t i = roots.size(); i > 0; --i)
        {
            coeff[i] = (coeff[i - 1] - r * coeff[i]);
        }
        coeff[0] = -r * coeff[0];
    }
}

#ifdef WITH_BLAS
TEST(PolynomialSolversTest, testPowN)
{
    // (x - 1)(x - 2)(x - 3)
    double coeff3[] = {-6.0, 11.0, -6.0, 1.0};
    double roots3[3];

    int rcnt = (int)corecvs::PolynomialSolver::solve(coeff3, roots3, 3);

    ASSERT_TRUE(rcnt == 3);
    for (int i = 0; i < rcnt; ++i)
    {
        ASSERT_NEAR(corecvs::PolynomialSolver::evaluate(coeff3, roots3[i], 3), 0.0, REL_SOL_TOLERANCE);
    }

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> unif(-COEFF_LIMIT, COEFF_LIMIT);
    double maxdiff = 0.0;

    std::vector<double> coeff, roots, roots2;
    for (int i = 0; i < RNG_RETRIES; ++i)
    {
        uint N = (rng() % 5) + 3;
        if (coeff.size() < N + 1)
        {
            coeff.resize(N + 1);
            roots.resize(N + 1);
        }
        for (uint j = 0; j < N; ++j)
            roots[j] = unif(rng);
        roots.resize(N);
        roots2.resize(N);
        coeffByRoots(coeff, roots);

        uint cnt = (uint)corecvs::PolynomialSolver::solve(&coeff[0], &roots2[0], N);
        ASSERT_TRUE(cnt == N);
        std::sort(roots.begin(), roots.end());
        std::sort(roots2.begin(), roots2.end());
        for (size_t j = 0; j < N; ++j)
        {
            ASSERT_NEAR(roots[j], roots2[j], std::abs(roots[j]) * REL_SOL_TOLERANCE);
            if (std::abs(roots[j] - roots2[j]) / std::max(std::abs(roots[j]), 1.0))
            {
                maxdiff = std::abs(roots[j] - roots2[j]) / std::max(std::abs(roots[j]), 1.0);
            }
        }
    }
    std::cout << "MAX diff: " << maxdiff << std::endl;
}

TEST(PolynomialSolversTest, testPowN2)
{
    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> unif(-COEFF_LIMIT, COEFF_LIMIT);
    for (int iter = 0; iter < RNG_RETRIES; ++iter)
    {
        int N = 3 + rng() % 4;
        std::vector<double> roots, roots2;
        for (int i = 0; i < N; ++i)
            roots.push_back(unif(rng));
        auto poly = corecvs::Polynomial::FromRoots(roots);
        corecvs::PolynomialSolver::solve(poly, roots2);
        ASSERT_TRUE(roots.size() == roots2.size());
        std::sort(roots.begin(), roots.end());
        std::sort(roots2.begin(), roots2.end());
        for (size_t i = 0; i < roots.size(); ++i)
            ASSERT_NEAR(roots[i], roots2[i], REL_SOL_TOLERANCE * std::abs(roots[i]));
    }
}
#endif
