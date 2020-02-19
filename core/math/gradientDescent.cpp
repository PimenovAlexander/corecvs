/**
 * \file gradientDescent.cpp
 * \brief Add Comment Here
 *
 * \date Oct 20, 2011
 * \author alexander
 */

#include "math/gradientDescent.h"
namespace corecvs {

using std::endl;
using std::flush;

vector<double> GradientDescent::minimise(const vector<double> &input)
{
    cout << "================== Starting DG minimize ================== " << endl;
    cout << "Lambda is: "<< lambda << endl;
    cout << "[" << flush;
    Vector beta(input);
    CORE_ASSERT_TRUE(f != NULL, "Function is NULL");
    CORE_ASSERT_TRUE(f->outputs == 1, "Function should map to one-dimensional space");
    CORE_ASSERT_TRUE_P(beta.size() == f->inputs, ("input guess has wrong dimension %d instead of %d\n", beta.size(), f->inputs));

    for (int g = 0; g < maxIterations; g++)
    {
        if ((g % ((maxIterations / 100) + 1) == 0))
        {
//            double d;
            cout << "#" << flush;
//            f->operator ()(&(beta[0]), &d);
//            streamsize wasPrecision = cout.precision(10);
//            cout << d << endl;
//            cout.precision(wasPrecision);
        }
//        cout << "Current Value:" << endl << beta << endl;

        Matrix J = f->getJacobian(&(beta[0]));
//        cout << "Current Gradient:" << endl << J << endl;

        double addition = 0.0;
        double gradNorm = 0.0;
        for (int i = 0; i < beta.size(); i++)
        {
            double gradx = J.a(0, i);
            double delta = lambda * gradx;
            gradNorm += gradx * gradx;
            addition += fabs(delta);
            beta[i] -= delta;
        }

        if (normalisation != NULL)
        {
            Vector normBeta(beta.size());
            normalisation->operator()(beta, normBeta);
            beta = normBeta;
        }

        if (addition == 0.0 ||  gradNorm < minGradient)
        {
            cout << "Algorithm reached saddle point or converged" << endl;
            break;
        }
    }

    vector<double> result;
    result.reserve(f->inputs);
    for (int i = 0; i < f->inputs; i++)
        result.push_back(beta[i]);

    cout << "]" << endl;

    return result;
}


vector<double> GradientDescent::fit(const vector<double> &input, const vector<double> &output)
{
    cout << "================== Starting Gradient fit ================== " << endl;
    Vector beta(input);
    CORE_ASSERT_TRUE(f != NULL, "Function is NULL");
    CORE_ASSERT_TRUE(f->outputs == 1, "Function should map to one-dimensional space");
    CORE_ASSERT_TRUE_P(beta.size() == f->inputs, ("input guess has wrong dimension %d instead of %d\n", beta.size(), f->inputs));
    CORE_ASSERT_TRUE_P((int)output.size() == f->outputs, ("output has wrong dimension %d instead of %d\n", (int)output.size(), f->outputs));

    for (int g = 0; g < maxIterations; g++)
    {

        Matrix J = f->getJacobian(&(beta[0]));
#ifdef TRACE
        cout << "Current Value:" << endl << beta << endl;
        cout << "Current Gradient:" << endl << J << endl;
#endif

        Vector y(1);
        f->operator ()(beta, y);
        double difference = y[0] - output[0];

#ifdef TRACE
        cout << "Difference:" << difference << endl;
#endif

        for (int i = 0; i < beta.size(); i++)
        {
            beta[i] -= lambda * J.a(0, i) * (difference);
        }

        if (normalisation != NULL)
        {
            Vector normBeta(beta.size());
            normalisation->operator()(beta, normBeta);
            beta = normBeta;
        }
    }

    vector<double> result;
    result.reserve(f->inputs);
    for (int i = 0; i < f->inputs; i++)
        result.push_back(beta[i]);
    return result;
}

/**
 *
 *
 *
 **/
vector<double> GradientDescentRnToRm::fit(const vector<double> &input, const vector<double> &output)
{
    cout << "================== Starting Gradient Rn->Rm fit ================== " << endl;
    Vector beta(input);
    CORE_ASSERT_TRUE(f != NULL, "Function is NULL");
    CORE_ASSERT_TRUE_P(beta.size() == f->inputs, ("input guess has wrong dimension %d instead of %d\n", beta.size(), f->inputs));
    CORE_ASSERT_TRUE_P((int)output.size() == f->outputs, ("output has wrong dimension %d instead of %d\n", (int)output.size(), f->outputs));

    /**Current value of the function */
    Vector F(f->outputs);
    Vector target(output);

    for (int g = 0; g < maxIterations; g++)
    {
        Matrix J = f->getJacobian(&(beta[0]));
        f->operator()(beta, F);
#ifdef TRACE
        cout << "Current Guess:" << endl << beta << endl;
        cout << "Current Jacobian:" << endl << J << endl;
        cout << "Current Result:" << endl << F << endl;
#endif
        F -= target;

#ifdef TRACE
        cout << "Current Error:" << endl << F << endl;
#endif

        /* J^T * F = (F^T * J)^T */
        Vector corrector = F * J;
#ifdef TRACE
        cout << "Corrector:" << endl << corrector << endl;
#endif

        beta -= (lambda * (1.0 / f->outputs)) * corrector;

        if (normalisation != NULL)
        {
            Vector normBeta(beta.size());
            normalisation->operator()(beta, normBeta);
            beta = normBeta;
        }
    }

    vector<double> result;
    result.reserve(f->inputs);
    for (int i = 0; i < f->inputs; i++)
        result.push_back(beta[i]);
    return result;
}

} //namespace corecvs

