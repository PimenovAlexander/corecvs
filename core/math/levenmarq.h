#ifndef LEVEN_MARQ_H
#define LEVEN_MARQ_H

/**
 * \file levenmarq.h
 * \brief This class implements the Levenberg Marquardt algorithm
 *
 * \ingroup cppcorefiles
 * \date Mar 4, 2010
 * \author alexander
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits>
#include <vector>
#include <chrono>

#include "global.h"

#include "vector2d.h"
#include "matrix.h"
#include "function.h"
#include "sparseMatrix.h"
#include "vector.h"


namespace corecvs {

template<typename MatrixClass, typename FunctionClass>
class LevenbergMarquardtImpl
{
public:
    FunctionClass *f;
    //FunctionArgs *f;
    FunctionArgs *normalisation;
    double startLambda;
    double maxLambda;
    double lambdaFactor;
    int    maxIterations;

    bool traceProgress = true;
    bool traceCrucial  = false;
    bool trace         = false;
    bool traceMatrix   = false;
    bool traceJacobian = false;
    bool useConjugatedGradient = false;
    int  conjugatedGradientIterations = 100;

    /* Additional outputs */
    bool hasParadox = false;
    int iterations = 0;

    /* The main ctor */
    LevenbergMarquardtImpl(int _maxIterations = 25, double _startLambda = 10, double _lambdaFactor = 2.0) :
        f(NULL),
        normalisation(NULL),
        startLambda(_startLambda),
        maxLambda(std::numeric_limits<double>::max()),
        lambdaFactor(_lambdaFactor),
        maxIterations(_maxIterations)
        {}

    /**
     *  Marquardt-Levenberg algoritm is a fast optimisation algorithm
     *
     *  \f[ (J^T J + \lambda diag(J^T J)) \delta = J^T [y_{target} - f(\beta)] \f]
     **/
    vector<double> fit(const vector<double> &input, const vector<double> &output)
    {
        if (traceProgress) {
            cout << "================== Starting LM fit (" << f->inputs << " -> " << f->outputs << ")================== " << endl;
            cout << "[" << std::flush;
        }

        Vector beta(input);
        Vector target(output);
        CORE_ASSERT_TRUE(f != NULL, "Function is NULL");
        CORE_ASSERT_TRUE_P(beta.size() == f->inputs, ("input guess has wrong dimension %d instead of %d\n", beta.size(), f->inputs));
        CORE_ASSERT_TRUE_P((int)output.size() == f->outputs, ("output has wrong dimension %d instead of %d\n", (int)output.size(), f->outputs));

        Vector y(f->outputs);    /**<Will hold the current function result*/
        Vector diff(f->outputs); /**<Will hold current difference to traget*/
        Vector delta(f->inputs);
        Vector yNew(f->outputs);
        Vector diffNew(f->outputs);

        FunctionArgs &F = *f;

        bool converged = false;

        double lambda = startLambda;
        double maxlambda = maxLambda;

        double norm = std::numeric_limits<double>::max();

        double totalEval = 0.0, totalJEval = 0.0, totalLinSolve = 0.0, totalATA = 0.0, totalTotal = 0.0;
        int g = 0;
        for (g = 0; (g < maxIterations) && (lambda < maxlambda) && !converged; g++)
        {
            double timeEval = 0.0, timeJEval = 0.0, timeLinSolve = 0.0, timeATA = 0.0, timeTotal = 0.0;
            auto beginT = std::chrono::high_resolution_clock::now();

            if (traceProgress) {
                if ((g % ((maxIterations / 100) + 1) == 0))
                {
                    cout << "#" << std::flush;
                }
            }

            auto Jbegin = std::chrono::high_resolution_clock::now();
            MatrixClass J = f->getNativeJacobian(&(beta[0]));
            auto Jend = std::chrono::high_resolution_clock::now();
            timeJEval += (Jend - Jbegin).count() / 1e9;

            if (traceJacobian) {
                cout << "New Jacobian:" << endl << J << endl;
            }

            auto ATAbegin = std::chrono::high_resolution_clock::now();
            /*
             * Note: Using obscure profiling techniques we found that out L-M implementation is slow
             *       for big tasks. So we changed this stuff into calls to BLAS.
             *       For small tasks it may even decrease performance since calls to BLAS come
             *       with some non-zero cost.
             *       May be we need to investigate, from which problem size we should switch to BLAS.
             *
             * NOTE: Cool guys do not compute JTJ explicitly, since we can get all useful info from
             *       J's QR decomposition (Q term cancels out and is not needed explicitly),
             *       but we are using JTJ in user-enableable ouput, so I do not implement QR-way
             */
            MatrixClass JTJ = J.ata();
            auto ATAend = std::chrono::high_resolution_clock::now();
            timeATA += (ATAend - ATAbegin).count() / 1e9;

            auto Fbegin = std::chrono::high_resolution_clock::now();
            F(beta, y);
            auto Fend = std::chrono::high_resolution_clock::now();
            timeEval += (Fend - Fbegin).count() / 1e9;

            diff = target - y;
#if 0
                for (int ijk = 0; ijk < diff.size(); ++ijk)
                    CORE_ASSERT_TRUE_S(!std::isnan(diff[ijk]));
                for (int ijk = 0; ijk < y.size(); ++ijk)
                    CORE_ASSERT_TRUE_S(!std::isnan(y[ijk]));
                for (int ijk = 0; ijk < beta.size(); ++ijk)
                    CORE_ASSERT_TRUE_S(!std::isnan(beta[ijk]));
#endif
            Vector d = diff * J;
//                for (int ijk = 0; ijk < d.size(); ++ijk)
//                    CORE_ASSERT_TRUE_S(!std::isnan(d[ijk]));

            double normOld = norm;
            norm = diff.sumAllElementsSq();

            if (trace) {
                if (normOld < norm) {
                    cout << "Paradox: Norm has not decreased..." << (normOld - norm) << endl;
                    hasParadox = true;
                }

                cout << "Now  norm:" <<  norm << "( per param:" << sqrt(norm / diff.length) << ") lambda:" << lambda << endl;
            }

            while (true)
            {

                if (norm == 0.0)
                {
                    if (traceCrucial)
                    {
                        cout << "Algorithm fully converged" << endl;
                    }
                    converged = true;
                    break;
                }

                if (!(lambda < std::numeric_limits<double>::max()))
                {
                    if (traceCrucial)
                    {
                        cout << "Algorithm seem to be trapped at point: " << endl;
                        cout << "After: " << g << " iterations" << endl;
                        cout << beta << endl;
                    }

                    if (traceMatrix)
                    {
                        cout << "Current Jacobian:" << endl << J << endl;
                        cout << "Current JTJ:" << endl << JTJ << endl;
                        cout << "previous delta was:" << endl << delta << endl;

                        int old = cout.precision(30);
                        cout << "guess:" << endl << beta << endl;
                        F(beta, yNew);
                        cout << "value:" << endl << yNew.l2Metric() << endl;
                        F(beta + delta, yNew);
                        cout << "value at +step:" << endl << yNew.l2Metric() << endl;
                        F(beta - delta, yNew);
                        cout << "value at -step:" << endl << yNew.l2Metric() << endl;
                        for (int i = 0; i < delta.size(); i++)
                        {
                            delta[i] = 0.0001 * J.a(0,i);
                        }
                        F(beta + delta, yNew);
                        cout << "value at +J:" << endl << yNew.l2Metric() << endl;
                        F(beta - delta, yNew);
                        cout << "value at -J:" << endl << yNew.l2Metric() << endl;
                        cout.precision(old);
                    }

                    break;
                }

                // Make a temporary copy
                MatrixClass A(JTJ);
                Vector B(d);

                for (int j = 0; j < A.h; j++)
                {
                    double a = A.a(j, j) + lambda;
                    //double b = A.a(j, j) * (1.0 + lambda);
                    A.a(j, j) = a;
                }

                /*
                 * NOTE: A'A (generally) is semi-positive-definite, but if you are
                 *       solving well-posed problems then diag(A'A) > 0 and A'A is
                 *       positive defined.
                 *       If you are experiencing strange problems with nans/etc,
                 *       try to make last flag "false" and examine your problem for
                 *       degeneracy
                 */

                auto LSbegin = std::chrono::high_resolution_clock::now();
//                for (int ijk = 0; ijk < B.size(); ++ijk)
//                    CORE_ASSERT_TRUE_S(!std::isnan(B[ijk]));
//                std::cout << "A.det " << A.det() << std::endl;
                if (!useConjugatedGradient)
                {
                   A.linSolve(B, delta, true, true);
                }
                else
                {
                    delta = conjugatedGradient(A, B);
                }
                auto LSend = std::chrono::high_resolution_clock::now();
//                for (int ijk = 0; ijk < delta.size(); ++ijk)
//                    CORE_ASSERT_TRUE_S(!std::isnan(delta[ijk]));
                timeLinSolve += (LSend - LSbegin).count() / 1e9;

                auto EVbegin = std::chrono::high_resolution_clock::now();
                F(beta + delta, yNew);
                auto EVend = std::chrono::high_resolution_clock::now();
                timeEval += (EVend - EVbegin).count() / 1e9;

                diffNew = target - yNew;
                double normNew = diffNew.sumAllElementsSq();
                if (trace) {
                    cout << "  Guess:" <<  normNew << " - ";
                }

                if (normNew < norm) // If the current solution is better
                {
                    if (trace) {
                        cout << "Accepted" << endl;
                    }

                    if (traceMatrix) {
                        cout << "Old soluton:" << endl << beta << endl;
                    }

                    lambda /= lambdaFactor;
                    norm = normNew;
                    beta += delta;
                    if (normalisation != NULL)
                    {
                        Vector normBeta(beta.size());
                        normalisation->operator()(beta, normBeta);
                        beta = normBeta;
                    }

                    if (traceMatrix) {
                        cout << "New soluton:" << endl << beta << " - " << normNew << endl;
                    }
                    break;
                }
                else
                {
                    if (trace) {
                        cout << "Rejected lambda old: "<< lambda << " lambda new:" << lambda * lambdaFactor << endl;
                    }
                    lambda *= lambdaFactor; // Current solution is worse. Try new lambda
                    if (traceMatrix) {
                        cout << "Keep soluton:" << endl << beta << " - " << normNew << " l:"<<  lambda << endl;
                    }
                }
            }
            auto Tend = std::chrono::high_resolution_clock::now();
            timeTotal = (Tend - beginT).count() / 1e9;
    #if 0
            if (traceProgress)
            {
                std::cout << "Total : " << timeTotal    << "s " << std::endl
                          << "Eval  : " << timeEval     << "s (" << timeEval     / timeTotal * 100.0 << ")" << std::endl
                          << "JEval : " << timeJEval    << "s (" << timeJEval    / timeTotal * 100.0 << ")" << std::endl
                          << "ATA   : " << timeATA      << "s (" << timeATA      / timeTotal * 100.0 << ")" << std::endl
                          << "LS    : " << timeLinSolve << "s (" << timeLinSolve / timeTotal * 100.0 << ")" << std::endl
                          << "Other : " << (timeTotal - timeEval - timeJEval - timeATA - timeLinSolve) << "s (" << (timeTotal - timeEval - timeJEval - timeATA - timeLinSolve) / timeTotal * 100.0 << ")" << std::endl;
            }
    #endif
            totalTotal += timeTotal;
            totalEval += timeEval;
            totalJEval += timeJEval;
            totalATA += timeATA;
            totalLinSolve += timeLinSolve;
        }

            if (traceProgress)
            {
                std::cout << "Total : " << totalTotal    << "s " << std::endl
                          << "Eval  : " << totalEval     << "s (" << totalEval     / totalTotal * 100.0 << ")" << std::endl
                          << "JEval : " << totalJEval    << "s (" << totalJEval    / totalTotal * 100.0 << ")" << std::endl
                          << "ATA   : " << totalATA      << "s (" << totalATA      / totalTotal * 100.0 << ")" << std::endl
                          << "LS    : " << totalLinSolve << "s (" << totalLinSolve / totalTotal * 100.0 << ")" << std::endl
                          << "Other : " << (totalTotal - totalEval - totalJEval - totalATA - totalLinSolve) << "s (" << (totalTotal - totalEval - totalJEval - totalATA - totalLinSolve) / totalTotal * 100.0 << ")" << std::endl;
            }

        if (traceProgress) {
            cout << "]" << endl;
        }

        vector<double> result;
        result.reserve(f->inputs);
        for (int i = 0; i < f->inputs; i++) {
            result.push_back(beta[i]);
        }

        iterations = g;
        return result;
    }

    corecvs::Vector conjugatedGradient(const MatrixClass &A, const corecvs::Vector &B)
    {
        corecvs::Vector X(A.w), R = B, p = B;
        double rho0 = B.sumAllElementsSq(), rho1 = 0.0;
        for (int i = 0; i < A.w; ++i) X[i] = 0.0;

        static int cgf = 0, cgo = 0;

        std::cout << "PRECG: " << !(A*X-B) << std::endl;
        double eps = 1e-9 * rho0;
        for (int i = 0; i < conjugatedGradientIterations; ++i)
        {
            p = i ? R + (rho0 / rho1) * p : R;
            auto w = A * p;
            double alpha = rho0 / (w & p);
            X = X + alpha * p;
            R = A * X - B;
            rho1 = rho0;
            rho0 = R.sumAllElementsSq();
            // XXX: sometimes matrices are close to degenerate, so
            //      CG-step may fail
            if (rho0 < eps)
            {
                break;
            }
            if (rho1 < rho0)
                cgf++;
            else
                cgo++;
        }
        if ((A*X-B).sumAllElementsSq() > B.sumAllElementsSq())
        {
            corecvs::Vector v;
            A.linSolve(B, v);
            return v;
        }
        std::cout << "POST-CG: " << !(A*X-B) << " cg failures: " << ((double)cgf) / (double)((cgf + cgo + 1.0)) * 100.0 << "%" << std::endl;
        return X;
    }

};

typedef LevenbergMarquardtImpl<Matrix, FunctionArgs> LevenbergMarquardt;
typedef LevenbergMarquardtImpl<SparseMatrix, SparseFunctionArgs> LevenbergMarquardtSparse;

} //namespace corecvs
#endif

