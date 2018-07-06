#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

//#include <QtCore/QCoreApplication>
#include "core/alignment/curvatureFunc.h"
#include "core/math/levenmarq.h"
#include "core/alignment/radialFunc.h"

using namespace corecvs;

//p1 = p2 = 0;

TEST(PolynomDistortion, DISABLED_testAproximation)  // commented as it fails!
{
    double k1 = 15 * 1e-9;
    double k2 = 5 * 1e-17;
//    double k3 = 7 * 1e-16;
//    double k4 = 9 * 1e-18;
//    double k5 = 3 * 1e-20;
    double p1 = 0;
    double p2 = 0;
    //double k3 = 1 * 1e-15;
    double polynomDegree = 2;
    //double scale = 1;
    vector<Vector2dd> undistortedPoints;
    for (int i = 0; i < 100; i += 10)
    {
        for (int j = 0; j < 100; j += 10)
        {
            undistortedPoints.push_back(Vector2dd(j, i));
        }
    }
    Vector2dd center(50, 50);
    RadialFunc radFunc(undistortedPoints, center, polynomDegree);
    Vector distortedPoints((int)undistortedPoints.size() * 2);
    Vector params(polynomDegree + 2);
    params[0] = k1;
    params[1] = k2;
//    params[2] = k3;
//    params[3] = k4;
//    params[4] = k5;
    params[2] = p1;
    params[3] = p2;
    radFunc(params, distortedPoints);
    LevenbergMarquardt lmTransform(10000);
    lmTransform.f = &radFunc;
    vector<double> beginParams(polynomDegree + 2, 0);
    vector<double> distortedPointsVector;
    for (unsigned i = 0; i < undistortedPoints.size() * 2; i ++)
    {
        distortedPointsVector.push_back(distortedPoints[i]);
    }
    vector<double> finishParams  = lmTransform.fit(beginParams, distortedPointsVector);
    for (unsigned i = 0; i < finishParams.size(); i ++)
    {
        cout << finishParams.at(i) << endl;
    }
}

//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);
    
//    testAproximation();
//    return a.exec();
//}
