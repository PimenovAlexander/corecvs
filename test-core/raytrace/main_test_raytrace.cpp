/**
 * \file main_test_raytrace.cpp
 * \brief This is the main file for the test raytrace 
 *
 * \date мая 06, 2016
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <fstream>
#include <iostream>
#include "core/fileformats/objLoader.h"
#include "core/buffers/bufferFactory.h"
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/sdfRenderable.h"
#include "core/geometry/raytrace/materialExamples.h"

#include "core/fileformats/bmpLoader.h"
#include "core/fileformats/meshLoader.h"
#include "core/utils/preciseTimer.h"
#include "core/geometry/raytrace/perlinNoise.h"
#include "core/utils/log.h"

using namespace std;
using namespace corecvs;


TEST(Raytrace, DISABLED_testRaytraceBase)
{
    int h = 1600;
    int w = 1600;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.setProjection(new PinholeCameraIntrinsics(Vector2dd(w, h), degToRad(80.0)));

    /* Materials */
    RaytraceableMaterial blueMirror;
    blueMirror.ambient = RGBColor::Blue().toDouble() * 0.2 ;
    blueMirror.diffuse = RGBColor::White().toDouble() / 255.0;
    blueMirror.reflCoef = 0.0;
    blueMirror.refrCoef = 0;
    blueMirror.specular = RGBColor::Blue().toDouble() / 255.0;

    /* */
    RaytraceableMaterial redMirror;
    redMirror.ambient = RGBColor::Red().toDouble() * 0.2 ;
    redMirror.diffuse = RGBColor::White().toDouble() / 255.0;
    redMirror.reflCoef = 0.0;
    redMirror.refrCoef = 0;
    redMirror.specular = RGBColor(250.0, 220.0, 255.0).toDouble() / 255.0;
    /* */
    RaytraceableChessMaterial chess;

    RaytraceableSphere sphere1(Sphere3d(Vector3dd(0,0, 150.0), 50.0));
    sphere1.name = "Sphere1";
    sphere1.color = RGBColor::Red().toDouble();
    sphere1.material = &redMirror;

    RaytraceableSphere sphere2(Sphere3d(Vector3dd(100, 0, 150.0), 45.0));
    sphere2.name = "Sphere2";
    sphere2.color = RGBColor::Green().toDouble();
    sphere2.material = &blueMirror;

    RaytraceableSphere sphere3(Sphere3d(Vector3dd(0, 2030, 0.0), 2000.0));
    sphere3.name = "Sphere3";
    sphere3.color = RGBColor::Green().toDouble();
    sphere3.material = &chess;


    RaytraceableSphere sphere4(Sphere3d(Vector3dd(-150, -120, 250.0), 120.0));
    sphere4.name = "Sphere4";
    sphere4.color = RGBColor::Green().toDouble();
    sphere4.material = &blueMirror;

    RaytraceablePlane plane1(Plane3d(-Vector3dd::OrtY(), 25.0));
    plane1.name = "Plane1";
    plane1.color = RGBColor::Blue().toDouble();
    plane1.material = &redMirror;

    RaytraceableTriangle triangle1(Triangle3dd(Vector3dd(60, -40, 140), Vector3dd(10, -80, 110), Vector3dd(120, -100, 112)));
    triangle1.name = "Triangle1";
    triangle1.color = RGBColor::Blue().toDouble();
    triangle1.material = &blueMirror;

    RaytraceableTransform transformed(&sphere1, Matrix44::Scale(0.5, 1.0, 1.0));
    transformed.name = "transformed";
    transformed.color = RGBColor::Blue().toDouble();
    transformed.material = &blueMirror;

    Mesh3DDecorated mesh;
    MeshLoader().load(&mesh, "bun_zipper.ply");
    mesh.transform(Matrix44::Shift(0, 40, 80) * Matrix44::Scale(430.0) * Matrix44::RotationX(degToRad(180)));

    RaytraceableMesh rMesh(&mesh);
    rMesh.name = "Bunny1";
    rMesh.color = RGBColor::Blue().toDouble();
    rMesh.material = &blueMirror;

    RaytraceableUnion scene;
    scene.elements.push_back(&plane1);
    //scene.elements.push_back(&sphere1);
    scene.elements.push_back(&transformed);

    scene.elements.push_back(&sphere2);
    scene.elements.push_back(&sphere3);
    scene.elements.push_back(&sphere4);
    scene.elements.push_back(&triangle1);

//    scene.elements.push_back(&rMesh);


    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( 0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));


    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    renderer.ambient = RGBColor(20,20,20).toDouble();

    PreciseTimer timer;
    timer = PreciseTimer::currentTime();
    renderer.trace(buffer);
    SYNC_PRINT(("Total time %" PRIu64 " us", timer.usecsToNow()));


    BMPLoader().save("raytrace.bmp", buffer);
    delete_safe(buffer);
}

TEST(Raytrace, DISABLED_testRaytraceSpeedup)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *bufferS = new RGB24Buffer(h, w, RGBColor::Black());
    RGB24Buffer *bufferF = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.setProjection(new PinholeCameraIntrinsics(Vector2dd(w, h), degToRad(60.0)));

    /* Materials */
    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( 0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));

    Mesh3DDecorated mesh;
    MeshLoader loader;
    loader.load(&mesh, "bunny.ply");
    //loader.load(&mesh, "body-v2.obj");

    //Matrix44::Shift(0, 0 , 3500)
    //Matrix44::Shift(0, 50, 180)

    mesh.transform(Matrix44::Shift(0, 50, 180) * Matrix44::Scale(1.5) * Matrix44::RotationZ(degToRad(-90)) * Matrix44::RotationY(degToRad(-90)) * Matrix44::RotationX(degToRad(180)));
    mesh.dumpInfo();
    mesh.recomputeMeanNormals();

    RaytraceableMesh rMesh(&mesh);
    rMesh.name = "Bunny1";
    rMesh.color = RGBColor::Blue().toDouble();
    rMesh.material = MaterialExamples::ex1();

    RaytraceableUnion scene;
    scene.elements.push_back(&rMesh);

    for (double d = 60; d < 200; d+=30) {
        RaytraceableSphere* sphere1 = new RaytraceableSphere(Sphere3d(Vector3dd(-30, 0, 120.0), 15.00));
        sphere1->name = "Sphere1";
        sphere1->color = RGBColor::Red().toDouble();
        sphere1->material = MaterialExamples::bumpy();
        scene.elements.push_back(sphere1);
    }

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    renderer.ambient = RGBColor(20,20,20).toDouble();

    PreciseTimer timer = PreciseTimer::currentTime();
    //renderer.trace(bufferS);
    SYNC_PRINT(("Slow render time %lf ms\n", timer.usecsToNow() / 1000.0));
    BMPLoader().save("trace-slow.bmp", bufferS);


    RaytraceableOptiMesh roMesh(&mesh);

    roMesh.name = "Bunny1";
    roMesh.color = RGBColor::Blue().toDouble();
    roMesh.material = MaterialExamples::ex1();

    timer = PreciseTimer::currentTime();
    roMesh.optimize();
    SYNC_PRINT(("Mesh optimise time %lf us\n", timer.usecsToNow() / 1000.0));
    SYNC_PRINT(("Mesh tree size is   %d\n", roMesh.opt->childCount()));
    SYNC_PRINT(("Mesh tree triangles %d\n", roMesh.opt->elementCount()));

    //Mesh3D dumpTreeMesh;
    //dumpTreeMesh.switchColor(true);
    //roMesh.dumpToMesh(dumpTreeMesh, true, true);
    //dumpTreeMesh.dumpPLY("subtree.ply");

    RaytraceableUnion scene1;
    scene1.elements.push_back(&roMesh);

    for (double d = 60; d < 280; d+=30) {
        RaytraceableSphere* sphere1 = new RaytraceableSphere(Sphere3d(Vector3dd(-60 + d / 10.0 , 0, d), 15.00));
        sphere1->name = "Sphere1";
        sphere1->color = RGBColor::Red().toDouble();
        sphere1->material = MaterialExamples::bumpy();
        scene1.elements.push_back(sphere1);
    }

    renderer.object = &scene1;
    renderer.supersample = false;
    renderer.sampleNum = 70;

    timer = PreciseTimer::currentTime();
    //renderer.trace(bufferF);
    renderer.traceFOV(bufferF, 8, 140);
    SYNC_PRINT(("Fast render time %lf ms\n", timer.usecsToNow() / 1000.0));
    BMPLoader().save("trace-fast.bmp", bufferF);

    /*if (!bufferF->isEqualTrace(*bufferS))
    {
        SYNC_PRINT(("Erorr"));
    }*/

    delete_safe(bufferS);
    delete_safe(bufferF);
}

class LaserPlaneLight : public RaytraceablePointLight, public Plane3d
{
public:
    LaserPlaneLight(const Plane3d &plane) :
        RaytraceablePointLight(
            RGBColor::Red().toDouble(),
            plane.projectPointTo(Vector3dd::Zero())),
        Plane3d(plane)
    {
    }
    virtual bool checkRay(RayIntersection &ray) override
    {
        if (distanceTo(ray.ray.p) < 1.0)
        {
            return true;
        }
        return false;
    }
};

TEST(Raytrace, DISABLED_testRaytraceChess)
{
    int h = 1080;
    int w = 1920;

    int count = 0;


    Matrix44 Rotation_mesh1 = Matrix44::RotationY(M_PI_4);
    Matrix44 Rotation_mesh2 = Matrix44::RotationZ(M_PI);

    int faceNumber = 8;

    Mesh3DDecorated mesh1;
    Mesh3DDecorated mesh2;



    mesh1.mulTransform(Rotation_mesh2);
    mesh2.mulTransform(Rotation_mesh2);
    mesh1.mulTransform(Rotation_mesh1);
    mesh2.mulTransform(Rotation_mesh1);

    for ( int x = 0;x < faceNumber;x++ )
        for ( int y = 0;y < faceNumber;y++)
        {
            if((x + y) % 2 == 0){


                         mesh1.addTriangle(Vector3dd(x*10,y*10, 0.0),Vector3dd(x*10,10+y*10, 0.0),Vector3dd(10+x*10,10+y*10, 0.0));

                         mesh1.addTriangle(Vector3dd(10+x*10,10+y*10, 0.0),Vector3dd(10+x*10,y*10, 0.0),Vector3dd(x*10,y*10, 0.0));

                         mesh2.addTriangle(Vector3dd(0.0 ,y*10, -x*10 + 0.0),Vector3dd(0.0 ,10+y*10, - x*10 +0.0),Vector3dd(0.0,10+y*10, -10-x*10 +0.0));

                         mesh2.addTriangle(Vector3dd(0.0,10+y*10, -10-x*10 + 0.0),Vector3dd(0.0,y*10, -10-x*10+0.0),Vector3dd(0.0,y*10,-x*10+ 0.0));

                         mesh1.addTriangle(Vector3dd(x*10,0.0,-y*10 + 0.0),Vector3dd(x*10,0.0, -10-y*10+0.0),Vector3dd(10+x*10,0.0,-10-y*10+ 0.0));

                         mesh1.addTriangle(Vector3dd(10+x*10,0.0, -10-y*10+ 0.0),Vector3dd(10+x*10,0.0,-y*10+ 0.0),Vector3dd(x*10,0.0, -y*10+0.0));

            } else {

                        mesh2.addTriangle(Vector3dd(x*10,y*10, 0.0),Vector3dd(x*10,10+y*10, 0.0),Vector3dd(10+x*10,10+y*10, 0.0));

                        mesh2.addTriangle(Vector3dd(10+x*10,10+y*10, 0.0),Vector3dd(10+x*10,y*10, 0.0),Vector3dd(x*10,y*10, 0.0));

                        mesh1.addTriangle(Vector3dd(0.0 ,y*10, -x*10 + 0.0),Vector3dd(0.0 ,10+y*10, - x*10 +0.0),Vector3dd(0.0,10+y*10, -10-x*10 +0.0));

                        mesh1.addTriangle(Vector3dd(0.0,10+y*10, -10-x*10 + 0.0),Vector3dd(0.0,y*10, -10-x*10+0.0),Vector3dd(0.0,y*10,-x*10+ 0.0));

                        mesh2.addTriangle(Vector3dd(x*10,0.0,-y*10 + 0.0),Vector3dd(x*10,0.0, -10-y*10+0.0),Vector3dd(10+x*10,0.0,-10-y*10+ 0.0));

                        mesh2.addTriangle(Vector3dd(10+x*10,0.0, -10-y*10+ 0.0),Vector3dd(10+x*10,0.0,-y*10+ 0.0),Vector3dd(x*10,0.0, -y*10+0.0));
            }
        }



    RaytraceableUnion scene;

    RaytraceableMaterial chessMaterial;
    chessMaterial.ambient = RGBColor::Red().toDouble() * 0.2 ;
    chessMaterial.diffuse = RGBColor::Blue().toDouble() / 255.0;
    chessMaterial.reflCoef = 0.0;
    chessMaterial.reflCoef = 0;
    chessMaterial.specular = RGBColor::Red().toDouble() / 255.0;



    RaytraceableMesh rtmesh1(&mesh1);
    RaytraceableMesh rtmesh2(&mesh2);

    rtmesh1.color = RGBColor::Red().toDouble();
  //  rtmesh1.material = &chessMaterial;

    rtmesh2.color = RGBColor::Blue().toDouble();
   // rtmesh2.material = &chessMaterial;



    scene.elements.push_back(&rtmesh1);
    scene.elements.push_back(&rtmesh2);

    //for ( double pos = -150.0; pos < 150.0; pos+= 2.0, count++)
    for ( double pos = -100.0; pos <= -100.0; pos+= 2.0, count++)

    {
        RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

        RaytraceRenderer renderer;
        renderer.setProjection(new PinholeCameraIntrinsics(
                    Vector2dd(w, h),
                    degToRad(60.0)));
        renderer.position = Affine3DQ::Shift(0, pos, -250.0);

        RaytraceablePointLight light1(RGBColor::White().toDouble(), Vector3dd( -200, -190, -100));
        RaytraceablePointLight light2(RGBColor::Black().toDouble(), Vector3dd( 120, -70,  -200));
        LaserPlaneLight laser(
                    Plane3d::FromNormalAndPoint(
                        Vector3dd::OrtY(),
                        Vector3dd(0, pos - 30, 0)
                    ));

        renderer.object = &scene;
        renderer.lights.push_back(&light1);
        renderer.lights.push_back(&light2);
        renderer.lights.push_back(&laser);

        renderer.ambient = RGBColor(20,20,20).toDouble();

        renderer.supersample = true;
        renderer.sampleNum = 200;

        printf("Processing frame %d for position %lf\n", count, pos );
        renderer.trace(buffer);

        char name[100];
        snprintf2buf(name, "raytrace%d.bmp", count);
        BMPLoader().save(name, buffer);
        delete_safe(buffer);
    }
}


TEST(Raytrace, DISABLED_testScanExample)
{
    int h = 1080;
    int w = 1920;

    int count = 0;
    for ( double pos = -150.0; pos < 150.0; pos+= 2.0, count++)
    {
        RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

        RaytraceRenderer renderer;
        renderer.setProjection(new PinholeCameraIntrinsics(
                    Vector2dd(w, h),
                    degToRad(60.0)));
        renderer.position = Affine3DQ::Shift(0, pos, 0);

        /* Materials */
        RaytraceableMaterial blueMirror;
        blueMirror.ambient = RGBColor::Blue().toDouble() * 0.2 ;
        blueMirror.diffuse = RGBColor::White().toDouble() / 255.0;
        blueMirror.reflCoef = 0.0;
        blueMirror.refrCoef = 0;
        blueMirror.specular = RGBColor::Blue().toDouble() / 255.0;


        RaytraceableSphere sphere1(Sphere3d(Vector3dd(0,20, 250.0), 80.0));
        sphere1.name = "Sphere1";
        sphere1.color = RGBColor::Red().toDouble();
        sphere1.material = &blueMirror;

        RaytraceableSphere sphere2(Sphere3d(Vector3dd(0, -50, 250.0), 60.0));
        sphere2.name = "Sphere2";
        sphere2.color = RGBColor::Green().toDouble();
        sphere2.material = &blueMirror;


        RaytraceableUnion scene;
        scene.elements.push_back(&sphere1);
        scene.elements.push_back(&sphere2);

        RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( -200, -190, 150));
        RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd( 120, -70,  50));
        LaserPlaneLight laser(
                    Plane3d::FromNormalAndPoint(
                        Vector3dd::OrtY(),
                        Vector3dd(0, pos - 30, 0)
                    ));

        renderer.object = &scene;
        renderer.lights.push_back(&light1);
        renderer.lights.push_back(&light2);
        renderer.lights.push_back(&laser);

        renderer.ambient = RGBColor(20,20,20).toDouble();

        printf("Processinf frame %d for position %lf\n", count, pos );
        renderer.trace(buffer);

        char name[100];
        snprintf2buf(name, "raytrace%d.bmp", count);
        BMPLoader().save(name, buffer);
        delete_safe(buffer);
    }
}

TEST(Raytrace, DISABLED_testRaytraceModifiers)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.setProjection(new PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0)));
    renderer.position = Affine3DQ::Identity();

    /* Materials */
    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( 0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));

    RaytraceableSphere sphere1(Sphere3d(Vector3dd(0,0, 150.0), 50.0));
    sphere1.name = "Sphere1";
    sphere1.color = RGBColor::Red().toDouble();
    sphere1.material = MaterialExamples::glass(0.85);

    RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 50.0));
    sphere2.name = "Sphere1";
    sphere2.color = RGBColor::Red().toDouble();
    sphere2.material = MaterialExamples::bumpy();

    RaytraceableUnion scene;
    scene.elements.push_back(&sphere1);
    scene.elements.push_back(&sphere2);

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    renderer.trace(buffer);

    BMPLoader().save("trace-noise.bmp", buffer);
    delete_safe(buffer);
}

TEST(Raytrace, DISABLED_testRaytraceTextures)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.setProjection(new PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0)));
    //renderer.position = Affine3DQ::Shift(0, 0, -200);

    /* Ligths */
    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( 0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));

    Mesh3DDecorated mesh;
    OBJLoader loader;
    std::ifstream file("body-v2.obj", std::ifstream::in);
    loader.loadOBJ(file, mesh);
    //mesh.transform(Matrix44::Shift(0, 50, 2180) * Matrix44::Scale(1) * Matrix44::RotationZ(degToRad(-90)) * Matrix44::RotationY(degToRad(-90)) * Matrix44::RotationX(degToRad(180)));
    mesh.transform(Matrix44::Shift(0, 1200, 1580) * Matrix44::Scale(1.1) * Matrix44::RotationZ(degToRad(-90)) * Matrix44::RotationY(degToRad(-90)) * Matrix44::RotationX(degToRad(180)));

    mesh.dumpInfo();

    RGB24Buffer *texture = BMPLoader().loadRGB("body-v2.bmp");
    if (texture) {
        cout << "Texture Size:" <<  texture->getSize() << endl;
    } else {
        cout << "Can't load texture";
    }

    TextureMaterial textMat;
    textMat.texture = texture;

    RaytraceableOptiMesh roMesh(&mesh);
    roMesh.name = "Rabbit";
    roMesh.color = RGBColor::Blue().toDouble();
    roMesh.material = &textMat;
    roMesh.optimize();

    RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 50.0));
    sphere2.name = "Sphere1";
    sphere2.color = RGBColor::Red().toDouble();
    sphere2.material = MaterialExamples::ex1();

    RaytraceableUnion scene;
    scene.elements.push_back(&roMesh);
    scene.elements.push_back(&sphere2);

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    renderer.trace(buffer);

    BMPLoader().save("trace-noise.bmp", buffer);
    delete_safe(buffer);
}

TEST(Raytrace, testSDF)
{
    SDFRenderable object;
    object.F = [](Vector3dd v) { return 50.0 - (v - Vector3dd(0,0, 150.0)).l2Metric(); };

    RayIntersection ray;
    ray.ray = Ray3d(Vector3dd::OrtZ(), Vector3dd::Zero());
    object.intersect(ray);

    cout << "R:" << ray.ray << endl;
    cout << "P:" << ray.getPoint() << endl;
    cout << "N:" << ray.normal << endl;

    ASSERT_TRUE(ray.t == 100);


    ray.ray = Ray3d(Vector3dd(0.1, 0, 1.0).normalised(), Vector3dd::Zero());
    object.intersect(ray);
    cout << "R:" << ray.ray << endl;
    cout << "P:" << ray.getPoint() << endl;
    cout << "N:" << ray.normal << endl;
}

TEST(Raytrace, testCylinder)
{
    Mesh3D mesh;
    RaytraceableCylinder object;
    /*object.e1 = Vector3dd::OrtX();
    object.e2 = Vector3dd::OrtZ();
    object.n  = Vector3dd::OrtY();*/
    object.h = 50;
    object.r = 20;
    object.setPosition(Vector3dd(0,-10,200));

    Ray3d rays[] = {
      /*  Ray3d(Vector3dd::OrtZ()   , Vector3dd::Zero()),
        Ray3d(Vector3dd::OrtZ()   , Vector3dd(sqrt(object.r), 0, 0)),
        Ray3d(Vector3dd::OrtZ()   , Vector3dd(-19.99, 0, 0)),
        Ray3d(Vector3dd(0, 0.1, 1), Vector3dd(-19.99, 0, 0)),

        Ray3d(Vector3dd(0, 0.1, 1), Vector3dd(-19.99, 0, 0)),*/

        Ray3d(Vector3dd(0, -1, 1), Vector3dd(0, 240, 0)),
    };

    for (unsigned i = 0; i < CORE_COUNT_OF(rays); i++)
    {
        rays[i] = rays[i].normalised();
        RayIntersection ray;        
        ray.ray = rays[i];
        mesh.addLine(rays[i].getPoint(0), rays[i].getPoint(300));
        bool ok = object.intersect(ray);

        cout << i << endl;
        cout << " R:" << ray.ray << endl;
        if (ok) {
        cout << " P:" << ray.getPoint() << endl;
        //cout << " N:" << ray.normal << endl;
        } else  {
            cout << "No Intersection" << endl;
        }

        mesh.addPoint(ray.getPoint());
    }

    object.toMesh(mesh);
    mesh.dumpPLY("cylinder-int.ply");
}

TEST(Raytrace, testCylinder1)
{
    Mesh3D mesh;
    RaytraceableCylinder object;
    /*object.e1 = Vector3dd::OrtX();
    object.e2 = Vector3dd::OrtZ();
    object.n  = Vector3dd::OrtY();*/
    object.h = 50;
    object.r = 20;
    object.setPosition(0,-10,200);

    int limit = 100;

    vector<Ray3d> rays;

    for (int i = - limit; i < limit; i++)
    {
        for (int j = - limit; j < limit; j++)
        {
            /*rays.push_back(Ray3d(Vector3dd(i / (3.0 * limit), j /  (3.0 * limit), 1.0 ), Vector3dd::Zero()));
            rays.back().normalise();*/

            rays.push_back(Ray3d(Vector3dd(i / (2.0 * limit), j /  (2.0 * limit) - 0.4, 1.0 ), Vector3dd(60, 120, 0)));
            rays.back().normalise();
        }
    }

    int nIntersects = 0;
    for (size_t i = 0; i < rays.size(); i++)
    {
        RayIntersection ray;
        ray.ray = rays[i];
        mesh.addLine(rays[i].getPoint(0), rays[i].getPoint(30));
        bool ok = object.intersect(ray);

        ostringstream ss;
        ss << i << endl;
        ss << " R:" << ray.ray << endl;
        if (ok) {
            ss << " P:" << ray.getPoint() << endl;
          //ss << " N:" << ray.normal << endl;
            //L_DDEBUG << ss.str();
            nIntersects++;
        }
        else  {
            ss << "No Intersection" << endl;
        }
        //L_DDEBUG << ss.str();

        mesh.addPoint(ray.getPoint());
    }

    cout << " found intersections: " << nIntersects << " of rays: " << rays.size() << endl;

    //object.toMesh(mesh);
    mesh.addAOB(object.getBoundingBox());
    mesh.dumpPLY("cylinder-int.ply");
}


TEST(Raytrace, testTransform)
{
    Mesh3D mesh;
    mesh.switchColor(true);
    RaytraceableSphere sphere(Sphere3d(Vector3dd(0,10,200), 30));
    RaytraceableTransform object(&sphere, Matrix44::Scale(0.5, 1.0, 1.0));
    int limit = 100;

    vector<Ray3d> rays;


    for (int i = - limit;  i < limit ; i++)
    {
        for (int j = - limit;  j < limit ; j++)
        {
            /*rays.push_back(Ray3d(Vector3dd(i / (3.0 * limit), j /  (3.0 * limit), 1.0 ), Vector3dd::Zero()));
            rays.back().normalise();*/

            rays.push_back(Ray3d(Vector3dd(i / (2.0 * limit), j /  (2.0 * limit) - 0.4, 1.0 ), Vector3dd(60, 120, 0)));
            rays.back().normalise();
        }
    }

    for (size_t i = 0; i < rays.size(); i++)
    {
        RayIntersection ray;
        ray.ray = rays[i];
        mesh.addLine(rays[i].getPoint(0), rays[i].getPoint(30));
        bool ok = object.intersect(ray);

        std::stringstream s;
        s << i << endl;
        s << " R:" << ray.ray << endl;
        if (ok)
        {
            s << " P:" << ray.getPoint() << endl;
            mesh.setColor(RGBColor::Green());
            mesh.addPoint(ray.getPoint());
            ray.object->normal(ray);
            mesh.setColor(RGBColor::Blue());
            mesh.addLine(ray.getPoint(), ray.getPoint() + ray.normal * 5.0);

            //cout << " N:" << ray.normal << endl;
        } else {
            s << "No Intersection" << endl;
        }
        DOTRACE(s.str());
    }

    //object.toMesh(mesh);
    mesh.dumpPLY("transform-int.ply");
}


class SDFCube : public SDFRenderable
{
public:
/*
 *float sdBox(vec3 p, vec3 b) {
  vec3 d = abs(p) - b;
  return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0)) - 0.1;
}

 *
 */
    Vector3dd b;
    double over;

    double operator ()(Vector3dd &p)
    {
        Vector3dd d = p.cwiseAbs() - b;
        return std::min(d.maximum(), 0.0) + d.cwiseMax(Vector3dd::Zero()).l2Metric() + over;
    }

    SDFCube()
    {
        //F = this;
    }
};

TEST(Raytrace, DISABLED_testRaytraceSDF)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.setProjection(new PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0)));
    renderer.position = Affine3DQ::Identity();
    renderer.sky = new RaytraceableSky1();

    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd(  0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));

    SDFRenderable object;
    object.F = [](Vector3dd v) {
        //double step = 200;
        Vector3dd c1 = Vector3dd(0,-30, 150.0);
        Vector3dd c2 = Vector3dd(0, 40, 150.0);

        double d1 = 20.0 - sqrt((v - c1).sumAllElementsSq());
        double d2 = 20.0 - sqrt((v - c2).sumAllElementsSq());

        if (fabs(d1) > fabs(d2))
        {
            return d2;
        }
        return d1;
    };

    SDFRenderable object1;
    object1.F = [](Vector3dd v) {
        //double step = 200;
        Vector3dd c1 = Vector3dd(50,-15, 150.0);
        Vector3dd c2 = Vector3dd(50, 15, 150.0);

        double d1 = 20.0 - sqrt((v - c1).sumAllElementsSq());
        double d2 = 20.0 - sqrt((v - c2).sumAllElementsSq());

        return -1 / ((-1 / d1) + (-1 / d2));
    };



    object.name = "Sphere1";
    object.color = RGBColor::Red().toDouble();
    object.material = MaterialExamples::bumpy();

    object1.name = "Sphere2";
    object1.color = RGBColor::Red().toDouble();
    object1.material = MaterialExamples::bumpy();


    RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 50.0));
    sphere2.name = "Sphere2";
    sphere2.color = RGBColor::Red().toDouble();
    sphere2.material = MaterialExamples::bumpy();

    RaytraceableUnion scene;
    scene.elements.push_back(&object);
    scene.elements.push_back(&object1);
    scene.elements.push_back(&sphere2);

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    renderer.trace(buffer);

    BMPLoader().save("trace-noise.bmp", buffer);

    delete_safe(buffer);
}

TEST(Raytrace, DISABLED_testRaytraceCylinder)
{
    int h = 500;
    int w = 500;
    RaytraceRenderer renderer;
    renderer.setProjection(new PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0)));
    renderer.position = Affine3DQ::Identity();
    //renderer.sky = new RaytraceableSky1();

    RGB24Buffer *cubemap = BufferFactory::getInstance()->loadRGB24Bitmap("cubemap.bmp");
    //cout << "Loaded cubemap" << cubemap->getSize() << endl;
    RaytraceableCubemap cubeMaterial(cubemap);
    renderer.sky = &cubeMaterial;

    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd(  0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));

    Cylinder3d cylinder;
    cylinder.c      = Vector3dd(0,0,200);
    cylinder.normal = Vector3dd::OrtY();
    cylinder.height = 30;
    cylinder.r      = 40;

    RaytraceableCylinder object;
    object.h = 50;
    object.r = 20;
    object.setPosition(0,30,200);

    object.name = "Cylinder";
    object.color = RGBColor::Red().toDouble();
    object.material = new RaytraceableChessMaterial(5.0);
    object.material = MaterialExamples::ex1();

    RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 50.0));
    sphere2.name = "Sphere2";
    sphere2.color = RGBColor::Red().toDouble();
    sphere2.material = MaterialExamples::bumpy();

    RaytraceableUnion scene;
    scene.elements.push_back(&object);
    //scene.elements.push_back(&sphere2);

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    for (int i = 0; i < 150; i++) {
        RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
        double a = degToRad(360.0 / 150 * i);
        Vector3dd dir(0, sin(a), cos(a));
        //renderer.position = Affine3DQ::Shift(dir * 200.0) * Affine3DQ::RotationX(a) *  Affine3DQ::Shift(0, 0, -200.0);
        renderer.position = Affine3DQ::Shift(Vector3dd(0, i * 3 , 0 )) * Affine3DQ::RotationY(0.0);

        renderer.trace(buffer);

        char name[100];
        snprintf2buf(name, "trace-cylinder%d.bmp", i);
        BMPLoader().save(name, buffer);
        delete_safe(buffer);
    }
}
