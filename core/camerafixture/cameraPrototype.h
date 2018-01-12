#ifndef CAMERA_PROTOTYPE_H
#define CAMERA_PROTOTYPE_H

#include "core/camerafixture/fixtureScenePart.h"
#include "core/cameracalibration/cameraModel.h"

namespace corecvs {

class FixtureScene;
class CameraFixture;

class CameraPrototype : public FixtureScenePart, public CameraModel
{
public:
//    CameraFixture   *cameraFixture;

    CameraPrototype(FixtureScene * owner = NULL) :
         FixtureScenePart(owner)
//       , cameraFixture(NULL)
    {}

    CameraPrototype(
            const PinholeCameraIntrinsics &_intrinsics,
            const CameraLocationData &_extrinsics = CameraLocationData(),
            const LensDistortionModelParameters &_distortion = LensDistortionModelParameters(),
            FixtureScene * owner = NULL) :
          FixtureScenePart(owner)
        , CameraModel(_intrinsics, _extrinsics, _distortion)
//        , cameraFixture(NULL)
    {}

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        CameraModel::accept(visitor);
        IdType id = getObjectId();
        visitor.visit(id, IdType(0) , "id");
        setObjectId(id);
    }


    /** This is an experimental block of functions  it may change. Please use with caution **/

    /** WHY SO SLOW? **/
    //bool projectPointFromWorld(const Vector3dd &point, Vector2dd *projetionPtr = NULL);
};

}  // namespace corecvs

#endif // CAMERA_PROTOTYPE_H
