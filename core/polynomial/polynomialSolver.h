#ifndef POLYNOMIALSOLVER_H
#define POLYNOMIALSOLVER_H

#include "global.h"
#include "polynomial.h"

#define FIEDLER

namespace corecvs
{

/*
 * Solver for polynomial equations
 *
 * Polynom is stored in coeff in form \sum\limits_{i=0}^{n}c_i x^i
 * Currently only real roots are being found
 * For n=2 numerically stable evaluation is presented,
 * for n > 2 decomposition of companion matrix is used.
 */
class PolynomialSolver
{
public:
    /*
     * Degree-specific solvers (currently -- 1st and 2nd degree only)
     */
    template<size_t deg>
    static size_t solve_imp(const double* coeff, double* roots, const size_t &degree = deg);
    /*
     * Dispatch method for solving arbitrary degree
     */
    static size_t solve(const double* coeff, double* roots, size_t degree);
    static void   solve(const corecvs::Polynomial &poly, std::vector<double> &roots);
    static double evaluate(const double* coeff, const double &x, const size_t &degree);
    static const double RELATIVE_TOLERANCE;
private:
    /*
     * Degree-unspecific solver based on apropriate EV-problem
     */
#ifdef WITH_BLAS
    static size_t solve_companion(const double* coeff, double* roots, const size_t &degree);
#endif
};

template<>
size_t PolynomialSolver::solve_imp<1>(const double* coeff, double* roots, const size_t &degree);
template<>
size_t PolynomialSolver::solve_imp<2>(const double* coeff, double* roots, const size_t &degree);


}

#endif
