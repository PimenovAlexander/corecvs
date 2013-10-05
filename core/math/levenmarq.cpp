#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits>

#define TRACE_PROGRESS
//#define TRACE

#include "global.h"

#include "levenmarq.h"
#include "stdlib.h"
#include "vector.h"
namespace corecvs {

using std::flush;
/**
 *  Marquardt-Levenberg algoritim is a fast optimisation algorithm
 *
 *  \f[ (J^T J + \lambda diag(J^T J)) \delta = J^T [y_{target} - f(\beta)] \f]
 **/
vector<double> LevenbergMarquardt::fit(const vector<double> &input, const vector<double> &output)
{
#ifdef TRACE_PROGRESS
    cout << "================== Starting LM fit ================== " << endl;
    cout << "[" << flush;
#endif

    Vector beta(input);
    Vector target(output);
    ASSERT_TRUE  ( f != NULL, "Function is NULL");
    ASSERT_TRUE_P( beta.size() == f->inputs, ("input guess has wrong dimension %d instead of %d\n", beta.size(), f->inputs));
    ASSERT_TRUE_P( (int)output.size() == f->outputs, ("output has wrong dimension %d instead of %d\n", (int)output.size(), f->outputs));

    Vector y(f->outputs);    /**<Will hold the current function result*/
    Vector diff(f->outputs); /**<Will hold current difference to traget*/
    Vector delta(f->inputs);
    Vector yNew(f->outputs);
    Vector diffNew(f->outputs);

    FunctionArgs &F = *f;

    bool converged = false;

    double lambda = startLambda;

    for (int g = 0; g < maxIterations && lambda < std::numeric_limits<double>::max() && !converged; g++)
    {
#ifdef TRACE_PROGRESS
        if ((g % ((maxIterations / 100) + 1) == 0))
        {
            cout << "#" << flush;
        }
#endif
        Matrix J = f->getJacobian(&(beta[0]));
#ifdef TRACE
        cout << "New Jacobian:" << endl << J << endl;
#endif
        Matrix JT = J.t();
        Matrix JTJ = JT * J;

        F(beta, y);
        diff = target - y;
        Vector d = JT * diff;

        double norm = diff.sumAllElementsSq();
#ifdef TRACE
        cout << "Now  :" <<  norm << " " << lambda << endl;
#endif
        while (true)
        {

            if (norm == 0.0)
            {
#ifdef TRACE_PROGRESS
                cout << "Algorithm fully converged" << endl;
#endif
                converged = true;
                break;
            }

            if (!(lambda < std::numeric_limits<double>::max()))
            {
#ifdef TRACE_PROGRESS
                cout << "Algorithm seem to be trapped at point: " << endl;
                cout << "After: " << g << " iterations" << endl;
                cout << beta << endl;
#endif
#ifdef TRACE
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
#endif
                break;
            }

            // Make a temporary copy
            Matrix A(JTJ);
            Vector B(d);


            for (int j = 0; j < A.h; j++)
            {
                double a = A.a(j, j) + lambda;
                //double b = A.a(j, j) * (1.0 + lambda);
                A.a(j, j) = a;
            }

            delta = A.inv() * B;
            F(beta + delta, yNew);
            diffNew = target - yNew;
            double normNew = diffNew.sumAllElementsSq();
#ifdef TRACE
            cout << "  Guess:" <<  normNew << " - ";
#endif

            if (normNew < norm) // If the current solution is better
            {
#ifdef TRACE
                cout << "Accepted" << endl;
#endif
                lambda /= lambdaFactor;
                norm = normNew;
                beta += delta;
                if (normalisation != NULL)
                {
                    Vector normBeta(beta.size());
                    normalisation->operator()(beta, normBeta);
                    beta = normBeta;
                }
                break;
            }
            else
            {
#ifdef TRACE
                cout << "Rejected lambda up" << endl;
#endif
                lambda *= lambdaFactor;// Current solution is worse. Try new lambda
            }
        }
    }

#ifdef TRACE_PROGRESS
    cout << "]" << endl;
#endif

    vector<double> result;
    result.reserve(f->inputs);
    for (int i = 0; i < f->inputs; i++)
        result.push_back(beta[i]);
    return result;
}

} //namespace corecvs

