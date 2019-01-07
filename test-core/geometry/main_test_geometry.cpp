/**
 * \file main_test_geometry.cpp
 * \brief This is the main file for the test geometry
 *
 * \date Apr 10, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/polygons.h"
#include "core/math/vector/vector2d.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/fileformats/bmpLoader.h"
#include "core/geometry/convexPolyhedron.h"
#include "core/geometry/mesh3d.h"

#include "core/geometry/kdtree.h"

using namespace corecvs;

using corecvs::Polygon;
using corecvs::Mesh3D;
using corecvs::AxisAlignedBox3d;

TEST(Geometry, KDTree)
{
    std::vector<corecvs::Vector3dd > vec;
    std::vector<corecvs::Vector3dd*> vecP;

    int N = 65536, M = 32;
    std::uniform_real_distribution<double> runif(-1.0, 1.0);
    std::mt19937 rng;
    for (int i = 0; i < N; ++i)
        vec.emplace_back(runif(rng), runif(rng), runif(rng));

    for (auto& v: vec)
        vecP.push_back(&v);

    auto start = std::chrono::high_resolution_clock::now();
    corecvs::KDTree<corecvs::Vector3dd, 3> kdtree(vecP, 6);
    auto stop  = std::chrono::high_resolution_clock::now();
    std::cout << (stop - start).count() / 1e9 << "s for building K-d tree" << std::endl;

    for (int i = 0; i < M; ++i)
    {
        corecvs::Vector3dd query(runif(rng), runif(rng), runif(rng));
        auto res = kdtree.nearestNeighbour(query);
        ASSERT_NE(res, nullptr);

        double minDiff = std::numeric_limits<double>::max();
        for (auto &vv: vec)
            if (minDiff > !(vv - query))
                minDiff = !(vv - query);
        double gotDiff = !(*res - query);
        ASSERT_LE(std::abs(1.0 - minDiff / gotDiff), 1e-9);
    }
}
TEST(Geometry, KDTreePredicate)
{
    std::vector<corecvs::Vector3dd > vec;
    std::vector<corecvs::Vector3dd*> vecP;
    std::vector<int> used;

    int N = 65536, M = 32;
    std::uniform_real_distribution<double> runif(-1.0, 1.0);
    std::mt19937 rng;
    for (int i = 0; i < N; ++i)
    {
        vec.emplace_back(runif(rng), runif(rng), runif(rng));
        used.push_back(runif(rng) < 0.0 ? 0 : 1);
    }

    for (auto& v: vec)
        vecP.push_back(&v);

    auto start = std::chrono::high_resolution_clock::now();
    corecvs::KDTree<corecvs::Vector3dd, 3> kdtree(vecP);
    auto stop  = std::chrono::high_resolution_clock::now();
    std::cout << (stop - start).count() / 1e9 << "s for building K-d tree" << std::endl;

    for (int i = 0; i < M; ++i)
    {
        corecvs::Vector3dd query(runif(rng), runif(rng), runif(rng));
        auto res = kdtree.nearestNeighbour(query, [&](Vector3dd *v) { return used[v - &vec[0]] == 0; });
        ASSERT_NE(res, nullptr);

        double minDiff = std::numeric_limits<double>::max();
        for (auto &vv: vec)
            if (minDiff > !(vv - query) && !used[&vv - &vec[0]])
                minDiff = !(vv - query);
        double gotDiff = !(*res - query);
        ASSERT_LE(std::abs(1.0 - minDiff / gotDiff), 1e-9);
    }
}

TEST(Geometry, testPolygonInside)
{
    corecvs::Polygon p;
    p.push_back(Vector2dd(0.0,0.0));
    p.push_back(Vector2dd(0.0,1.0));
    p.push_back(Vector2dd(1.0,0.0));

    const unsigned TEST_POINTS = 5;

    Vector2dd tests[TEST_POINTS] = {
            Vector2dd( -0.1,  0.5),
            Vector2dd(  1.0,  1.0),
            Vector2dd( 0.25,  0.25),
            Vector2dd( 0.51,  0.5),
            Vector2dd( 0.49,  0.5)
    };

    bool results[TEST_POINTS] = {false, false, true, false, true};

    for (unsigned i = 0; i < TEST_POINTS; i++)
    {
        cout << tests[i] << " should be " << results[i] << " is " << p.isInside(tests[i]) << endl;
    }
}

//TEST(Geometry, testIntersection)
//{

//    RGB24Buffer image(1000,1000, RGBColor::White());

//    corecvs::Polygon p;
//    for (int i = 0; i < 5; i++)
//    {
//        p.push_back(Vector2dd::FromPolar(-2 * M_PI / 5 * i, 260) + Vector2dd(image.w / 2.0, image.h / 2.0));
//    }


//    AbstractPainter<RGB24Buffer> painter(&image);

//    painter.drawPolygon(p, RGBColor::Black());
//    for (unsigned i = 0; i < p.size(); i++)
//    {
//        Vector2dd p1 = p[i];
//        Vector2dd p2 = p[(i + 1) % p.size()];

//        Vector2dd center = (p1 + p2) / 2.0;
//        Vector2dd decal = (p2 - p1).rightNormal().normalised();
//        decal = center + decal * 8;
//        image.drawLine(center.x(), center.y(), decal.x(), decal.y(), RGBColor::gray(128));
//    }


//    for (int i = 1; i < 10; i++)
//    {
//        Ray2d ray( Vector2dd::FromPolar( M_PI / 20.0 * i + 0.02, 100), Vector2dd(140,180));

//        Vector2dd p1 = ray.getPoint(0.0);
//        Vector2dd p2 = ray.getPoint(8.0);
//        image.drawLine(fround(p1.x()), fround(p1.y()), fround(p2.x()), fround(p2.y()), RGBColor::Yellow());
//        for (int j = 0; j < 8; j++)
//        {
//            Vector2dd p = ray.getPoint(j);
//            image.drawCrosshare1(p.x(), p.y(), RGBColor::Cyan());
//        }

//        double t1, t2;
//        ray.clip<Polygon>(p, t1, t2);

//        p1 = ray.getPoint(t1);
//        p2 = ray.getPoint(t2);

//        cout << "t1 = " << t1 << endl;
//        cout << "t2 = " << t2 << endl;
//        cout << "p1 = " << p1 << endl;
//        cout << "p2 = " << p2 << endl;

//        image.drawLine(fround(p1.x()), fround(p1.y()), fround(p2.x()), fround(p2.y()),
//                       t1 > t2 ? RGBColor::Red() : RGBColor::Green());

//    }


//    BMPLoader().save("out1.bmp", &image);

//}

TEST(Geometry, testIntersection3D)
{
    Mesh3D mesh;
    AxisAlignedBox3d box(Vector3dd(-100, -100, -100), Vector3dd(100, 100, 100));
    ConvexPolyhedron poly(box);

    mesh.switchColor();
    mesh.currentColor = RGBColor::Blue();
    mesh.addAOB(box, false);
    Ray3d ray(Vector3dd(-120, -90, -80), Vector3dd(2.0,1.1,1.5));

    mesh.currentColor = RGBColor::White();
    mesh.addLine(ray.getPoint(0), ray.getPoint(5.0));


    double t1, t2;
    bool result = ray.clip<ConvexPolyhedron> (poly, t1, t2);

    mesh.currentColor = result ? RGBColor::Green() : RGBColor::Red();
    mesh.addLine(ray.getPoint(t1), ray.getPoint(t2));

    std::ofstream file("test.ply", std::ios::out);
    mesh.dumpPLY(file);
    file.close();
}


TEST(Geometry, testIntersection3DFast)
{
    Mesh3D mesh;
    AxisAlignedBox3d box(Vector3dd(-100, -100, -100), Vector3dd(100, 100, 100));

    mesh.switchColor();
    mesh.currentColor = RGBColor::Blue();
    mesh.addAOB(box, false);
    Ray3d ray(Vector3dd(2.0,1.1,1.5), Vector3dd(-120, -90, -80));

    mesh.currentColor = RGBColor::White();
    mesh.addLine(ray.getPoint(0), ray.getPoint(2.0));


    double t1, t2;
    bool result = box.intersectWith(ray, t1, t2);

    mesh.currentColor = result ? RGBColor::Green() : RGBColor::Red();
    mesh.addLine(ray.getPoint(t1), ray.getPoint(t2));

    mesh.setColor(RGBColor::Green());
    mesh.addIcoSphere(ray.getPoint(t1), 0.1);

    mesh.setColor(RGBColor::Red());
    mesh.addIcoSphere(ray.getPoint(t2), 0.1);

    SYNC_PRINT((" Intersect %s at (%lf %lf)", result ? "yes" : "no", t1, t2 ));
    cout << "In " <<  ray.getPoint(t1) << endl;
    cout << "Out" <<  ray.getPoint(t2) << endl;

    /* Ok */
    Ray3d rays[] = {
        Ray3d(Vector3dd(1.0, 0.01, 0.01), Vector3dd(-150,    0,    0)),
        Ray3d(Vector3dd(0.01, 1.0, 0.01), Vector3dd(   0, -150,    0)),
        Ray3d(Vector3dd(0.01, 0.01, 1.0), Vector3dd(   0,    0, -150)),


        Ray3d(Vector3dd(-1.0, 0.01, 0.0), Vector3dd(150,   0,   0)),
        Ray3d(Vector3dd(0.01, -1.0, 0.0), Vector3dd(  0, 150,   0)),
        Ray3d(Vector3dd(0.0, 0.01, -1.0), Vector3dd(  0,   0, 150)),

        Ray3d(Vector3dd(1.0, 0.01, 0.0), Vector3dd(-150,    0,    0)),
        Ray3d(Vector3dd(0.01, 1.0, 0.0), Vector3dd(   0, -150,    0)),
        Ray3d(Vector3dd(0.0, 0.01, 1.0), Vector3dd(   0,    0, -150)),


        Ray3d(Vector3dd(1.0, 0.0, 0.0), Vector3dd(-150,    0,    0)),
        Ray3d(Vector3dd(0.0, 1.0, 0.0), Vector3dd(   0, -150,    0)),
        Ray3d(Vector3dd(0.0, 0.0, 1.0), Vector3dd(   0,    0, -150)),
    };

    for (size_t r = 0; r < CORE_COUNT_OF(rays); r++)
    {
        bool result = box.intersectWith(rays[r], t1, t2);
        cout << "Testing:" << r << " - " << (result ? "true" : "false") << "(" << t1 << "," << t2 << ")" << endl;

        mesh.currentColor = RGBColor::White();
        mesh.addLine(rays[r].getPoint(0), rays[r].getPoint(7.0));
        mesh.currentColor = result ? RGBColor::Green() : RGBColor::Red();
        mesh.addLine(rays[r].getPoint(t1), rays[r].getPoint(t2));

        ASSERT_TRUE(result);
        ASSERT_TRUE(t1 == 50);
        ASSERT_TRUE(t2 == 250);
    }

    std::ofstream file("testf.ply", std::ios::out);
    mesh.dumpPLY(file);
    file.close();

}

TEST(Geometry, testIntersection)
{
    Ray3d r1 = Ray3d::FromOriginAndDirection( Vector3dd(0.0, 0.0, 100.0), Vector3dd(1.0, -0.375, 0.0));
    Ray3d r2 = Ray3d::FromOriginAndDirection( Vector3dd(0.0, 0.0, 100.0), Vector3dd(1.0, -0.375, 1.7938e-16));

    ConvexPolyhedron cp;

    cp.faces.push_back(Plane3d(    0,  1, 0.375, -0));
    cp.faces.push_back(Plane3d(-0.75,  0, 0.375, -0));
    cp.faces.push_back(Plane3d(    0, -1, 0.375, -0));
    cp.faces.push_back(Plane3d(0.75,   0, 0.375, -0));
    cp.faces.push_back(Plane3d(  -0,  -0,    -1,  1));

    {
        double t1,t2;
        bool b1 = cp.intersectWith(r1, t1, t2);
        cout << b1 << " " << t1 << " " << t2 << endl;
    }

    {
        double t1,t2;
        bool b2 = cp.intersectWith(r2, t1, t2);
        cout << b2 << " " << t1 << " " << t2 << endl;
    }


}



TEST(Geometry, rayBasics)
{
    Ray3d ray(Vector3dd::OrtX(), Vector3dd::Zero());
    Vector3dd p(Vector3dd(1.0,1.0,1.0));

    Vector3dd pr = ray.projectOnRay(p);

    cout << "Ray      :" << ray << endl;
    cout << "Point    :" << p  << endl;
    cout << "Projected:" << pr << endl;

    ASSERT_TRUE(pr.notTooFar(Vector3dd::OrtX(), 1e-7));

}

TEST(Geometry, intersectionPolyhedronP0)
{    
    Mesh3D input;
    Mesh3D result;

    AxisAlignedBox3d box1(Vector3dd(-100, -100, -100), Vector3dd(10, 10, 10));
    ConvexPolyhedron poly1(box1.getPoints());
    input.switchColor();
    input.currentColor = RGBColor::Green();
    poly1.addToMesh(input);
    cout<< "\npoly1" << endl;
    for(Plane3d &plane: poly1.faces)
    {
        cout << plane << endl;
    }

    AxisAlignedBox3d box2(Vector3dd(-10, -10, -10), Vector3dd(100, 100, 100));
    ConvexPolyhedron poly2(box2.getPoints());
    cout<< "\npoly2" << endl;
    for(Plane3d &plane: poly2.faces)
    {
        cout << plane << endl;
    }
    input.switchColor();
    input.currentColor = RGBColor::Blue();
    poly2.addToMesh(input);
    input.dumpPLY("input.ply");

    ConvexPolyhedron pRes = ConvexPolyhedron::intersect(poly1,poly2);
    cout<< "result planes:" << pRes.faces.size()<<" points: "<<pRes.vertices.size()<<endl;
    result.switchColor();
    result.currentColor = RGBColor::Red();
    pRes.addToMesh(result);
    result.dumpPLY("result.ply");
}


TEST(Geometry, intersectionPolyhedronP1)
{
    Mesh3D inputs[6];

    AxisAlignedBox3d box1(Vector3dd(-20, -20, -20), Vector3dd(10, 10, 10));
    AxisAlignedBox3d box2(Vector3dd(-10, -10, -10), Vector3dd(20, 20, 20));

    {
        inputs[0].addAOB(box1, true);
        inputs[1].addAOB(box2, true);
        inputs[2].addIcoSphere(Vector3dd::Zero(), 20, 0);
        inputs[3].addSphere   (Vector3dd::Zero(), 14);
        inputs[4].addCylinder (Vector3dd::Zero(), 8, 60);
        inputs[5].addDodecahedron(Vector3dd::Zero(), 14);
    }

    for (int i = 0; i < CORE_COUNT_OF(inputs); i++)
    {
        Mesh3D input;
        std::ostringstream out;
        out << "in_" << i << ".ply";
        input.switchColor();
        input.currentColor = RGBColor::getPalleteColor(i);
        input.add(inputs[i]);
        input.dumpPLY(out.str());

        for (int j = i+1; j < CORE_COUNT_OF(inputs); j++)
        {
            Mesh3D debug;

            Mesh3D &in0 = inputs[i];
            Mesh3D &in1 = inputs[j];

            debug.switchColor();
            debug.currentColor = RGBColor::Green();
            //debug.add(in0);
            debug.currentColor = RGBColor::Yellow();
            //debug.add(in1);

            ConvexPolyhedron poly0(in0.vertexes);
            ConvexPolyhedron poly1(in1.vertexes);


            ConvexPolyhedron intersection = ConvexPolyhedron::intersect(poly0,poly1);

            debug.currentColor = RGBColor::Blue();
            intersection.addToMesh(debug);

            std::ostringstream out;
            out << "int_" << i << "_" << j << ".ply";
            debug.dumpPLY(out.str());
        }
    }
}



//int main (int /*argC*/, char ** /*argV*/)
//{
//    testIntersection3D();
//    testIntersection();
//    testPolygonInside();
//}
