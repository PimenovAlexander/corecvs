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
#include <objLoader.h>
#include "gtest/gtest.h"

#include "global.h"

#include "raytrace/raytraceRenderer.h"
#include "raytrace/raytraceObjects.h"
#include "raytrace/sdfRenderable.h"
#include "raytrace/materialExamples.h"

#include "bmpLoader.h"
#include "meshLoader.h"
#include "preciseTimer.h"
#include "perlinNoise.h"

using namespace std;
using namespace corecvs;


TEST(Raytrace, testRaytraceBase)
{
    int h = 1600;
    int w = 1600;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.intrisics = PinholeCameraIntrinsics(Vector2dd(w, h), degToRad(80.0));

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
    SYNC_PRINT(("Total time %ld us", timer.usecsToNow()));


    BMPLoader().save("raytrace.bmp", buffer);
    delete_safe(buffer);

}



TEST(Raytrace, testRaytraceSpeedup)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *bufferS = new RGB24Buffer(h, w, RGBColor::Black());
    RGB24Buffer *bufferF = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.intrisics = PinholeCameraIntrinsics(Vector2dd(w, h), degToRad(60.0));

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
    SYNC_PRINT(("Mesh tree triangles %d\n", roMesh.opt->triangleCount()));

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

TEST(Raytrace, testRaytraceChess)
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
        renderer.intrisics = PinholeCameraIntrinsics(
                    Vector2dd(w, h),
                    degToRad(60.0));
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


TEST(Raytrace, testScanExample)
{
    int h = 1080;
    int w = 1920;

    int count = 0;
    for ( double pos = -150.0; pos < 150.0; pos+= 2.0, count++)
    {
        RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

        RaytraceRenderer renderer;
        renderer.intrisics = PinholeCameraIntrinsics(
                    Vector2dd(w, h),
                    degToRad(60.0));
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


TEST(Raytrace, testRaytraceModifiers)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.intrisics = PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0));
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

TEST(Raytrace, testRaytraceTextures)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.intrisics = PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0));
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

    ray.ray = Ray3d(Vector3dd(0.1, 0, 1.0).normalised(), Vector3dd::Zero());
    object.intersect(ray);
    cout << "R:" << ray.ray << endl;
    cout << "P:" << ray.getPoint() << endl;
    cout << "N:" << ray.normal << endl;


}

TEST(Raytrace, testRaytraceSDF)
{
    int h = 1500;
    int w = 1500;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceRenderer renderer;
    renderer.intrisics = PinholeCameraIntrinsics(
                Vector2dd(w, h),
                degToRad(60.0));
    renderer.position = Affine3DQ::Identity();

    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( 0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));

    SDFRenderable object;
    object.F = [](Vector3dd v) {
        Vector3dd c1 = Vector3dd(0,-30, 150.0);
        Vector3dd c2 = Vector3dd(0, 30, 150.0);

        return  sqrt(400.0 / ((v - c1).sumAllElementsSq() +
                              (v - c2).sumAllElementsSq()));
    };
    object.name = "Sphere1";
    object.color = RGBColor::Red().toDouble();
    object.material = MaterialExamples::bumpy();

    RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 50.0));
    sphere2.name = "Sphere2";
    sphere2.color = RGBColor::Red().toDouble();
    sphere2.material = MaterialExamples::bumpy();

    RaytraceableUnion scene;
    scene.elements.push_back(&object);
    scene.elements.push_back(&sphere2);

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    renderer.trace(buffer);

    BMPLoader().save("trace-noise.bmp", buffer);


    delete_safe(buffer);
}
