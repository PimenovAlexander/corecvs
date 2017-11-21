#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/materialExamples.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"


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


void raytrace_scene_calibrate( void )
{
    SYNC_PRINT(("raytrace_scene_calibrate( void )\n"));
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

void raytrace_scene_scanner( void )
{
     SYNC_PRINT(("raytrace_scene_scanner( void )\n"));
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
