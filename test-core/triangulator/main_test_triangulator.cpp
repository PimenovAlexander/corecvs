/**
 * \file main_test_triangulator.cpp
 * \brief This is the main file for the test triangulator 
 *
 * \date Dec 13, 2012
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/rectification/essentialMatrix.h"
#include "core/math/vector/vector3d.h"
#include "core/rectification/triangulator.h"
#include "core/cammodel/cameraParameters.h"


using namespace std;
using corecvs::EssentialDecomposition;

TEST(Triangulator, testDepthAndDisparity)
{
    EssentialDecomposition decomposition(Matrix33(1.0), Vector3dd(-1.0,0.0,0.0));

    Vector2dd a( 0.0, 0.0);
    Vector2dd b( 1.0, 0.0);

    Vector3dd a3(a, 1.0);
    Vector3dd b3(b, 1.0);

    RectificationResult rectification;
    rectification.F              = EssentialMatrix();
    rectification.leftCamera     = CameraIntrinsicsLegacy(Vector2dd(1.0, 1.0), Vector2dd(0.0, 0.0), 1.0, 1.0);
    rectification.rightCamera    = CameraIntrinsicsLegacy(Vector2dd(1.0, 1.0), Vector2dd(0.0, 0.0), 1.0, 1.0);
    rectification.baseline       = 1.0;
    rectification.leftTransform  = Matrix33(1.0);
    rectification.rightTransform = Matrix33(1.0);

    Triangulator triangulator(rectification);

    double depth = triangulator.getDepth(a, a - b);
    cout << "Depth :" << depth << endl;

    double disparity = triangulator.getDisparity(depth * a3);
    cout << "Disparity : " << disparity << endl;


    //  double d1l, d2l, errl;

/*    decomposition.getScaler(a, b, d1l, d2l, errl);
    printf("%lf %lf %lf\n", d1l, d2l, errl);

    double d1n, d2n, errn;
    decomposition.getDistance(a, b, d1n, d2n, errn);
    printf("%lf %lf %lf\n", d1n, d2n, errn);
*/
}
