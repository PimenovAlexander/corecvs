#ifndef FIXTURE_CAMERA_H
#define FIXTURE_CAMERA_H

#include <stdint.h>
#include <unordered_map>

#include "utils/atomicOps.h"
#include "cameracalibration/calibrationLocation.h"  // LocationData
#include "alignment/lensDistortionModelParameters.h"
#include "geometry/line.h"
#include "geometry/convexPolyhedron.h"
#include "alignment/pointObservation.h"
#include "cameracalibration/cameraModel.h"
#include "camerafixture/fixtureScenePart.h"

#include "camerafixture/cameraPrototype.h"  // pls see comment below

namespace corecvs {

class CameraFixture;
class ImageRelatedData;
class SceneFeaturePoint;


class FixtureCamera : public FixtureScenePart, public CameraModel
{
public:
    CameraFixture   *cameraFixture = NULL;

    /**
     *   We are now in transition (since 2016 and probably forever). Camera prototype should prevail and the inheritance of CameraModel
     *   should be removed. So far, open it when it will be ready.
     *   Don't forget to open proper code at the FixtureScene::deleteCameraPrototype(CameraPrototype *cameraPrototype).
     **/
    CameraPrototype *cameraPrototype = NULL;

    /* This variable is not controlled and maintained */
    int             sequenceNumber;

    FixtureCamera(FixtureScene * owner = NULL) :
        FixtureScenePart(owner),
        cameraFixture(NULL)
    {}

    FixtureCamera(
            const PinholeCameraIntrinsics &_intrinsics,
            const CameraLocationData &_extrinsics = CameraLocationData(),
            const LensDistortionModelParameters &_distortion = LensDistortionModelParameters(),
            FixtureScene * owner = NULL)
        : FixtureScenePart(owner)
        , CameraModel(_intrinsics, _extrinsics, _distortion)
        , cameraFixture(NULL)
    {}

    template<class VisitorType, class SceneType = FixtureScene>
    void accept(VisitorType &visitor)
    {
        typedef typename SceneType::ImageType          RealImageType;

        CameraModel::accept(visitor);
        IdType id = getObjectId();
        visitor.visit(id, IdType(0), "id");
        setObjectId(id);

        int imageSize = (int)mImages.size();
        visitor.visit(imageSize, 0, "image.size");
        // cout << "Camera will have " << imageSize << " images" << endl;
        setImageCount(imageSize);

        for (int i = 0; i < imageSize; i++)
        {
            char buffer[100]; snprintf2buf(buffer, "image[%d]", i);
            visitor.visit(*static_cast<RealImageType *>(mImages[i]), buffer);
        }
    }

    /** This is an experimental block of functions  it may change. Please use with caution **/

    /** WHY SO SLOW? **/
    bool projectPointFromWorld(const Vector3dd &point, Vector2dd *projectionPtr = NULL);

    Affine3DQ getWorldLocation();
    void setWorldLocation(const Affine3DQ &location, bool moveFixture = false);

    CameraModel getWorldCameraModel();

    /** Images **/
    void addImageToCamera(ImageRelatedData *data);

    void setImageCount       (size_t count);
    std::vector<ImageRelatedData *> mImages;  /*< Not owned*/

    /** Helper functions **/
    bool addObservation(SceneFeaturePoint *point, bool setUserType);

};





} // namespace corecvs

#endif // #ifndef FIXTURE_CAMERA_H
