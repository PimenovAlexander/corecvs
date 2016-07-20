/**
 * \file main_test_raytrace.cpp
 * \brief This is the main file for the test raytrace 
 *
 * \date мая 06, 2016
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "raytrace/raytraceRenderer.h"
#include "bmpLoader.h"
#include "meshLoader.h"

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

    Mesh3D mesh;
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

    renderer.trace(buffer);

    BMPLoader().save("raytrace.bmp", buffer);
    delete_safe(buffer);

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

TEST(Raytrace, testRaytraceExample)
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
                    Plane3d::FormNormalAndPoint(
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
