/**
 * \file main_test_derivative.cpp
 * \brief This is the main file for the test derivative 
 *
 * \date Jun 06, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "function.h"

using namespace corecvs;

class SineCosine : public FunctionArgs
{
public:
    SineCosine() : FunctionArgs(2,2) {};

    virtual void operator()(const double in[], double out[])
    {
        double sum = in[0] + in[1];
        out[0] = sin(sum);
        out[1] = cos(sum);
    };
};

TEST(Derivative, testFunction)
{
    SineCosine f;
    double x[] = {1.0, 0.5};
    double y[2];
    f(x,y);
    cout << y[0] << " " << y[1] << endl;
    Matrix J = f.getJacobian(x);
    cout << "Jacobian" << endl;
    cout << J;
}
