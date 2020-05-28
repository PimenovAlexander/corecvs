#pragma once
/**
 * \file cameraFixture.h
 * \ingroup cppcorefiles
 *
 **/
#include <type_traits>
#include <cstring>

#include "alignment/pointObservation.h"

#include "utils/typesafeBitmaskEnums.h"
#include "cameracalibration/calibrationLocation.h"  // LocationData
#include "fixtureCamera.h"
#include "fixtureScenePart.h"

namespace corecvs {

/**
 *   See FixtureScene for more data on ownership of the objectes in structure
 **/
class CameraFixture : public FixtureScenePart
{
public:
    std::vector<FixtureCamera *> cameras;
    Affine3DQ                    location;
    std::string                  name;

    int                          sequenceNumber;

    CameraFixture(FixtureScene *owner = NULL) : FixtureScenePart(owner)
    {
        //SYNC_PRINT(("CameraFixture():CameraFixture(owner = %p): called \n", owner));
    }

    CameraFixture(
        const std::vector<FixtureCamera *> & _cameras,
        const Affine3DQ &_location = Affine3DQ())
      : cameras(_cameras)
      , location(_location)
    {}

    /** This is a legacy compatibilty block **/

    CameraFixture(
        const std::vector<FixtureCamera *> & _cameras,
        const CameraLocationData &_location)
      : cameras(_cameras)
      , location(_location.toMockAffine3D())
    {}

    CameraLocationData getLocation() const
    {
        return CameraLocationData(location);
    }

    void setLocation(const CameraLocationData &_location)
    {
        location = _location.toMockAffine3D();
    }

    /* New style setter */
    void setLocation(const Affine3DQ &_location)
    {
        location = _location;
    }

	void transformLocation(const Matrix44& coordinatesTransform);
	       Affine3DQ getTransformedLocation(const Matrix44& coordinatesTransform) const;
    static Affine3DQ getTransformedLocation(const Matrix44& coordinatesTransform, const Affine3DQ &location);

    FixtureCamera getWorldCamera(FixtureCamera *camPtr) const
    {
        FixtureCamera toReturn = *camPtr;
        toReturn.extrinsics.transform(location);
        toReturn.cameraFixture = nullptr;           // as we've detached camera from the fixture for proper working camera.getWorldLocation()
        return toReturn;
    }

    FixtureCamera getWorldCamera(int cam) const
    {
        return getWorldCamera(cameras[cam]);
    }

    FixtureCamera getRawCamera(int cam) const
    {
     /*   auto c = cameras[cam];
        c.extrinsics.orientation = c.extrinsics.orientation ^ location.orientation;
        c.extrinsics.position = (location.orientation.conjugated() * cameras[cam].extrinsics.position) + location.position;
        return c;*/
        return getWorldCamera(cam);
    }

    double scoreFundamental(FixtureCamera *thisCamera, Vector2dd thisPoint, CameraFixture *otherFixture, FixtureCamera *otherCamera, Vector2dd otherPoint)
    {
        auto FAB = fundamentalTo(thisCamera, otherFixture, otherCamera);
        corecvs::Line2d left = FAB.mulBy2dRight(otherPoint);
        corecvs::Line2d right= FAB.mulBy2dLeft (thisPoint);
        return std::max(left.distanceTo(thisPoint), right.distanceTo(otherPoint));
    }

    int getCameraId(FixtureCamera* ptr) const
    {
        for (auto& i: cameras)
            if (i == ptr)
                return &i - &cameras[0];
        return -1;
    }

    void setCameraCount(size_t count);


    Matrix44 getMMatrix(int cam) const
    {
        return getRawCamera(cam).getCameraMatrix();
    }
    Matrix44 getMMatrix(FixtureCamera *cam) const
    {
        auto M=getWorldCamera(cam).getCameraMatrix();
        return M;
    }

    Vector2dd reprojectionError(const Vector3dd &p, const Vector2dd &pp, FixtureCamera* cam)
    {
        return cam->reprojectionError(location.inverted().apply(p), pp);
    }
    Vector3dd crossProductError(const Vector3dd &p, const Vector2dd &pp, FixtureCamera* cam)
    {
        return cam->crossProductError(location.inverted().apply(p), pp);
    }

    double angleErrorRad(const Vector3dd &p, const Vector2dd &pp, FixtureCamera* cam)
    {
        return cam->angleErrorRad(location.inverted().apply(p), pp);
    }


    Vector3dd rayDiffError(const Vector3dd &p, const Vector2dd &pp, FixtureCamera* cam)
    {
        return cam->rayDiffError(location.inverted().apply(p), pp);
    }


    Vector2dd project(const Vector3dd &pt, int cam) const
    {
        return cameras[cam]->project(location.inverted().apply(pt));
    }
    Vector2dd project(const Vector3dd &pt, FixtureCamera *cam) const
    {
        return cam->project(location.inverted().apply(pt));
    }

    bool isVisible(const Vector3dd &pt, int cam) const
    {
        return cameras[cam]->isVisible(location.inverted().apply(pt));
    }
    bool isVisible(const Vector3dd &pt, FixtureCamera *cam) const
    {
        return cam->isVisible(location.inverted().apply(pt));
    }

    bool isVisible(const Vector3dd &pt) const
    {
        for (int i = 0; i < (int)cameras.size(); ++i)
        {
            if (isVisible(pt, i))
                return true;
        }
        return false;
    }

    Matrix33 fundamentalTo(FixtureCamera *thisCam, CameraFixture *other, FixtureCamera *otherCam)
    {
        if (!thisCam->intrinsics->isPinhole() || !otherCam->intrinsics->isPinhole())
            return EssentialMatrix();

        PinholeCameraIntrinsics *intrinsics1 = static_cast<PinholeCameraIntrinsics *>(thisCam ->intrinsics.get());
        PinholeCameraIntrinsics *intrinsics2 = static_cast<PinholeCameraIntrinsics *>(otherCam->intrinsics.get());

        Matrix33 K1 = intrinsics1->getKMatrix33();
        Matrix33 K2 = intrinsics2->getKMatrix33();
        return K1.inv().transposed() * essentialTo(thisCam, other, otherCam) * K2.inv();
    }

    EssentialDecomposition essentialTo(FixtureCamera *thisCam, CameraFixture *other, FixtureCamera *otherCam)
    {
        auto thisExtrinsics = thisCam->extrinsics,
             otherExtrinsics = otherCam->extrinsics;
        thisExtrinsics.transform(location);
        otherExtrinsics.transform(other->location);
        return CameraModel::ComputeEssentialDecomposition(thisExtrinsics, otherExtrinsics);
    }

    inline Ray3d rayFromPixel(FixtureCamera *cam, const Vector2dd &point) const
    {
        return location * cam->rayFromPixel(point);
    }

    template<class VisitorType, class SceneType = FixtureScene>
    void accept(VisitorType &visitor)
    {
        typedef typename SceneType::CameraType   RealCameraType;
//        typedef typename SceneType::FixtureType  RealFixtureType;
//        typedef typename SceneType::PointType    RealPointType;

        /* So far compatibilty is on */
        int camsize = (int)cameras.size();
        visitor.visit(camsize, 0, "cameras.size");

        setCameraCount(camsize);

        for (size_t i = 0; i < (size_t)camsize; i++)
        {
            char buffer[100];
            snprintf2buf(buffer, "cameras[%d]", i);
            visitor.visit(*static_cast<RealCameraType *>(cameras[i]), buffer);
        }

        /*
        CameraLocationData loc = getLocation();
        visitor.visit(loc, CameraLocationData(), "location");
        setLocation(loc);
        */
        visitor.visit(location, Affine3DQ(), "location");
        visitor.visit(name, std::string(""), "name");
    }

};

} // namespace corecvs
