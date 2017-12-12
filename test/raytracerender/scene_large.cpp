#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/materialExamples.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"
#include "core/buffers/bufferFactory.h"
#include "core/geometry/raytrace/sdfRenderable.h"
#include "core/geometry/raytrace/sdfRenderableObjects.h"
//#include "core/math/vector/vector3ddUtils.h"

void raytrace_scene_large( void )
{
      SYNC_PRINT(("raytrace_scene_large( void )\n"));


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


      SDFRenderableBox box = SDFRenderableBox(
                  Vector3dd(-50.0, 40.0, 110.0), Vector3dd(5.0, 20.0, 2.0));
      SDFRenderableEllipsoid ellipsoid = SDFRenderableEllipsoid(
                  Vector3dd(-50.0, 0.0, 140.0), Vector3dd(20.0, 7.0, 25.0));
      SDFRenderableRoundedBox roundBox = SDFRenderableRoundedBox(
                  Vector3dd(-50.0, -40.0, 110.0), Vector3dd(7.0, 10.0, 3.0), 1.0);
      SDFRenderableTorus torus = SDFRenderableTorus(
                  Vector3dd(60.0, 60.0, 130.0), Vector2d<double>(10.0, 2.0));
      SDFRenderableCone cone = SDFRenderableCone(
                  Vector3dd(0.0, 40.0, 120.0), Vector3dd(1.5, 2.1, 4.1));
      SDFRenderableCylinder cylinder = SDFRenderableCylinder(
                  Vector3dd(50.0, -40.0, 130.0), Vector2d<double>(15.0, 12.0));
      SDFRenderableSphere spherer = SDFRenderableSphere(
                  Vector3dd(50,20, 150.0), 20.0);
      SDFRenderableSphere sphere = SDFRenderableSphere(
                  Vector3dd(-10, -20, 150.0), 20.0);
      SDFRenderableTorus88 torus88 = SDFRenderableTorus88(
                  Vector3dd(20.0, 60.0, 130.0), Vector2d<double>(10.0, 2.0));
      SDFRenderableTorus82 torus82 = SDFRenderableTorus82(
                  Vector3dd(-20.0, 60.0, 130.0), Vector2d<double>(7.0, 2.0));
      SDFRenderableBox box2 = SDFRenderableBox(
                  Vector3dd(50, 20, 150.0), Vector3dd(18.0, 18.0, 18.0));
      SDFRenderableSubstraction sphereSub = SDFRenderableSubstraction(
                  box2, spherer);

      ellipsoid.name = "ellipsoid";
      ellipsoid.color = RGBColor::Red().toDouble();
      ellipsoid.material = MaterialExamples::bumpy();

      sphere.name = "Sphere1";
      sphere.color = RGBColor::Red().toDouble();
      sphere.material = MaterialExamples::bumpy();

      RGBColor color = RGBColor::lerpColor(RGBColor::Brown(), RGBColor::Green(), (double) 50/ 5.0);
      box.name = "Box";
      box.color = RGBColor::Red().toDouble();
      box.material = MaterialExamples::ex3(color);

      roundBox.name = "RoundBox";
      roundBox.color = RGBColor::Red().toDouble();
      roundBox.material = MaterialExamples::ex3(color);

      torus.name = "Torus";
      torus.color = RGBColor::Red().toDouble();
      torus.material = MaterialExamples::bumpy();

      torus88.name = "Torus88";
      torus88.color = RGBColor::Red().toDouble();
      torus88.material = MaterialExamples::ex3(color);

      torus82.name = "Torus82";
      torus82.color = RGBColor::Red().toDouble();
      torus82.material = MaterialExamples::bumpy();

      cone.name = "Cone";
      cone.color = RGBColor::Red().toDouble();
      cone.material = MaterialExamples::ex3(color);

      cylinder.name = "Cylinder";
      cylinder.color = RGBColor::Red().toDouble();
      cylinder.material = MaterialExamples::bumpy();

      sphereSub.name = "SphereSubstraction";
      sphereSub.color = RGBColor::Red().toDouble();
      sphereSub.material = MaterialExamples::ex3(color);

      RaytraceableUnion scene;
      scene.elements.push_back(&ellipsoid);
      scene.elements.push_back(&sphere);
      scene.elements.push_back(&sphereSub);
      scene.elements.push_back(&torus);
      scene.elements.push_back(&torus88);
      scene.elements.push_back(&torus82);
      scene.elements.push_back(&roundBox);
      scene.elements.push_back(&box);
      scene.elements.push_back(&cone);
      scene.elements.push_back(&cylinder);


      renderer.object = &scene;
      renderer.lights.push_back(&light1);
      renderer.lights.push_back(&light2);

      renderer.trace(buffer);

      BMPLoader().save("trace-noise.bmp", buffer);

      delete_safe(buffer);
}

bool addLevel(const Matrix44 &transform, RaytraceableUnion &output, int depth, double height, double width)
{
    const int limit = 7;
    double angle = 45;
    static const int branches[] = {3, 4, 3, 2, 3};
    static std::mt19937 rnd;
    static std::uniform_real_distribution<double> azimuth0(-degToRad(180), degToRad(180));
    static std::uniform_real_distribution<double> azimuthD(-degToRad( 30), degToRad( 30));

    static std::uniform_real_distribution<double> heightL(0.9, 1.1);

    if (depth > limit || width <= 0)
        return true;

    RaytraceableCylinder *object = new RaytraceableCylinder;
    object->h = (depth == limit)  ? 0.25  : height * heightL(rnd);
    object->r = (depth == limit)  ? 5.0 : width ;
    object->setPosition(transform.translationPart());
    object->setRotation(transform.topLeft33());

    RGBColor color = (depth == limit)  ?  RGBColor::Green() : RGBColor::lerpColor(RGBColor::Brown(), RGBColor::Amber(), (double) depth/ 5.0);

    object->name = "Cylinder";
    object->color = color.toDouble();
    object->material = MaterialExamples::ex3(color);

    output.elements.push_back(object);

    //Vector3dd currentTop = object->h * (transform.topLeft33() * Vector3dd::OrtZ()).normalised();
    Vector3dd currentTop = object->h * Vector3dd::OrtZ();

    Matrix44 topShift = Matrix44::Shift(currentTop);
    Matrix44 atTop = transform * topShift;

    double zRot0 = azimuth0(rnd);

    int recusition = depth < CORE_COUNT_OF(branches) ? branches[depth] :  3;
    for (int i = 0; i < recusition; i++)
    {
        double zRot = zRot0 + i * (degToRad(360) / recusition) + azimuthD(rnd);
        Matrix44 rotation = Matrix44::RotationZ(zRot) * Matrix44::RotationY(angle);
        addLevel(atTop * rotation, output, depth + 1, height * 0.64, width * 0.7);
    }

    /*
    addLevel(atTop * Matrix44::RotationY(-degToRad(angle)), output, depth + 1, height * 0.7, width * 0.7);
    addLevel(atTop * Matrix44::RotationY( degToRad(angle)), output, depth + 1, height * 0.7, width * 0.7);
    */

    return true;
}

void raytrace_scene_tree( void )
{
      SYNC_PRINT(("raytrace_scene_tree( void )\n"));


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



      RaytraceableSphere sphere2(Sphere3d(Vector3dd(-80,0, 250.0), 50.0));
      sphere2.name = "Sphere2";
      sphere2.color = RGBColor::Red().toDouble();
      sphere2.material = MaterialExamples::bumpy();

      RaytraceableUnion scene;

      addLevel(Matrix44::Shift(Vector3dd(0, 20, 200)) * Matrix44::RotationX(degToRad(90)) , scene, 0, 50, 5);
      //addLevel(Matrix44::Identity(), scene, 0, 50, 5);


      scene.elements.push_back(&sphere2);

      scene.optimize();

      Mesh3D dump;
      dump.switchColor();
      scene.toMesh(dump);
      dump.dumpPLY("trace-tree.ply");


      renderer.object = &scene;
      renderer.lights.push_back(&light1);
      renderer.lights.push_back(&light2);

      renderer.trace(buffer);

      BMPLoader().save("trace-tree.bmp", buffer);

      delete_safe(buffer);
}
