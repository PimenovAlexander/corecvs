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
#include "core/geometry/projectiveConvexQuickHull.h"

#include "core/geometry/mesh3d.h"



using namespace std;
using namespace corecvs;

/*
./bin/test_core --help
./bin/test_core --gtest_list_tests
./bin/test_core --gtest_filter="convexhull."

*/

TEST(ConvexHull, testConvexHull)
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


ConvexQuickHull::HullFaces testHull(const ConvexQuickHull::Vertices &verts, const ConvexQuickHull::HullFaces &groundTruth) {
    double eps = 0.00001;
    ConvexQuickHull::HullFaces faces = ConvexQuickHull::quickHull(verts, eps);

    //faces.reorientNormals();

    bool test = true;
    if (faces.size() != groundTruth.size())
        test = false;
    else {
        for (size_t i = 0; i < groundTruth.size(); i++)
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
    return faces;
}


TEST(ConvexHull, testQuickHull)
{
    printf("First test: one point\n");
    {
        ConvexQuickHull::Vertices verts = {{1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}};
        ConvexQuickHull::HullFaces groundTruth = {};       
    }
    printf("\nSecond test: line\n");
    {
        ConvexQuickHull::Vertices verts = {{1,0,0}, {1,0,0}, {2,0,0}, {3,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}};
        ConvexQuickHull::HullFaces groundTruth = {};
        testHull(verts, groundTruth);
    }
    printf("\nThird test: plane\n");
    {
        ConvexQuickHull::Vertices verts = {{1,0,0}, {1,0,0}, {2,0,0}, {0,3,0}, {0,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}};
        ConvexQuickHull::HullFaces groundTruth = {};
        testHull(verts, groundTruth);
    }
    printf("\nForth test: cube with (0,0,0) point inside\n");
    {
        ConvexQuickHull::Vertices verts = {{0, 0, 0}, {-1, -1, 1}, {1, -1, 1}, {-1, -1, -1}, {1, -1, -1}, {-1, 1, 1}, {1, 1, 1}, {-1, 1, -1}, {1, 1, -1}};

        ConvexQuickHull::HullFaces groundTruth = {
            {{ {1, -1, -1}, { 1,  1, -1}, { 1,  1,  1}}},
            {{ {-1, 1, -1}, { 1,  1,  1}, { 1,  1, -1}}},
            {{ {-1, 1, -1}, { 1,  1, -1}, { 1, -1, -1}}},
            {{ {-1, -1, 1}, {-1, -1, -1}, { 1, -1, -1}}},
            {{ {-1, 1, -1}, { 1, -1, -1}, {-1, -1, -1}}},
            {{ {-1, 1, -1}, {-1, -1, -1}, {-1, -1,  1}}},
            {{ {1, -1, -1}, { 1, -1,  1}, {-1, -1,  1}}},
            {{ {1,  1,  1}, {-1, -1,  1}, { 1, -1,  1}}},
            {{ {1,  1,  1}, { 1, -1,  1}, { 1, -1, -1}}},
            {{ {1,  1,  1}, {-1,  1,  1}, {-1, -1,  1}}},
            {{ {-1, 1, -1}, {-1, -1,  1}, {-1,  1,  1}}},
            {{ {-1, 1, -1}, {-1,  1,  1}, { 1,  1,  1}}}
        };

        ConvexQuickHull::HullFaces faces = testHull(verts, groundTruth);
        cout << "Cube volume:" << faces.getVolume() << " expected " << (2 * 2* 2) << endl;
        CORE_ASSERT_DOUBLE_EQUAL_E(faces.getVolume(), (2 * 2* 2), 1e-7, "Ground truth not matched");

        vector<Vector3dd> f = faces.getPoints();
        for (Vector3dd &p : f) {
            cout << " - " <<  p  << endl;
        }

        CORE_ASSERT_TRUE_P(f.size() == 8, ("Wrong point number %d vs %d", (int)f.size(), 8));

        CORE_ASSERT_TRUE_P(faces.isInside(Vector3dd::Zero()), ("Inside check failed"));
    }

    printf("\nFifth test: dodecahedron\n");
    {
        ConvexQuickHull::Vertices verts = {{0.469, 0.469, 0.469}, {0.290, 0.000, 0.759}, {-0.759, -0.290, 0.000}, {0.759, 0.290, 0.000}, {-0.469, 0.469, -0.469}, {0.000, -0.759, -0.290}, {-0.759, 0.290, 0.000}, {0.469, -0.469, 0.469}, {-0.469, 0.469, 0.469}, {-0.469, -0.469, 0.469}, {0.469, -0.469, -0.469}, {0.290, 0.000, -0.759}, {-0.469, -0.469, -0.469}, {0.000, -0.759, 0.290}, {0.000, 0.759, -0.290}, {-0.290, 0.000, 0.759}, {0.759, -0.290, 0.000}, {-0.290, 0.000, -0.759}, {0.469, 0.469, -0.469}, {0.000, 0.759, 0.290}};
        ConvexQuickHull::HullFaces groundTruth = {
            {{    { 0.000,  0.759,  0.290}, {-0.290,  0.000,  0.759}, { 0.290,  0.000,  0.759}}},
            {{    { 0.000,  0.759,  0.290}, {-0.469,  0.469,  0.469}, {-0.290,  0.000,  0.759}}},
            {{    {-0.759,  0.290,  0.000}, {-0.290,  0.000,  0.759}, {-0.469,  0.469,  0.469}}},
            {{    {-0.759,  0.290,  0.000}, {-0.469,  0.469,  0.469}, { 0.000,  0.759,  0.290}}},
            {{    { 0.759, -0.290,  0.000}, { 0.290,  0.000, -0.759}, { 0.759,  0.290,  0.000}}},
            {{    { 0.759, -0.290,  0.000}, { 0.759,  0.290,  0.000}, { 0.290,  0.000,  0.759}}},
            {{    { 0.759, -0.290,  0.000}, { 0.000, -0.759,  0.290}, { 0.000, -0.759, -0.290}}},
            {{    {-0.290,  0.000,  0.759}, { 0.000, -0.759,  0.290}, { 0.290,  0.000,  0.759}}},
            {{    {-0.759,  0.290,  0.000}, {-0.759, -0.290,  0.000}, {-0.290,  0.000,  0.759}}},
            {{    { 0.000, -0.759, -0.290}, { 0.000, -0.759,  0.290}, {-0.759, -0.290,  0.000}}},
            {{    { 0.000, -0.759, -0.290}, {-0.759, -0.290,  0.000}, {-0.469, -0.469, -0.469}}},
            {{    { 0.290,  0.000, -0.759}, { 0.469, -0.469, -0.469}, { 0.000, -0.759, -0.290}}},
            {{    { 0.759, -0.290,  0.000}, { 0.000, -0.759, -0.290}, { 0.469, -0.469, -0.469}}},
            {{    { 0.759, -0.290,  0.000}, { 0.469, -0.469, -0.469}, { 0.290,  0.000, -0.759}}},
            {{    { 0.759,  0.290,  0.000}, { 0.469,  0.469,  0.469}, { 0.290,  0.000,  0.759}}},
            {{    { 0.000,  0.759,  0.290}, { 0.290,  0.000,  0.759}, { 0.469,  0.469,  0.469}}},
            {{    { 0.000,  0.759,  0.290}, { 0.469,  0.469,  0.469}, { 0.759,  0.290,  0.000}}},
            {{    { 0.000, -0.759,  0.290}, {-0.469, -0.469,  0.469}, {-0.759, -0.290,  0.000}}},
            {{    {-0.290,  0.000,  0.759}, {-0.759, -0.290,  0.000}, {-0.469, -0.469,  0.469}}},
            {{    {-0.290,  0.000,  0.759}, {-0.469, -0.469,  0.469}, { 0.000, -0.759,  0.290}}},
            {{    { 0.000,  0.759,  0.290}, { 0.759,  0.290,  0.000}, { 0.000,  0.759, -0.290}}},
            {{    {-0.759,  0.290,  0.000}, { 0.000,  0.759,  0.290}, { 0.000,  0.759, -0.290}}},
            {{    {-0.759,  0.290,  0.000}, { 0.000,  0.759, -0.290}, {-0.469,  0.469, -0.469}}},
            {{    { 0.000, -0.759, -0.290}, {-0.469, -0.469, -0.469}, {-0.290,  0.000, -0.759}}},
            {{    { 0.000, -0.759, -0.290}, {-0.290,  0.000, -0.759}, { 0.290,  0.000, -0.759}}},
            {{    {-0.759,  0.290,  0.000}, {-0.469,  0.469, -0.469}, {-0.290,  0.000, -0.759}}},
            {{    {-0.759, -0.290,  0.000}, {-0.290,  0.000, -0.759}, {-0.469, -0.469, -0.469}}},
            {{    {-0.759,  0.290,  0.000}, {-0.290,  0.000, -0.759}, {-0.759, -0.290,  0.000}}},
            {{    { 0.000,  0.759, -0.290}, {-0.290,  0.000, -0.759}, {-0.469,  0.469, -0.469}}},
            {{    { 0.290,  0.000, -0.759}, {-0.290,  0.000, -0.759}, { 0.000,  0.759, -0.290}}},
            {{    { 0.000, -0.759,  0.290}, { 0.469, -0.469,  0.469}, { 0.290,  0.000,  0.759}}},
            {{    { 0.759, -0.290,  0.000}, { 0.290,  0.000,  0.759}, { 0.469, -0.469,  0.469}}},
            {{    { 0.759, -0.290,  0.000}, { 0.469, -0.469,  0.469}, { 0.000, -0.759,  0.290}}},
            {{    { 0.759,  0.290,  0.000}, { 0.469,  0.469, -0.469}, { 0.000,  0.759, -0.290}}},
            {{    { 0.290,  0.000, -0.759}, { 0.000,  0.759, -0.290}, { 0.469,  0.469, -0.469}}},
            {{    { 0.290,  0.000, -0.759}, { 0.469,  0.469, -0.469}, { 0.759,  0.290,  0.000}}}};

        testHull(verts, groundTruth);
    }

}


TEST(ConvexHull, testQuickHullNormals)
{
    Mesh3D inputMesh;
    inputMesh.addIcoSphere(Vector3dd::Zero(), 100, 2);

    ConvexQuickHull::HullFaces faces = ConvexQuickHull::quickHull(inputMesh.vertexes, 1e-9);

    faces.reorientNormals();

    faces.reorientNormals(-1);
    faces.reorientNormals(-1);

    faces.reorientNormals(1);
    faces.reorientNormals(1);

    faces.reorientNormals(0);
}

TEST(convexhull, testQuickFazzer)
{
    Mesh3D inputMesh;
    inputMesh.addIcoSphere(Vector3dd::Zero(), 100, 0);

    mt19937 mt;
    vector<Vector3dd> input = inputMesh.vertexes;
    for (int i=0; i < 1000; i++)
    {
        /* Add some point */
        std::uniform_int_distribution<int> d(0, (int)input.size() - 1);
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

TEST(ConvexHull, testStrange)
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
    SYNC_PRINT(("Faces: %d\n", (int)faces.size()));
}

TEST(ConvexHull, testIntersection)
{
    Mesh3D debug;

    AxisAlignedBox3d b1 = AxisAlignedBox3d::ByCenter(Vector3dd::Zero(), Vector3dd(2,2,2));
    AxisAlignedBox3d b2 = AxisAlignedBox3d::ByCenter(Vector3dd::Zero(), Vector3dd(1,1,3));

    Mesh3D m1;
    m1.addAOB(b1);

    Mesh3D m2;
    m2.addAOB(b2);

    ConvexQuickHull::HullFaces box1(m1);
    ConvexQuickHull::HullFaces box2(m2);

    ConvexQuickHull::HullFaces intersect = ConvexQuickHull::intersect(box1, box2);

    vector<Vector3dd> points = intersect.getPoints();
    for (Vector3dd p : points) {
        cout << " " << p << endl;
    }

    debug.switchColor(true);
    debug.setColor(RGBColor::Green());
    debug.add(m1);
    debug.setColor(RGBColor::Blue());
    debug.add(m2);

    Mesh3D sect;
    intersect.addToMesh(sect);
    debug.setColor(RGBColor::Red());
    debug.add(sect);

    debug.dumpPLY("intersect.ply");
    CORE_ASSERT_TRUE_P((points.size() == 9), ("Wrong number of points %d %d\n", (int)points.size(), 9));
}

TEST(ConvexHull, testBestConvexHull)
{
    vector<ProjectiveCoord4d> input;
    input.push_back(ProjectiveCoord4d(Vector3dd::Zero(), 1));
    input.push_back(ProjectiveCoord4d(Vector3dd::OrtX(), 1));
    input.push_back(ProjectiveCoord4d(Vector3dd::OrtY(), 1));
    input.push_back(ProjectiveCoord4d(Vector3dd::OrtZ(), 1));
    input.push_back(ProjectiveCoord4d(Vector3dd::OrtZ() * 2, 1));

    SYNC_PRINT(("Input points\n"));
    for (size_t t = 0; t < input.size(); t++ )
        cout << t << " " << input[t] << endl;
    ProjectiveConvexQuickHull::HullFaces faces = ProjectiveConvexQuickHull::quickHull(input, 1e-9);
    SYNC_PRINT(("Faces: %d\n", (int)faces.size()));
}
