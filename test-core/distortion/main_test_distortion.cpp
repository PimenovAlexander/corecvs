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

#include "global.h"

#include "vector2d.h"
#include "anglePointsFunction.h"
#include "matrix.h"
#include "radialCorrection.h"
#include "radialFunc.h"


using namespace std;
using corecvs::AnglePointsFunction;
using corecvs::FunctionArgs;
using corecvs::Vector2dd;
using corecvs::Matrix;
using corecvs::RadialCorrection;
using corecvs::RadialFunc;

#if 0
void testRadialModel(void)
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

   RadialCorrection model = RadialFunc::inputToCorrection(&(in[0]), 4);
   model.mParams.center = center;
   model.mParams.p1 = 0;
   model.mParams.p2 = 0;
   model.mParams.aspect = 1.0;
   model.mParams.focal = 20.0;

   vector<Vector2dd> out(line.size(), Vector2dd(0));

   for (unsigned i = 0; i < line.size(); i++)
   {
       out[i] = model.map(line[i].y(), line[i].x());
       cout << setprecision(15) << out[i] << " ,";
   }
   cout << endl;

   /*=================*/

   RadialFunc f(line, center, 4);
   f.setScaleFactor(1.0);
   f(&(in[0]), &(out[0].x()));
   for (unsigned i = 0; i < line.size(); i++)
   {
       cout << setprecision(15) << out[i] << " ,";
   }
   cout << endl;

}
#endif

void testRadialJacobian(void)
{
    vector<vector<Vector2dd> > samples;

    vector<Vector2dd> line;
    line.push_back(Vector2dd(10.0, 10.0));
    line.push_back(Vector2dd(10.1,  5.0));
    line.push_back(Vector2dd(10.1, -5.0));
    line.push_back(Vector2dd(10.0,-10.0));
    samples.push_back(line);

    Vector2dd center(1.0, 0.0);

    AnglePointsFunction *function = NULL; //new AnglePointsFunction(samples, center, 4, 1.0);
    vector<double> in(function->inputs, 0);

    Matrix classicJacobian = function->FunctionArgs::getJacobian(&(in[0]));
    cout << "Classic Jacobian" << endl;
    cout << classicJacobian;

    Matrix manualJacobian = function->getJacobian(&(in[0]));
    cout << "Manual Jacobian" << endl;
    cout << manualJacobian;

    delete function;

}

int main (int /*argC*/, char **/*argV*/)
{
//    cout << "testRadialModel" << endl;
//    testRadialModel();
    cout << "testRadialJacobian" << endl;
    testRadialJacobian();
    cout << "PASSED" << endl;
    return 0;
}
