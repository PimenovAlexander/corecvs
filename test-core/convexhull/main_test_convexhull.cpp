/**
 * \file main_test_convexhull.cpp
 * \brief This is the main file for the test convexhull 
 *
 * \date дек 23, 2017
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <random>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/convexhull.h"
#include "core/geometry/mesh3d.h"



using namespace std;
using namespace corecvs;

/*
./bin/test_core --help
./bin/test_core --gtest_list_tests
./bin/test_core --gtest_filter="convexhull."

*/

TEST(convexhull, testconvexhull)
{
    vector<MyVector> vectors;

    std::mt19937 mt;
    std::uniform_real_distribution<double> dis(-1000, 1000);

    for (int i = 0; i < 10000; i ++)
    {
        vectors.push_back(MyVector( dis(mt), dis(mt), dis(mt)));
    }


    ConvexHullCalc hull(vectors);
    hull.calc();

    Mesh3D mesh;
    mesh.switchColor(true);
    ConvexHullResult *hullResult = hull.getHull();
    if (hullResult->is3D())
    {
        ConvexHull3D *hull3D = static_cast<ConvexHull3D *>(hullResult);
        for (const Face &face :hull3D->faces)
        {
            static int i = 0;
            i++;
            mesh.setColor(RGBColor::rainbow1((double)i / hull3D->faces.size() ));
            mesh.addTriangle(face);
        }
    } else {
        SYNC_PRINT(("Hull is planar"));
    }

    mesh.dumpPLY("hull.ply");


    cout << "Starting test <convexhull>" << endl;
    cout << "Test <convexhull> PASSED" << endl;
}


TEST(convexhull, testconvexhull)
{
    Mesh3D mesh;

    mesh.addIcoSphere(Vector3dd::Zero(), 10, 0);


}
