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
#include "core/geometry/convexHull.h"
#include "core/geometry/convexQuickHull.h"

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
    vector<Vector3dd> vectors;

    std::mt19937 mt;
    std::uniform_real_distribution<double> dis(-1000, 1000);

    for (int i = 0; i < 10000; i ++)
    {
        vectors.push_back(MyVector( dis(mt), dis(mt), dis(mt)));
    }

    {
        ConvexHullCalc hull(vectors);

        PreciseTimer timer = PreciseTimer::currentTime();
        hull.calc();
        uint64_t delay = timer.usecsToNow();
        printf("Convex Hull :%8" PRIu64 "us \n", delay);


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
    }

    {
         PreciseTimer timer = PreciseTimer::currentTime();
         ConvexQuickHull::HullFaces result = ConvexQuickHull::quickHull(vectors, 1e-9);
         uint64_t delay = timer.usecsToNow();
         printf("Quick Hull :%8" PRIu64 "us \n", delay);

         Mesh3D mesh;
         mesh.switchColor(true);

         for (size_t i = 0; i < result.size(); i++)
         {
             mesh.setColor(RGBColor::rainbow1((double)i / result.size() ));
             mesh.addTriangle(result[i].plane);
         }

         mesh.dumpPLY("qhull.ply");
    }

    cout << "Test <convexhull> PASSED" << endl;
}

#if 0
TEST(convexhull, testconvexhull1)
{
    Mesh3D mesh;

    mesh.addIcoSphere(Vector3dd::Zero(), 10, 0);


}
#endif

bool isEqual(const Triangle3dd &t1, const Triangle3dd &t2)
{
    for (int i = 0; i < Triangle3dd::SIZE; i++)
    {
        int j = 0;
        for (; j < Triangle3dd::SIZE; j++)
        {
            if (t1.p[i].notTooFar(t2.p[j], 1e-7))
            {
                break;

            }
        }
        if (j == Triangle3dd::SIZE)
            return false;
    }

    return true;
}


void testHull(const ConvexQuickHull::vertices &verts, const ConvexQuickHull::HullFaces &groundTruth) {
    double eps = 0.00001;
    ConvexQuickHull::HullFaces faces = ConvexQuickHull::quickHull(verts, eps);
    bool test = true;
    if (faces.size() != groundTruth.size())
        test = false;
    else {
        for (int i = 0; i < groundTruth.size(); i++)
            if (!isEqual(groundTruth[i].plane, faces[i].plane))
                test = false;
    }
    int i = 0;
    for (auto face : faces) {
        printf("(%F, %F, %F)\n", face.plane.p1().x(), face.plane.p1().y(), face.plane.p1().z());
        printf("(%F, %F, %F)\n", face.plane.p2().x(), face.plane.p2().y(), face.plane.p2().z());
        printf("(%F, %F, %F)\n", face.plane.p3().x(), face.plane.p3().y(), face.plane.p3().z());
        printf("\n");
        ++i;
    }
    if (test)
        printf("test completed\n");
    else
        printf("test failed\n");
    CORE_ASSERT_TRUE(test, "Ground truth not matched");

    printf("number of facets: %i\n", i);
}


TEST(convexhull, testquickhull)
{
    printf("First test: one point\n");
    ConvexQuickHull::vertices verts = {{1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}};
    ConvexQuickHull::HullFaces groundTruth = {};

    testHull(verts, groundTruth);
    printf("\nSecond test: line\n");

    verts = {{1,0,0}, {1,0,0}, {2,0,0}, {3,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}};
    groundTruth = {};
    testHull(verts, groundTruth);
    printf("\nThird test: plane\n");
    verts = {{1,0,0}, {1,0,0}, {2,0,0}, {0,3,0}, {0,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}};
    groundTruth = {};
    testHull(verts, groundTruth);
    printf("\nForth test: cube with (0,0,0) point inside\n");
    verts = {{0, 0, 0}, {-1, -1, 1}, {1, -1, 1}, {-1, -1, -1}, {1, -1, -1}, {-1, 1, 1}, {1, 1, 1}, {-1, 1, -1}, {1, 1, -1}};
    groundTruth = {
        {{ {1, -1, -1}, {1, 1, -1}, {1,  1,  1}}},
        {{ {-1, 1, -1}, {1, 1,  1}, {1,  1, -1}}},
        {{ {-1, 1, -1}, {1, 1, -1}, {1, -1, -1}}},
        {{ {-1, -1, 1}, {-1, -1, -1}, {1, -1, -1}}},
        {{ {-1, 1, -1}, {1, -1, -1}, {-1, -1, -1}}},
        {{ {-1, 1, -1}, {-1, -1, -1}, {-1, -1, 1}}},
        {{ {1, -1, -1}, {1, -1, 1}, {-1, -1, 1}}},
        {{ {1, 1, 1}, {-1, -1, 1}, {1, -1, 1}}},
        {{ {1, 1, 1}, {1, -1, 1}, {1, -1, -1}}},
        {{ {1, 1, 1}, {-1, 1, 1}, {-1, -1, 1}}},
        {{ {-1, 1, -1}, {-1, -1, 1}, {-1, 1, 1}}},
        {{ {-1, 1, -1}, {-1, 1, 1}, {1, 1, 1}}}
    };

    testHull(verts, groundTruth);
    printf("\nFifth test: dodecahedron\n");
    verts = {{0.469, 0.469, 0.469}, {0.290, 0.000, 0.759}, {-0.759, -0.290, 0.000}, {0.759, 0.290, 0.000}, {-0.469, 0.469, -0.469}, {0.000, -0.759, -0.290}, {-0.759, 0.290, 0.000}, {0.469, -0.469, 0.469}, {-0.469, 0.469, 0.469}, {-0.469, -0.469, 0.469}, {0.469, -0.469, -0.469}, {0.290, 0.000, -0.759}, {-0.469, -0.469, -0.469}, {0.000, -0.759, 0.290}, {0.000, 0.759, -0.290}, {-0.290, 0.000, 0.759}, {0.759, -0.290, 0.000}, {-0.290, 0.000, -0.759}, {0.469, 0.469, -0.469}, {0.000, 0.759, 0.290}};
    groundTruth = {
        {{     {0.000000, 0.759000, 0.290000}, {-0.290000, 0.000000, 0.759000}, {0.290000, 0.000000, 0.759000}}},
        {{    {0.000000, 0.759000, 0.290000}, {-0.469000, 0.469000, 0.469000}, {-0.290000, 0.000000, 0.759000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {-0.290000, 0.000000, 0.759000}, {-0.469000, 0.469000, 0.469000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {-0.469000, 0.469000, 0.469000}, {0.000000, 0.759000, 0.290000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.290000, 0.000000, -0.759000}, {0.759000, 0.290000, 0.000000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.759000, 0.290000, 0.000000}, {0.290000, 0.000000, 0.759000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.000000, -0.759000, 0.290000}, {0.000000, -0.759000, -0.290000}}},
        {{    {-0.290000, 0.000000, 0.759000}, {0.000000, -0.759000, 0.290000}, {0.290000, 0.000000, 0.759000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {-0.759000, -0.290000, 0.000000}, {-0.290000, 0.000000, 0.759000}}},
        {{    {0.000000, -0.759000, -0.290000}, {0.000000, -0.759000, 0.290000}, {-0.759000, -0.290000, 0.000000}}},
        {{    {0.000000, -0.759000, -0.290000}, {-0.759000, -0.290000, 0.000000}, {-0.469000, -0.469000, -0.469000}}},
        {{    {0.290000, 0.000000, -0.759000}, {0.469000, -0.469000, -0.469000}, {0.000000, -0.759000, -0.290000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.000000, -0.759000, -0.290000}, {0.469000, -0.469000, -0.469000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.469000, -0.469000, -0.469000}, {0.290000, 0.000000, -0.759000}}},
        {{    {0.759000, 0.290000, 0.000000}, {0.469000, 0.469000, 0.469000}, {0.290000, 0.000000, 0.759000}}},
        {{    {0.000000, 0.759000, 0.290000}, {0.290000, 0.000000, 0.759000}, {0.469000, 0.469000, 0.469000}}},
        {{    {0.000000, 0.759000, 0.290000}, {0.469000, 0.469000, 0.469000}, {0.759000, 0.290000, 0.000000}}},
        {{    {0.000000, -0.759000, 0.290000}, {-0.469000, -0.469000, 0.469000}, {-0.759000, -0.290000, 0.000000}}},
        {{    {-0.290000, 0.000000, 0.759000}, {-0.759000, -0.290000, 0.000000}, {-0.469000, -0.469000, 0.469000}}},
        {{    {-0.290000, 0.000000, 0.759000}, {-0.469000, -0.469000, 0.469000}, {0.000000, -0.759000, 0.290000}}},
        {{    {0.000000, 0.759000, 0.290000}, {0.759000, 0.290000, 0.000000}, {0.000000, 0.759000, -0.290000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {0.000000, 0.759000, 0.290000}, {0.000000, 0.759000, -0.290000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {0.000000, 0.759000, -0.290000}, {-0.469000, 0.469000, -0.469000}}},
        {{    {0.000000, -0.759000, -0.290000}, {-0.469000, -0.469000, -0.469000}, {-0.290000, 0.000000, -0.759000}}},
        {{    {0.000000, -0.759000, -0.290000}, {-0.290000, 0.000000, -0.759000}, {0.290000, 0.000000, -0.759000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {-0.469000, 0.469000, -0.469000}, {-0.290000, 0.000000, -0.759000}}},
        {{    {-0.759000, -0.290000, 0.000000}, {-0.290000, 0.000000, -0.759000}, {-0.469000, -0.469000, -0.469000}}},
        {{    {-0.759000, 0.290000, 0.000000}, {-0.290000, 0.000000, -0.759000}, {-0.759000, -0.290000, 0.000000}}},
        {{    {0.000000, 0.759000, -0.290000}, {-0.290000, 0.000000, -0.759000}, {-0.469000, 0.469000, -0.469000}}},
        {{    {0.290000, 0.000000, -0.759000}, {-0.290000, 0.000000, -0.759000}, {0.000000, 0.759000, -0.290000}}},
        {{    {0.000000, -0.759000, 0.290000}, {0.469000, -0.469000, 0.469000}, {0.290000, 0.000000, 0.759000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.290000, 0.000000, 0.759000}, {0.469000, -0.469000, 0.469000}}},
        {{    {0.759000, -0.290000, 0.000000}, {0.469000, -0.469000, 0.469000}, {0.000000, -0.759000, 0.290000}}},
        {{    {0.759000, 0.290000, 0.000000}, {0.469000, 0.469000, -0.469000}, {0.000000, 0.759000, -0.290000}}},
        {{    {0.290000, 0.000000, -0.759000}, {0.000000, 0.759000, -0.290000}, {0.469000, 0.469000, -0.469000}}},
        {{    {0.290000, 0.000000, -0.759000}, {0.469000, 0.469000, -0.469000}, {0.759000, 0.290000, 0.000000}}}};

    testHull(verts, groundTruth);

}

TEST(convexhull, testquickfazzer)
{
    Mesh3D inputMesh;
    inputMesh.addIcoSphere(Vector3dd::Zero(), 100, 0);

    mt19937 mt;
    vector<Vector3dd> input = inputMesh.vertexes;
    for (int i=0; i < 1000; i++)
    {
        /* Add some point */
        std::uniform_int_distribution<int> d(0, input.size() - 1);
        Vector3dd a = input[d(mt)];
        Vector3dd b = input[d(mt)];
        Vector3dd c = input[d(mt)];

        input.push_back((a+b+c) / 3.0);

        Vector3dd add = input[d(mt)];
        input.insert(input.begin() + d(mt), add);

        SYNC_PRINT(("Testing size: %d\n", (int)input.size()));
        ConvexQuickHull::HullFaces faces = ConvexQuickHull::quickHull(input, 1e-9);
        SYNC_PRINT(("Faces: %d\n", (int)faces.size()));

        ASSERT_TRUE((faces.size() == 20));

    }
}

TEST(convexhull, teststrange)
{
    vector<Vector3dd> input;
    input.push_back(Vector3dd::Zero());
    input.push_back(Vector3dd::OrtX());
    input.push_back(Vector3dd::OrtY());
    input.push_back(Vector3dd::OrtZ());

    input.push_back(Vector3dd::OrtZ() * 2);

    SYNC_PRINT(("Input points\n"));
    for (size_t t = 0; t < input.size(); t++ )
    {
        cout << t << " " << input[t] << endl;
    }
    ConvexQuickHull::HullFaces faces = ConvexQuickHull::quickHull(input, 1e-9);
    SYNC_PRINT(("Faces: %d\n", faces.size()));
}
