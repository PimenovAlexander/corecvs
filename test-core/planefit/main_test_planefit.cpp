/**
 * \file main_test_planefit.cpp
 * \brief This is the main file for the test planefit 
 *
 * \date апр 08, 2017
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <vector>
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/plane3dFit.h"
#include "core/geometry/polygons.h"

using namespace std;
using namespace corecvs;

TEST(planefit, testFitModel)
{
    vector<Vector3dd  > input;
    vector<Vector3dd *> inputPtr;

    Matrix44 trasform = Matrix44::Shift(5,3,1) * Matrix44::RotationY(degToRad(30));

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            Vector3dd pl(i , j, 0.0);

            input.push_back(trasform * pl);

        }
    }

    for (Vector3dd &v : input)
    {
         inputPtr.push_back(&v);
    }

    Plane3dFit::PlaneReconstructionModel model = Plane3dFit::PlaneReconstructionModel(inputPtr);
    cout << "Result: " << model.plane << endl;

    for (Vector3dd &v : input)
    {
         ASSERT_TRUE(model.fits(v, 0.0001));
    }


    cout << "PASSED: testFitModel" << endl;
}

TEST(planefit, testPlaneFrame)
{
   PlaneFrame frame;
   frame.p1 = Vector3dd::Zero();
   frame.e1 = Vector3dd::OrtX();
   frame.e2 = Vector3dd::OrtY();

   Vector3dd v = Vector3dd(4,5,7);

   ASSERT_TRUE(frame.projectTo(v).notTooFar(Vector2dd(4,5)));
}
