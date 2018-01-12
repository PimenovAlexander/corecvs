#include "core/polynomial/polynomialSolver.h"
#include "core/math/matrix/matrix.h"
#include "core/math/vector/vector.h"
#include "wrappers/cblasLapack/cblasLapackeWrapper.h"

const double corecvs::PolynomialSolver::RELATIVE_TOLERANCE = 1e-9;

double corecvs::PolynomialSolver::evaluate(const double* coeff, const double &x, const size_t &degree)
{
    if (!degree)
        return coeff[0];

    double v = coeff[degree];
    for (size_t i = degree; i > 0; --i)
    {
        v = coeff[i - 1] + v * x;
    }
    return v;
}

template<>
size_t corecvs::PolynomialSolver::solve_imp<1>(const double* coeff, double* roots, const size_t &degree)
{
    CORE_UNUSED(degree);

    if (std::abs(coeff[1]) < RELATIVE_TOLERANCE * coeff[0])
        return 0;

    roots[0] = -coeff[0] / coeff[1];
    return 1;
}

// There is numerically-stable closed-form solution for quadrics
template<>
size_t corecvs::PolynomialSolver::solve_imp<2>(const double* coeff, double* roots, const size_t &degree)
{
    CORE_UNUSED(degree);

    double a = coeff[2], b = coeff[1], c = coeff[0];

    double b2 = b * b;
    double ac = 4.0 * a * c;
    if (b2 < ac * (1.0 - RELATIVE_TOLERANCE))
        return 0;
    if (b2 < ac * (1.0 + RELATIVE_TOLERANCE))
    {
        roots[0] = -b / (2.0 * a);
        return 1;
    }
    double a2 = a * 2.0;
    double c2 = c * 2.0;

    double sqrtd = sqrt(b2 - ac);
    if (b > 0.0)
    {
        double q = (-b - sqrtd);
        roots[0] = q / a2;
        roots[1] = c2 / q;
    }
    else
    {
        double q = (-b + sqrtd);
        roots[0] = c2 / q;
        roots[1] = q / a2;
    }
    return 2;
}
    
size_t corecvs::PolynomialSolver::solve(const double* coeff, double* roots, size_t degree)
{
    if (degree == 2)
        return solve_imp<2>(coeff, roots, degree);
    if (degree == 1)
        return solve_imp<1>(coeff, roots, degree);
#ifdef WITH_BLAS
    return solve_companion(coeff, roots, degree);
#else
	CORE_ASSERT_TRUE_S(false);
	return 0;
#endif
}

void corecvs::PolynomialSolver::solve(const corecvs::Polynomial &poly, std::vector<double> &roots)
{
    roots.resize(poly.degree());
    roots.resize(solve(poly.data(), &roots[0], poly.degree()));
}

#ifdef WITH_BLAS
/** Unfortunately I do not know any super-stable closed-form methods for solving
 *  3+-order polynomial equations and do not want to bother with Cardano/Ferrari-like stuff.
 */
size_t corecvs::PolynomialSolver::solve_companion(const double* coeff, double* roots, const size_t &degree)
{
    /*
     * Here we use [Frobenius] companion matrix or Fiedler matrix
     *
     * For more info you can read
     * M. Fieler A note on companion matrices [Linear Algebra and its Applications 372 (2003]
     * For further reading you may consider 
     * Fernando De Teran Backward stability of polynomial root-finding using Fiedler companion matrices [IMA Journal of Numerical Analysis (2014)]
     */
    corecvs::Matrix companion((int)degree, (int)degree);
    double max_coeff = coeff[degree];
#ifndef FIEDLER
    companion.a(0, degree - 1) = -coeff[0] / max_coeff;
    for (size_t i = 1; i < degree; ++i)
    {
        companion.a(i, i - 1) = 1.0;
        companion.a(i, degree - 1) = -coeff[i] / max_coeff;
    }
#else
    size_t curr_degree = degree - 1;
    for (size_t i = 0; i < degree; ++i)
    {
        if (i % 2)
        {
            if (i == 1)
            {
                companion.a(1, 0) = 1.0;
            }
            else
            {
                companion.a((int)i, (int)i - 2) = 1.0;
            }
        }
        else
        {
            if (!i)
            {
                companion.a(0, 0) = -coeff[curr_degree--] / max_coeff;
                companion.a(0, 1) = -coeff[curr_degree--] / max_coeff;
                companion.a(0, 2) = 1.0;
            }
            else
            {
                companion.a((int)i, (int)i - 1) = -coeff[curr_degree--] / max_coeff;
                if (i != degree - 1)
                {
                    companion.a((int)i, (int)i + 1) = -coeff[curr_degree--] / max_coeff;
                    if (i != degree - 2)
                        companion.a((int)i, (int)i + 2) = 1.0;
                }
            }
        }
    }
#endif // FIEDLER

    // evd
    corecvs::Vector wr((int)degree), wi((int)degree);
    LAPACKE_dgeev(LAPACK_ROW_MAJOR, 'N', 'N', (int)degree, companion.data, companion.stride, &wr[0], &wi[0], 0, (int)degree, 0, (int)degree);

    // find non-complex and return
    size_t cnt = 0;
    for (size_t i = 0; i < degree; ++i)
    {
        if (std::abs(wi[(int)i]) <= std::abs(wr[(int)i]) * RELATIVE_TOLERANCE)
        {
            roots[cnt++] = wr[(int)i];
        }
    }
    return cnt;
}
#endif // WITH_BLAS
