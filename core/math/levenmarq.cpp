#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits>

#include "global.h"

#include "levenmarq.h"
#include "stdlib.h"
#include "vector.h"

#ifdef WITH_BLAS
#ifdef WITH_MKL
#include <mkl.h>
#else
#  include <complex>
#  define lapack_complex_float  std::complex<float>
#  define lapack_complex_double std::complex<double>
#  include <cblas.h>
#  include <lapacke.h>
#endif
#endif

namespace corecvs {

//#define TRACE_PROGRESS
//#define TRACE_CRUCIAL
//#define TRACE
//#define TRACE_MATRIX


using std::flush;
/**
 *  Marquardt-Levenberg algoritim is a fast optimisation algorithm
 *
 *  \f[ (J^T J + \lambda diag(J^T J)) \delta = J^T [y_{target} - f(\beta)] \f]
 **/
vector<double> LevenbergMarquardt::fit(const vector<double> &input, const vector<double> &output)
{
    if (traceProgress) {
        cout << "================== Starting LM fit ================== " << endl;
        cout << "[" << flush;
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

    int g = 0;
    for (g = 0; (g < maxIterations) && (lambda < maxlambda) && !converged; g++)
    {
        if (traceProgress) {
            if ((g % ((maxIterations / 100) + 1) == 0))
            {
                cout << "#" << flush;
            }
        }

        Matrix J = f->getJacobian(&(beta[0]));

        if (traceJacobian) {
            cout << "New Jacobian:" << endl << J << endl;
        }

        /*
         * XXX: Using obscure profiling techniques I found that out L-M implementation is slow
         *      for big tasks. So I changed this stuff into calls to BLAS.
         *      For small tasks it may even decrease performance since calls to BLAS come
         *      with some non-zero cost.
         *      May be we need to investigate, from which problem size we should switch to BLAS
         *      implementation.
         * NOTE: Cool guys do not compute JTJ explicitly, since we can get all useful info from
         *       J's QR decomposition (Q term cancels out and is not needed explicitly),
         *       but we are using JTJ in user-enableable ouput, so I do not implement QR-way
         */
#ifndef WITH_BLAS
        Matrix JT = J.t();
        Matrix JTJ = JT * J;
#else
        Matrix JTJ(J.w, J.w);
        cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, J.w, J.w, J.h, 1.0, &J.a(0, 0), J.stride, &J.a(0, 0), J.stride, 0.0, &JTJ.a(0, 0), JTJ.stride);
#endif

        F(beta, y);
        diff = target - y;
#ifndef WITH_BLAS
        Vector d = JT * diff;
#else
        Vector d(J.w);
        cblas_dgemv(CblasRowMajor, CblasTrans, J.h, J.w, 1.0, &J.a(0, 0), J.stride, &diff[0], 1, 0.0, &d[0], 1);
#endif

        double normOld = norm;
        norm = diff.sumAllElementsSq();

        if (trace) {
            if (normOld < norm) {
                cout << "Paradox: Norm has not decreased..." << (normOld - norm) << endl;
                hasParadox = true;
            }

            cout << "Now  :" <<  norm << " " << lambda << endl;
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
            Matrix A(JTJ);
#ifndef WITH_BLAS
            Vector B(d);
#endif


            for (int j = 0; j < A.h; j++)
            {
                double a = A.a(j, j) + lambda;
                //double b = A.a(j, j) * (1.0 + lambda);
                A.a(j, j) = a;
            }

            /*
             * XXX: Using obscure profiling techniques I found that out L-M implementation is slow
             *      for big tasks. So I changed this stuff into calls to LAPACK.
             *      For small tasks it may even decrease performance since calls to LAPACK come
             *      with some non-zero cost.
             *      May be we need to investigate, from which problem size we should switch to LAPACK
             *      implementation.
             */
#ifndef WITH_BLAS
            delta = A.inv() * B;
#else
            Vector pivot(A.h);
            delta = d;
            LAPACKE_dsysv( LAPACK_ROW_MAJOR, 'L', A.h, 1, &A.a(0, 0), A.stride, (int*)&pivot[0], &delta[0], 1);
#endif
            F(beta + delta, yNew);
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

} //namespace corecvs

