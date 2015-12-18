/**
 * \file main_test_levenberg.cpp
 * \brief This is the main file for the test levenberg 
 *
 * \date Jul 05, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "function.h"
#include "levenmarq.h"
#include "helperFunctions.h"
#include "bmpLoader.h"

using namespace std;
using namespace corecvs;


class LevenbergTest : FunctionArgs {
public:
    LevenbergTest() : FunctionArgs(2,10) {};

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

TEST(Levenberg, testMarquardtLevenberg)
{
    LevenbergTest function;

    LevenbergMarquardt optimiser;

    optimiser.f = (FunctionArgs *)&function;
    optimiser.maxIterations = 20;

    double toguess[2] = {100.2, 105.1};
    vector<double> target(10);

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

TEST(Levenberg, DISABLED_plotRosenberg)
{
    const int STEPS = 40;
    FILE *Out = fopen("rosenberg.txt", "wt");
    RosenbrockFunction func;
    for (int i = 0; i < STEPS; i++)
    {
        for (int j = 0; j < STEPS; j++)
        {
            double x = ((double) i / (STEPS - 1));
            double y = ((double) j / (STEPS - 1));
            double in[2];
            double out[1];
            in[0] = x * 4.0 - 2.0;
            in[1] = y * 4.0 - 1.0;
            func.operator ()(in, out);
            fprintf(Out, "%lf %lf %lf\n", in[0], in[1], out[0]);
        }
    }

    ScalerFunction scaled(&func, 0.01, 0.0);
    RGB24Buffer *image = new RGB24Buffer(500,500);
    image->drawFunction(-1.0, -2.0, 4.0, 4.0, scaled);
    image->drawIsolines(-1.0, -2.0, 4.0, 4.0, 10, scaled);
    BMPLoader().save("rosenplot.bmp", image);
    delete image;

    fclose(Out);
}
