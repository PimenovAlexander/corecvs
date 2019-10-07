#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/materialExamples.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"
#include "core/fileformats/meshLoader.h"

void raytrace_scene_speedup(void)
{
    SYNC_PRINT(("raytrace_scene_speedup( void )\n"));
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
    if (!loader.load(&mesh, "bunny.ply"))
    {
        SYNC_PRINT(("Can't load model. Exiting. To get model run: "));
        SYNC_PRINT(("wget http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz"));
        return;
    }
    //loader.load(&mesh, "body-v2.obj");

    //Matrix44::Shift(0, 0 , 3500)
    //Matrix44::Shift(0, 50, 180)

    /*mesh.transform(Matrix44::Shift(0, 50, 180) *
                   Matrix44::Scale(1.5) *
                   Matrix44::RotationZ(degToRad(-90)) *
                   Matrix44::RotationY(degToRad(-90)) *
                   Matrix44::RotationX(degToRad(180)));*/

    mesh.transform(
        Matrix44::Shift(50, 50, 180) *
        Matrix44::Scale(700) *
        Matrix44::RotationY(degToRad(180)) *
        Matrix44::RotationZ(degToRad(180)));

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
    SYNC_PRINT(("Mesh optimise time %lf ms\n", timer.usecsToNow() / 1000.0));
    SYNC_PRINT(("Mesh tree size is   %d\n", roMesh.opt->childCount()));
    SYNC_PRINT(("Mesh tree triangles %d\n", roMesh.opt->elementCount()));

    //Mesh3D dumpTreeMesh;
    //dumpTreeMesh.switchColor(true);
    //roMesh.dumpToMesh(dumpTreeMesh, true, true);
    //dumpTreeMesh.dumpPLY("subtree.ply");

    RaytraceableUnion scene1;
    scene1.elements.push_back(&roMesh);

    for (double d = 60; d < 280; d+=30) {
        RaytraceableSphere* sphere1 = new RaytraceableSphere(Sphere3d(Vector3dd(-80 + d / 10.0 , 0, d), 20.00));
        sphere1->name = "Sphere1";
        sphere1->color = RGBColor::Red().toDouble();
        sphere1->material = MaterialExamples::bumpy();
        scene1.elements.push_back(sphere1);
    }

    renderer.object = &scene1;
    renderer.supersample = false;
    renderer.sampleNum = 260;

    timer = PreciseTimer::currentTime();
    //renderer.trace(bufferF);

    for (double focus = 120; focus < 260; focus += 20)
    {
        char buffer[256];
        renderer.traceFOV(bufferF, 8, focus);
        SYNC_PRINT(("Fast render time %lf ms\n", timer.usecsToNow() / 1000.0));

        snprintf2buf(buffer, "trace-focus%d.bmp", (int)focus);
        BMPLoader().save(buffer, bufferF);
    }

    /*if (!bufferF->isEqualTrace(*bufferS))
    {
        SYNC_PRINT(("Erorr"));
    }*/


    delete_safe(bufferS);
    delete_safe(bufferF);


}
