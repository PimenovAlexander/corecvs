#include "gtest/gtest.h"
#include "core/math/vector/vector3d.h"

#include "core/camerafixture/cameraFixture.h"
#include "core/camerafixture/fixtureCamera.h"
#include "core/camerafixture/fixtureScene.h"

#include "core/reflection/jsonPrinter.h"
#include "core/geometry/mesh/mesh3d.h"

#include "core/fileformats/bmpLoader.h"
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#include "gen/error3PointParameters.h"

#include "core/geometry/ellipticalApproximation.h"
#include "core/math/matrix/similarityReconstructor.h"

#include "errorEstimatorBlock.h"

#if 0
#ifdef WITH_JSONMODERN
#   include "jsonModernReader.h"
    typedef JSONModernReader JSONReader;
#elif defined(WITH_RAPIDJSON)
#   include "rapidJSONReader.h"
    typedef RapidJSONReader  JSONReader;
#else
#   include "jsonGetter.h"   // it depends on Qt!
    typedef JSONGetter       JSONReader;
#endif
#endif

#include <wrappers/jsonmodern/jsonModernReader.h>
typedef JSONModernReader JSONReader;

using namespace corecvs;


/**
 *   There is a method for this is CameraModel  reprojectionError, but it ignores distortion
 **/
EllipticalApproximation1d angularError(FixtureScene *scene, FixtureCamera *cam )
{
    CameraModel model = cam->getWorldCameraModel();

    EllipticalApproximation1d approx;

    for (SceneFeaturePoint *point : scene->featurePoints())
    {
        if (!point->hasObservation(cam))
            continue;

        SceneObservation *obs = point->getObservation(cam);

        Vector3dd pos = point->position;
        Vector2dd projReal = model.project(pos);
        Vector2dd projObs  = obs->getUndist();

        Vector3dd dir1 = model.intrinsics->reverse(projReal);
        Vector3dd dir2 = model.intrinsics->reverse(projObs);

        approx.addPoint(dir1.angleTo(dir2));
    }

    cout << "Angular precision of <" << cam->nameId << "> is "  << radToDeg(approx.getMean()) << "deg" << endl;
    return approx;
}


void manyImages()
{

    for (int i = 0; i <  24; i++)
    {
        std::ostringstream name;
#ifdef WITH_LIBPNG
        name << "point3_" << i  << ".png";
        LibpngFileReader saver;
#else
        name << "point3_" << i  << ".bmp";
        BMPLoader saver;
#endif


        /*Error3PointParameters inputParams;
        JSONReader reader("test1.json");
        reader.visit(inputParams, "params");*/


        ErrorEstimatorBlock block;
        JSONReader reader("test1.json");
        reader.visit(block, "params");
        block.mParameters.setRandomSeed(i);
        block();

        saver.save(name.str(), block.outImage());

    }
}

int main(int argc, char **argv)
{

    /*manyImages();
    return 0;*/



    Error3PointParameters params;
    JSONReader getter("params3points.json");

    if (!getter.hasError()) {
        getter.visit(params, "params");
    } else {
        SYNC_PRINT(("Using default parametes\n"));
        params.setBaseline(3.0);
        params.setDistance(30);
        params.setSamplingIterations(1000);
        params.setObservationError(Vector2dParameters(0.5, 0.5));
        params.setPoint3Distance(30);
        params.setStationErrorSigma(Vector3dParameters(0.005, 0.005, 0.008));
        params.setPoint3ErrorSigma (Vector3dParameters(0.005, 0.005, 0.008));
        params.setTiltErrorDeg(0.03);
        params.setTargetType(TargetType3Point::SINGLE_POINT);
        SYNC_PRINT(("Dumping them to _params3points.json\n"));

        JSONPrinter printer("_params3points.json");
        printer.visit(params, "params");
    }

    cout  << "Using parameters form <" << "params3points.json" << ">\n" << params;


    cout << endl;
    cout << endl;



/*    test1top3point(10.0, 1.5);
    test1top3point(30.0, 1.5);
    test1top3point(50.0, 1.5);*/

     //test1top(params);
     //test1top3point(params);
    ErrorEstimatorBlock block;
    block.setParameters(params);
    block(); /* Execute! */


    return 0;
}


