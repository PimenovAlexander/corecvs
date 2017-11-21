#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/materialExamples.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"
#include "core/buffers/bufferFactory.h"

void addPole(const Vector3dd &position, RaytraceableUnion &scene)
{
    int zones = 14;
    double totalHeight = 200;


    for ( int i = 0; i < zones; i ++)
    {
        RaytraceableCylinder *object = new RaytraceableCylinder;
        object->h = totalHeight / zones ;
        object->r = 4;
        object->setPosition(position + Vector3dd(0, -10 + i * object->h,  0));

        object->name = "Cylinder";
        object->color = (i % 2) ? RGBColor::Black().toDouble() : RGBColor::White().toDouble();
        object->material = NULL;
        //object->material = new RaytraceableChessMaterial(5.0);
        //object->material = (i % 2) ? MaterialExamples::ex1() : MaterialExamples::ex2();
        scene.elements.push_back(object);
    }


}

void raytrace_scene_pole( void )
{
    SYNC_PRINT(("raytrace_scene_pole( void )\n"));
    PreciseTimer timer = PreciseTimer::currentTime();

    int h = 300 * 3;
    int w = 400 * 3;
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

    RaytraceableUnion scene;

    for ( int i = 0; i < 7; i ++)
    {
        addPole(Vector3dd(0,0,1000) + Vector3dd(200,0,1000) * i, scene);
    }

    RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 20.0));
    sphere2.name = "Sphere2";
    sphere2.color = RGBColor::Red().toDouble();
    sphere2.material = MaterialExamples::bumpy();

    scene.elements.push_back(&sphere2);

    renderer.object = &scene;
    renderer.lights.push_back(&light1);
    renderer.lights.push_back(&light2);

    for (int i = 0; i < 1; i++) {
        RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
        double a = degToRad(360.0 / 150 * i);
        Vector3dd dir(0, sin(a), cos(a));
        //renderer.position = Affine3DQ::Shift(dir * 200.0) * Affine3DQ::RotationX(a) *  Affine3DQ::Shift(0, 0, -200.0);
        renderer.position = Affine3DQ::Shift(Vector3dd(0, i * 3 , 0 )) * Affine3DQ::RotationY(0.0);

        renderer.supersample = true;
        renderer.sampleNum = 400;
        renderer.trace(buffer);

        char name[100];
        snprintf2buf(name, "trace-cylinder%d.bmp", i);
        BMPLoader().save(name, buffer);

        scene.optimize();
        renderer.trace(buffer);
        snprintf2buf(name, "trace-cylinder-f%d.bmp", i);
        BMPLoader().save(name, buffer);
        delete_safe(buffer);
    }



    SYNC_PRINT(("Processed: %lf seconds elapsed\n", timer.usecsToNow() / 1e6 ));
}
