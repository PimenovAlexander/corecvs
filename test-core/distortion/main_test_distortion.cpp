/**
 * \file main_test_distortion.cpp
 * \brief This is the main file for the test distortion 
 *
 * \date Jun 07, 2013
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/alignment/anglePointsFunction.h"
#include "core/math/matrix/matrix.h"
#include "core/alignment/radialCorrection.h"
#include "core/alignment/radialFunc.h"


using corecvs::AnglePointsFunction;
using corecvs::FunctionArgs;
using corecvs::Vector2dd;
using corecvs::Matrix;
using corecvs::RadialCorrection;
using corecvs::RadialFunc;


TEST(Distortion, testRadialModel)
{
   vector<vector<Vector2dd> > samples;
   vector<Vector2dd> line;
   line.push_back(Vector2dd(10.0, 10.0));
   line.push_back(Vector2dd(10.1,  5.0));
   line.push_back(Vector2dd(10.1, -5.0));
   line.push_back(Vector2dd(10.0,-10.0));
   samples.push_back(line);

   Vector2dd center(1.0, 0.0);

   vector<double> in(4, 0);
   in[0] = 0.001;
   in[1] = 0.00001;
   in[2] = 0.0000001;
   in[3] = 0.000000001;

   //RadialCorrection model = RadialFunc::inputToCorrection(&(in[0]), 4);
   //model.mParams.center = center;
   //model.mParams.p1 = 0;
   //model.mParams.p2 = 0;
   //model.mParams.aspect = 1.0;
   //model.mParams.focal = 20.0;

   vector<Vector2dd> out(line.size(), Vector2dd(0));

   //for (unsigned i = 0; i < line.size(); i++)
   //{
   //    out[i] = model.map(line[i].y(), line[i].x());
   //    cout << setprecision(15) << out[i] << " ,";
   //}
   //cout << endl;

   /*=================*/

   RadialFunc f(line, center, 4);
   f.setScaleFactor(1.0);
   f(&(in[0]), &(out[0].x()));
   for (unsigned i = 0; i < line.size(); i++)
   {
       std::cout << std::setprecision(15) << out[i] << " ,";
   }
   std::cout << std::endl;
}


TEST(Distortion, testRadialJacobian)
{
    vector<vector<Vector2dd> > samples;

    vector<Vector2dd> line;
    line.push_back(Vector2dd(10.0, 10.0));
    line.push_back(Vector2dd(10.1,  5.0));
    line.push_back(Vector2dd(10.1, -5.0));
    line.push_back(Vector2dd(10.0,-10.0));
    samples.push_back(line);

    Vector2dd center(1.0, 0.0);

    LensDistortionModelParameters params;
    RadialCorrection lockedDimentions(params);
    bool guessCenter = false;
    bool guessTangent = false;
    int  polynomialDegree = false;
    bool evenDegreeOnly = false;
    ModelToRadialCorrection modelFactory(lockedDimentions, guessCenter, guessTangent, polynomialDegree, evenDegreeOnly); // center, 4, 1.0);

    AnglePointsFunction *function = new AnglePointsFunction(samples, modelFactory);
    vector<double> in(function->inputs, 0);

    Matrix classicJacobian = function->FunctionArgs::getJacobian(&(in[0]));
    std::cout << "Classic Jacobian" << std::endl
        << classicJacobian;

    Matrix manualJacobian = function->getJacobian(&(in[0]));
    std::cout << "Manual Jacobian" << std::endl
        << manualJacobian;

    delete function;
}
