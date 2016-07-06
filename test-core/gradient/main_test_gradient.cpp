/**
 * \file main_test_gradient.cpp
 * \brief This is the main file for the test gradient 
 *
 * \date Oct 20, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "mathUtils.h"
#include "gradientDescent.h"
#include "vector3d.h"

using namespace std;
using namespace corecvs;

/**
 *   Computes:
 *   \f[F(x,y,z) = (x - x_{min})^2 + (y - y_{min})^2 + (z - z_{min})^2 \f]
 **/
class GradientTest : FunctionArgs {
public:
    Vector3dd min;

    GradientTest(Vector3dd _min) :
        FunctionArgs(3,1),
        min(_min)
        {};

    virtual void operator()(const double in[], double out[])
    {
       Vector3dd value(in[0], in[1], in[2]);
       value -= min;
       out[0] = value.l2Metric();;
    };

};

class GradientRnToRmTest : FunctionArgs {
public:
    GradientRnToRmTest() : FunctionArgs(2,10) {};

    virtual void operator()(const double in[], double out[])
    {
       double alpha = in[0];
       double beta  = in[1];

       for (int i = 0; i < 10; i ++)
       {
           double x = M_PI / 10.0 * i;
           out[i] = alpha * cos (beta * x) + beta * sin (alpha * x);
       }
    }
};

class GradientRnToRm2Test : FunctionArgs {
public:
    GradientRnToRm2Test() : FunctionArgs(3,3) {};

    virtual void operator()(const double in[], double out[])
    {
        out[0] = 3 * in[0];
        out[0] -= cos(in[1] * in[2]);
        out[0] -= 1.5;

        out[1] = 4 * in[0]*in[0];
        out[1] -= 625 * in[1] * in[1];
        out[1] += 2 * in[1];
        out[1] -= 1;

        out[2] = exp((-1) * in[0] * in[1]);
        out[2] += 20 * in[2];
        out[2] += (28.41592654/3);
    }
};

TEST(Gradient, testGradientDescent)
{
    Vector3dd min(1.0, -1.5, 2.0);
    GradientTest function(min);

    GradientDescent optimiser;

    optimiser.f = (FunctionArgs *)&function;
    optimiser.lambda = 0.01;
    optimiser.maxIterations = 3000;


    vector<double> startguess(3);
    startguess[0] = 0.0;
    startguess[1] = 0.0;
    startguess[2] = 0.0;

    vector<double> target(1);
    target[0] = 0.0;

    vector<double> resultMin = optimiser.minimise(startguess);
    Vector3dd resMin(resultMin[0], resultMin[1], resultMin[2]);

    optimiser.lambda = 0.1;
    optimiser.maxIterations = 100000;
    vector<double> result = optimiser.fit(startguess, target);
    Vector3dd res(result[0], result[1], result[2]);

    cout << "Minimum Answer:" << resMin << endl;
    cout << "        Answer:" << res    << endl;

    CORE_ASSERT_TRUE(resMin.notTooFar(min, 1e-2), "Minimum search failed");
    CORE_ASSERT_TRUE(res.notTooFar(min, 1e-2), "GD opt failed");
}

TEST(Gradient, DISABLED_testGradientDescentRnToRm)
{
    GradientRnToRmTest function;

    GradientDescentRnToRm optimiser;

    optimiser.f = (FunctionArgs *)&function;
    optimiser.lambda = 0.0001;
    optimiser.maxIterations = 500000;

    double toguess[2] = {100.2, 105.1};
    vector<double> target(10);
/*
    target[0] += 0.5;
    target[1] -= 0.5;
*/
    function(toguess, &(target[0]));

    vector<double> startguess(2);
    startguess[0] = 100.1;
    startguess[1] = 105.8;

    vector<double> result = optimiser.fit(startguess, target);

    cout << "Answer:" << endl;
    for (int i = 0; i < 2; i++)
        cout << result[i] << ",";
    cout << endl;

    for (int i = 0; i < 10; i++)
        cout << target[i] << ",";
    cout << endl;

    CORE_ASSERT_DOUBLE_EQUAL_E(result[0], toguess[0], 1e-7, "Wrong result");
    CORE_ASSERT_DOUBLE_EQUAL_E(result[1], toguess[1], 1e-7, "Wrong result");
}

TEST(Gradient, testGradientDescentRnToRm2)
{
    GradientRnToRm2Test function;

    GradientDescentRnToRm optimiser;

    optimiser.f = (FunctionArgs *)&function;
    optimiser.maxIterations = 100;

    double toguess[3] = {0, 0, 0};
    vector<double> target(3);
    function(toguess, &(target[0]));
    for (int i = 0; i < 3; i++)
        cout << target[i] << ",";
    cout << endl;

    target[0] = 0;
    target[1] = 0;
    target[2] = 0;


    vector<double> startguess(3);
    startguess[0] = 0;
    startguess[1] = 0;
    startguess[2] = 0;

    vector<double> result = optimiser.fit(startguess, target);

    cout << "Answer:" << endl;
    for (int i = 0; i < 3; i++)
        cout << result[i] << ",";
    cout << endl;

    for (int i = 0; i < 3; i++)
        cout << target[i] << ",";
    cout << endl;

}

//int main (int /*argC*/, char ** /*argV*/)
//{
////    testGradientDescent();
//    testGradientDescentRnToRm();
////    testGradientDescentRnToRm2();
//        cout << "PASSED" << endl;
//        return 0;
//}
