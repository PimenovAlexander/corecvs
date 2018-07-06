#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/materialExamples.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"


void raytrace_scene1( void )
{
    SYNC_PRINT(("raytrace_scene1( void )\n"));
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

//    Mesh3DDecorated mesh;
//    MeshLoader().load(&mesh, "bun_zipper.ply");
//    mesh.transform(Matrix44::Shift(0, 40, 80) * Matrix44::Scale(430.0) * Matrix44::RotationX(degToRad(180)));

//    RaytraceableMesh rMesh(&mesh);
//    rMesh.name = "Bunny1";
//    rMesh.color = RGBColor::Blue().toDouble();
//    rMesh.material = &blueMirror;

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

    {
        renderer.trace(buffer);
        BMPLoader().save("raytrace-gn.bmp", buffer);
    }

    {
        renderer.setProjection(new StereographicProjection(Vector2dd(w, h) / 2.0, std::max(h,w) / 2, Vector2dd(w, h)));
        renderer.trace(buffer);
        BMPLoader().save("raytrace-sh.bmp", buffer);
    }

    {
        renderer.setProjection(new EquidistantProjection(Vector2dd(w, h) / 2.0, std::max(h,w) / 2, Vector2dd(w, h)));
        renderer.trace(buffer);
        BMPLoader().save("raytrace-eq-d.bmp", buffer);
    }

    {
        renderer.setProjection(new EquisolidAngleProjection(Vector2dd(w, h) / 2.0, std::max(h,w) / 2, Vector2dd(w, h)));
        renderer.trace(buffer);
        BMPLoader().save("raytrace-eq-s.bmp", buffer);
    }

/*
    {
        renderer.setProjection(new OrthographicProjection(Vector2dd(w, h) / 2.0, std::max(h,w) / 2, Vector2dd(w, h)));
        renderer.trace(buffer);
        BMPLoader().save("raytrace-ort.bmp", buffer);
    }
*/

    delete_safe(buffer);

}


void raytrace_scale( void )
{
    SYNC_PRINT(("raytrace_scale( void )\n"));
    int h = 1600;
    int w = 1600;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    RaytraceableSphere sphere1(Sphere3d(Vector3dd(0,0, 150.0), 50.0));
    sphere1.name = "Sphere1";
    sphere1.color = RGBColor::Red().toDouble();
    sphere1.material = NULL;

    RaytraceablePointLight light1(RGBColor::White() .toDouble(), Vector3dd( 0, -190, 150));
    RaytraceablePointLight light2(RGBColor::Yellow().toDouble(), Vector3dd(-120, -70,  50));


    BumpyMaterial * bumpy = new BumpyMaterial;

    bumpy->ambient = RGBColor::Red().toDouble() * 0.2 ;
    bumpy->diffuse = RGBColor::White().toDouble() / 255.0;
    bumpy->reflCoef = 0.0;
    bumpy->refrCoef = 0.0;
    bumpy->specular = TraceColor(0.0);

    for (double s = 1.0; s > 0.5; s-=0.03)
    {
        RaytraceRenderer renderer;
        renderer.setProjection(new PinholeCameraIntrinsics(Vector2dd(w, h), degToRad(80.0)));

        char name[100];
        RaytraceableTransform transformed(&sphere1, Matrix44::Scale(s, 1.0, 1.0));
        transformed.name = "transformed";
        transformed.color = RGBColor::Blue().toDouble();
        transformed.material = bumpy;

        RaytraceableUnion scene;
        scene.elements.push_back(&transformed);

        renderer.object = &scene;
        renderer.lights.push_back(&light1);
        renderer.lights.push_back(&light2);

        renderer.ambient = RGBColor(20,20,20).toDouble();

        renderer.trace(buffer);
        snprintf2buf(name, "raytrace-scale-%lf.bmp", s);
        BMPLoader().save(name, buffer);
    }

    delete_safe(buffer);
}
