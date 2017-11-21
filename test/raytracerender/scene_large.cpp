#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/geometry/raytrace/materialExamples.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/preciseTimer.h"
#include "core/buffers/bufferFactory.h"
#include "core/geometry/raytrace/sdfRenderable.h"


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
