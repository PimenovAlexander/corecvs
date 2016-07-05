#pragma once

#include <unordered_map>

#include "calibrationLocation.h"  // LocationData
#include "lensDistortionModelParameters.h"
#include "line.h"
#include "convexPolyhedron.h"
#include "pointObservation.h"
#include "calibrationCamera.h"

namespace corecvs {

class FixtureScene;
class CameraFixture;

/**
 * This class is so far just a common base for all objects in scene heap.
 * Should bring this to other file
 **/
class FixtureScenePart {

public:
    /* No particular reason for this, except to encourage leak checks */
    static atomic_int OBJECT_COUNT;

    FixtureScene *ownerScene;

    /**
     * Ok this id should be reworked...
     * Nice idea is for the id to be pointer itself.
     *
     * This can't be true in case of the moment after serialisation...
     * we need to invent a way how to make sure it is ok.
     *
     **/

     typedef uint64_t IdType;
private:
    IdType id;
public:
    IdType getObjectId() {
        if (id == 0) {
            id = reinterpret_cast<IdType>(this);
        }
        return id;
    }

    void setObjectId(IdType id) {
        this->id = id;
    }

    /* We could have copy constructors and stuff... but so far this is enough */
    FixtureScenePart(FixtureScene * owner = NULL) :
        ownerScene(owner),
        id (0)
    {
        atomic_inc_and_fetch(&OBJECT_COUNT);
    }


    virtual ~FixtureScenePart() {
        atomic_dec_and_fetch(&OBJECT_COUNT);
    }
};


typedef std::unordered_map<std::string, void *> MetaContainer;

class FixtureCamera : public FixtureScenePart, public CameraModel
{
public:
    CameraFixture   *cameraFixture;

    /* This variable is not contorlled and maintained */
    int sequenceNumber;

    FixtureCamera(FixtureScene * owner = NULL) :
        FixtureScenePart(owner),
        cameraFixture(NULL)
    {}

    FixtureCamera(
            const PinholeCameraIntrinsics &_intrinsics,
            const CameraLocationData &_extrinsics = CameraLocationData(),
            const LensDistortionModelParameters &_distortion = LensDistortionModelParameters(),
            FixtureScene * owner = NULL) :
        FixtureScenePart(owner),
        CameraModel(_intrinsics, _extrinsics, _distortion),
        cameraFixture(NULL)
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
    bool projectPointFromWorld(const Vector3dd &point, Vector2dd *projetionPtr = NULL);


};


} // namespace corecvs
