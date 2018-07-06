#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/math/affine.h"
#include "core/rectification/sceneStereoAlignerBlock.h"
#include "core/rectification/stereoAligner.h"

namespace corecvs {

SceneStereoAlignerBlock::SceneStereoAlignerBlock()
{}

int SceneStereoAlignerBlock::operator ()()
{
    if (inScene() == NULL)
    {
        SYNC_PRINT(("SceneStereoAlignerBlock::Fail. No input scene\n"));
        return 1;
    }

    if (camera1 == NULL) {
        camera1 = inScene()->getCameraByNumber(inFixture1(), inCamera1());
    }
    if (camera2 == NULL) {
        camera2 = inScene()->getCameraByNumber(inFixture2(), inCamera2());
    }

    if (camera1 == NULL || camera2 == NULL)
    {
        SYNC_PRINT(("SceneStereoAlignerBlock::Fail. No input camera"));
        return 2;
    }

    Affine3DQ pos1 = camera1->getWorldLocation();
    Affine3DQ pos2 = camera2->getWorldLocation();

    Affine3DQ relativeTransform = pos1.inverted() * pos2;

    cout << "SceneStereoAlignerBlock(): Start transform" << relativeTransform << endl;

    Matrix33 F = camera1->getWorldCameraModel().fundamentalTo(camera2->getWorldCameraModel());
    EssentialMatrix E(F);

    E.prettyPrint();

    /* We would use existing code, though it is an inverse abstraction.
     * We already know all the geometry, but will be using code based on the Essential Matrix */

    Vector2dd lSize = camera1->intrinsics->size();
    Vector2dd rSize = camera2->intrinsics->size();

    Vector2dd size  = (lSize + rSize) / 2;

    Vector3dd direction = Vector3dd(mParameters.zdirX(), mParameters.zdirY(), mParameters.zdirZ());
    if (mParameters.autoZ())
    {
        printf("SceneStereoAlignerBlock::Autoguessing best Z\n");
        direction = StereoAligner::getBestZ(F, size /*, GRANULARITY, distL, distR*/);
    }
    cout << "Using Z value: " << direction << endl;

    /**/
    //ProjectiveTransform  leftTransform(1.0);
    //ProjectiveTransform rightTransform(1.0);

    StereoAligner::getAlignmentTransformation(F, &leftTransform, &rightTransform, direction);

    Vector2dd center = size / 2.0;
    StereoAligner::getLateralCorrectionFixPoint(&leftTransform, &rightTransform, center);

    cout << "LTransform:\n" << leftTransform << endl;
    cout << "RTransform:\n" << rightTransform << endl;

    cout << "Sanity check" << endl;
    EssentialMatrix Ix;
    cout << (leftTransform.transposed() * Ix * rightTransform) / F << endl;

    /* Forming new images*/
    ProjectiveTransform  leftTransformInv =  leftTransform.inv();
    ProjectiveTransform rightTransformInv = rightTransform.inv();

    // RGB24Buffer *resImage1 = inImage1()->doReverseDeformationBlTyped<ProjectiveTransform>(&leftTransform);
    // RGB24Buffer *resImage2 = inImage2()->doReverseDeformationBlTyped<ProjectiveTransform>(&rightTransform);

    RGB24Buffer *resImage1 = inImage1()->doReverseDeformationBlTyped<ProjectiveTransform>(&leftTransformInv);
    RGB24Buffer *resImage2 = inImage2()->doReverseDeformationBlTyped<ProjectiveTransform>(&rightTransformInv);

    setOutImage1(resImage1);
    setOutImage2(resImage2);


    /*Working with areas*/
    Polygon pin1  = Polygon::FromImageSize(inImage1()->getSize());
    Polygon pin2  = Polygon::FromImageSize(inImage2()->getSize());
    pin1.transform( leftTransform);
    pin2.transform(rightTransform);
    Polygon pout1 = Polygon::FromImageSize(resImage1->getSize());
    Polygon pout2 = Polygon::FromImageSize(resImage2->getSize());

    PolygonCombiner c1(pin1, pout1);
    c1.prepare();
    pin1 = c1.intersection();
    PolygonCombiner c2(pin2, pout2);
    c2.prepare();
    pin2 = c2.intersection();

    if (pin1.isConvex()) {
        ConvexPolygon cp = pin1.toConvexPolygon();
        cp.inset(21);
        pin1 = Polygon::FromConvexPolygon(cp);
    }
    if (pin2.isConvex()) {
        ConvexPolygon cp = pin2.toConvexPolygon();
        cp.inset(21);
        pin2 = Polygon::FromConvexPolygon(cp);
    }

    leftArea  = pin1;
    rightArea = pin2;

    //AbstractPainter<RGB24Buffer>(resImage1).drawPolygon(leftArea, RGBColor::Yellow());
    //AbstractPainter<RGB24Buffer>(resImage2).drawPolygon(rightArea, RGBColor::Yellow());

    /*Forming new Fixture*/
    if (mParameters.produceCameras())
    {
        CameraFixture *fixture = inScene()->createCameraFixture();
        fixture->name = "Stereo";
        fixture->setLocation(pos1);

        FixtureCamera *cam1 = inScene()->createCamera();
        cam1->nameId = "Stereo 1";
        FixtureCamera *cam2 = inScene()->createCamera();
        cam2->nameId = "Stereo 2";

        inScene()->addCameraToFixture(cam1, fixture);
        inScene()->addCameraToFixture(cam2, fixture);
        //cam1->extrinsics = CameraLocationData(Affine3DQ::Identity());
        //cam2->extrinsics = CameraLocationData(relativeTransform);

        cam1->extrinsics = CameraLocationData(Affine3DQ::Identity());
        cam2->extrinsics = CameraLocationData(relativeTransform);

        cam1->intrinsics.reset(camera1->intrinsics->clone());
        cam2->intrinsics.reset(camera2->intrinsics->clone());

        if (mParameters.produceObservations())
        {
            /* Making new observations */
            for (size_t i = 0; i < inScene()->featurePoints().size(); i++)
            {
                SceneFeaturePoint *point = inScene()->featurePoints()[i];

                SceneObservation *obs1 = point->getObservation(camera1);
                SceneObservation *obs2 = point->getObservation(camera2);

                if (obs1 != NULL)
                {
                    Vector2dd undist = obs1->getUndist();
                    Vector2dd pos1 = leftTransform * undist;
                    SceneObservation observation(cam1, point, pos1, fixture);
                    point->observations[cam1] = observation;
                }

                if (obs2 != NULL)
                {
                    Vector2dd undist = obs2->getUndist();
                    Vector2dd pos2 = rightTransform * undist;
                    SceneObservation observation(cam2, point, pos2, fixture);
                    point->observations[cam2] = observation;
                }
            }
        }
        setOutCamera1(cam1);
        setOutCamera2(cam2);
    }

    return 0;
}

} //namespace corecvs
