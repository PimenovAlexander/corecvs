/*
 * main.cpp
 *
 *  Created on: Dec 7, 2010
 *      Author: alexander
 */

#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"

#include "matrix.h"
#include "classicKalman.h"

using namespace corecvs;

/**
 *  an example form here
 *  http://ru.wikipedia.org/wiki/%D0%A4%D0%B8%D0%BB%D1%8C%D1%82%D1%80_%D0%9A%D0%B0%D0%BB%D0%BC%D0%B0%D0%BD%D0%B0#.D0.9F.D1.80.D0.B8.D0.BC.D0.B5.D1.80.D1.8B_.D0.B8.D1.81.D0.BF.D0.BE.D0.BB.D1.8C.D0.B7.D0.BE.D0.B2.D0.B0.D0.BD.D0.B8.D1.8F
 *
 *
 *
*  UDUT decomposition for
*        [  2 ]
*        [ t  ]
*        [ -- ]
*   G =  [ 2  ]
*        [    ]
*        [ t  ]
*
*           [  4   3 ]
*           [ t   t  ]
*           [ --  -- ]
*    T      [ 4   2  ]
* G*G   =   [        ]
*           [  3     ]
*           [ t    2 ]
*           [ --  t  ]
*           [ 2      ]
*
*  Decomposition of the matrix is :
*
*       T        [ 1  a/b ]   [ 0  0  ]   [ 1     0 ]
*  U D U    =    [        ] . [       ] . [         ]
*                [        ]   [     2 ]   [         ]
*                [ 0    1 ]   [ 0  a  ]   [ a/b   1 ]
*
*
 * */

#define GNUPLOT_TRACE


class TestF : public FunctionArgs
{
public:
    static const double TIME_QUANT;

    TestF() :  FunctionArgs(2,2) {};
    /**
     *   /f[ x = x + vt\f]
     *
     **/
    virtual void operator()(const double in[], double out[])
    {
        out[0] = in[0] + TIME_QUANT * in[1];
        out[1] = in[1];
    }
};

const double TestF::TIME_QUANT = 1;


class TestH : public FunctionArgs
{
public:
    TestH() :  FunctionArgs(2,1) {};
    virtual void operator()(const double in[], double out[])
    {
        out[0] = in[0];
    }
};

TEST(Kalman, testClassicKalman)
{
    static const int STATE_DIMENTION = 2;
    static const int MEASURE_DIMENTION = 1;

    TestF F;
    TestH H;

    double x1[2] = {1.0, 1.0};
    cout << "F Jacobian" << endl << F.getJacobian(x1) << endl;

    double x2[2] = {1.0};
    cout << "H Jacobian" << endl << H.getJacobian(x2) << endl;

    Matrix P(STATE_DIMENTION,STATE_DIMENTION);
    P.fillWithArgs(
            0.0, 0.0,
            0.0, 0.0 );

    Matrix Q(STATE_DIMENTION,STATE_DIMENTION);
    Q.fillWithArgs(
            0.25, 0.5,
            0.5 , 1.0);
    Q *= 0.25;

    Matrix R(MEASURE_DIMENTION,MEASURE_DIMENTION);
    R.fillWithArgs(10.0);

    Vector x(2);
    x[0] = 0.0;
    x[1] = 0.0;


    ClassicKalman kalmanFilter(
    &F,
    &H,
    Q,
    R,
    P,
    x);


#ifdef GNUPLOT_TRACE
    FILE *fp = fopen("gnuplot1.dat", "wt");
    fprintf(fp,"#Step; Measurement; Old Position; Old Speed; Predicted Position; Predicted Speed; Innovated Position; Innovated Speed\n");
#endif

    int step;
    for (step = 0; step < 25; step++)
    {
        double NOISE_MAG = 0.0;
        double measurement = (step + 1) * 0.2 + (NOISE_MAG * ((rand() % 1000) / 1000.0)) - (NOISE_MAG / 2.0);
        kalmanFilter.predict();

        kalmanFilter.z[0] = measurement;
        kalmanFilter.update();


#ifdef GNUPLOT_TRACE
        fprintf(fp,"%d; %lg;     %lg; %lg;        %lg; %lg       %lg; %lg\n",
                step,
                measurement,
                0.0, 0.0,
                kalmanFilter.xPredicted[0], kalmanFilter.xPredicted[1],
                kalmanFilter.x[0], kalmanFilter.x[1]);

#else
        printf("Measurement     : x=%lg\n", measurement);
        printf("Old        State: x=%lg v=%lg\n", 0.0, 0.0);
        printf("Predicted  State: x=%lg v=%lg\n", kalmanFilter.);
        printf("Current    State: x=%lg v=%lg\n", kalmanFilter.currentState->a(0), kalmanFilter.currentState->a(1));
#endif
    }
#ifdef GNUPLOT_TRACE
    fclose(fp);
    int gnuplotResult = system("gnuplot data/gnuplot.pg > out.png;");
    printf("GNUPLOT result is %d\n", gnuplotResult);
#endif

}

//int main (int /*argC*/, char ** /*argV[]*/)
//{
//    testClassicKalman ();
//}
