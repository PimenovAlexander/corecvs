#ifndef CALIBRATION_CAMERA_H
#define CALIBRATION_CAMERA_H

#include <unordered_map>

#include "calibrationLocation.h"  // LocationData
#include "lensDistortionModelParameters.h"
#include "line.h"
#include "convexPolyhedron.h"
#include "pointObservation.h"
#include "selectableGeometryFeatures.h"
#include "essentialMatrix.h"
#include "distortionApplicationParameters.h"

/* Future derived */
//#include "rgb24Buffer.h"

namespace corecvs {

/*class RGB24Buffer;*/
class FixtureScene;

/**
 * This class is so far just a common base for all objects in scene heap.
 * Should bring this to other file
 **/
class ScenePart {
public:
    /* No particular reason for this, except to encourage leak checks */
    static int OBJECT_COUNT;

    FixtureScene *ownerScene;

    /* We could have copy constructors and stuff... but so far this is enough */
    ScenePart(FixtureScene * owner = NULL) :
        ownerScene(owner)
    {
        OBJECT_COUNT++;
    }


    virtual ~ScenePart() {
        OBJECT_COUNT++;
    }
};


typedef std::unordered_map<std::string, void *> MetaContainer;



/**
 * XXX: We already have intrinsics class somewhere (CameraIntrinsicsLegacy), but
 *      it is not full enough to hold abstract projective pin-hole model (e.g. skewed/non-rectangular)
 *      So this one is now the one to use
 *
 *
 *  TODO: The idea is that if we merge distorsion calibration WITH extrinsics/intrinsics
 *        calibration, then this method will project point using forward distorsion map
 *
 **/

struct PinholeCameraIntrinsics
{
    const static int DEFAULT_SIZE_X = 2592;
    const static int DEFAULT_SIZE_Y = 1944;


    Vector2dd focal;            /**< Focal length (in px) in two directions */
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    skew;             /**< Skew parameter to compensate for optical axis tilt */
    Vector2dd size;             /**< Imager resolution (in pixel) */
    Vector2dd distortedSize;    /**< Source image resolution (FIXME: probably should move it somewhere) */


    PinholeCameraIntrinsics(
            double fx = 1.0,
            double fy = 1.0,
            double cx = DEFAULT_SIZE_X / 2.0,
            double cy = DEFAULT_SIZE_Y / 2.0,
            double skew = 0.0,
            Vector2dd size = Vector2dd(DEFAULT_SIZE_X, DEFAULT_SIZE_Y),
            Vector2dd distortedSize = Vector2dd(DEFAULT_SIZE_X, DEFAULT_SIZE_Y))
      : focal         (fx, fy)
      , principal     (cx, cy)
      , skew          (skew)
      , size          (size)
      , distortedSize (distortedSize)
    {}

    PinholeCameraIntrinsics(Vector2dd resolution, double hfov);


    /**
     * This actually doesn't differ from matrix multiplication, just is a little bit more lightweight
     *
     **/
    Vector2dd project(const Vector3dd &p) const
    {
        Vector2dd result = (focal * p.xy() + Vector2dd(skew * p.y(), 0.0)) / p.z() + principal;
        return result;
    }

    Vector2dd reprojectionError(const Vector3dd &p, const Vector2dd &pp) const
    {
        return project(p) - pp;
    }
    Vector3dd crossProductError(const Vector3dd &p, const Vector2dd &pp)
    {
        return p.normalised() ^ reverse(pp).normalised();
    }
    double angleError(const Vector3dd &p, const Vector2dd &pp)
    {
        return reverse(pp).normalised().angleTo(p.normalised()) * 180.0 / M_PI;
    }
    Vector3dd rayDiffError(const Vector3dd &p, const Vector2dd &pp)
    {
        return reverse(pp).normalised() - p.normalised();
    }


    Vector3dd reverse(const Vector2dd &p) const
    {
        Vector2dd result;
        result[1] = (p[1] - principal[1]) / focal[1];
        result[0] = (p[0] - skew * result[1] - principal[0]) / focal[0];
        return Vector3dd(result.x(), result.y(), 1.0);
    }

    bool isVisible(const Vector3dd &p) const
    {
        if (p[2] <= 0.0) {
            return false;
        }
        Vector2dd proj = project(p);

        if (!proj.isInRect(Vector2dd(0.0, 0.0), size))
        {
            return false;
        }
        return true;
    }

    explicit operator Matrix33() const  { return getKMatrix33(); }

    Matrix44 getKMatrix() const;
    Matrix44 getInvKMatrix() const;

    Matrix33 getKMatrix33() const;
    Matrix33 getInvKMatrix33() const;

    double   getVFov() const;
    double   getHFov() const;

    double   getAspect() const          { return size.y() / size.x(); }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        static const Vector2dd DEFAULT_SIZE(DEFAULT_SIZE_X, DEFAULT_SIZE_Y);

        visitor.visit(focal.x()    , 1.0                 , "fx"  );
        visitor.visit(focal.y()    , 1.0                 , "fy"  );
        visitor.visit(principal.x(), DEFAULT_SIZE_X / 2.0, "cx"  );
        visitor.visit(principal.y(), DEFAULT_SIZE_Y / 2.0, "cy"  );
        visitor.visit(skew         , 0.0                 , "skew");
        visitor.visit(size         , DEFAULT_SIZE        , "size");
        visitor.visit(distortedSize, DEFAULT_SIZE        , "distortedSize");
    }

    /* Helper pseudonim getters */
    double  w() const    { return size.x();      }
    double  h() const    { return size.y();      }

    double cx() const    { return principal.x(); }
    double cy() const    { return principal.y(); }
    double fx() const    { return focal.x(); }
    double fy() const    { return focal.y(); }

};


class CameraModel /*: public ScenePart*/
{
public:
    /**/
    PinholeCameraIntrinsics         intrinsics;
    /**/
    LensDistortionModelParameters   distortion;
    /**/
    CameraLocationData              extrinsics;

    /* cache for rotation should be introduced. First of all it is faster... */
    //Matrix33 rotMatrix;

public:

    /* This should be moved to the derived class */
    /*RGB24Buffer    *image;*/
    std::string     nameId;

public:
    CameraModel(/*FixtureScene * owner = NULL*/) /*:
        ScenePart(owner)*/
    {}

    CameraModel(
            const PinholeCameraIntrinsics &_intrinsics,
            const CameraLocationData &_extrinsics = CameraLocationData(),
            const LensDistortionModelParameters &_distortion = LensDistortionModelParameters())
      : intrinsics(_intrinsics)
      , distortion(_distortion)
      , extrinsics(_extrinsics)
    {}



    template <bool full=false>
    Vector2dd project(const Vector3dd &p) const
    {
        Vector2dd v = intrinsics.project(extrinsics.project(p));
        if (full)
            return distortion.mapForward(v);
        return v;
    }

    Vector2dd project(const Vector3dd &p, bool full) const
    {
        return full ? project<true>(p) : project<false>(p);
    }

    Vector2dd reprojectionError(PointObservation &observation)
    {
        return observation.projection - project(observation.point);
    }

    Vector2dd reprojectionError(const Vector3dd &p, const Vector2dd &pp) const
    {
        return intrinsics.reprojectionError(extrinsics.project(p), pp);
    }
    Vector3dd crossProductError(const Vector3dd &p, const Vector2dd &pp)
    {
        return intrinsics.crossProductError(extrinsics.project(p), pp);
    }
    double angleError(const Vector3dd &p, const Vector2dd &pp)
    {
        return intrinsics.angleError(extrinsics.project(p), pp);
    }
    Vector3dd rayDiffError(const Vector3dd &p, const Vector2dd &pp)
    {
        bool fail = std::isnan(p[2]);
        for (int i = 0; i < 2; ++i)
            fail |= std::isnan(p[i]) || std::isnan(pp[i]);
        if (fail)
            std::cout << "CAM:" << p << " " << pp << std::endl;
        return intrinsics.rayDiffError(extrinsics.project(p), pp);
    }

    /**
     * Return a direction in camera corrdinate frame passing though a point p.
     *
     * This only uses extrinsics and provides the way to intercept the projection process before
     * intrinsics application take action
     *
     * \param p
     **/
    Vector3dd dirToPoint(const Vector3dd &p)
    {
        return extrinsics.project(p);
    }

    Matrix33 fundamentalTo(const CameraModel &right) const;
    Matrix33 essentialTo  (const CameraModel &right) const;
    EssentialDecomposition essentialDecomposition(const CameraModel &right) const;

    /**
     * Only checks for the fact that point belongs to viewport.
     * If you are projecting 3d point you should be sure that point is in front
     **/
    bool isVisible(const Vector2dd &point)
    {
        return point.isInRect(Vector2dd(0.0,0.0), intrinsics.size);
    }

    /**
     * Checks full visibility of 3d point
     **/
    bool isVisible(const Vector3dd &pt) const
    {
       return intrinsics.isVisible(extrinsics.project(pt));
    }

    bool isInFront(const Vector3dd &pt)
    {
        return ((pt - extrinsics.position) & forwardDirection()) > 0;
    }

    /**
     * Setups intrinsics.size and modifies distortion shift/scale
     * in a proper way
     */
    void estimateUndistortedSize(const DistortionApplicationParameters &applicationParams);

    Ray3d               rayFromPixel(const Vector2dd &point) const;
    Vector3dd           dirFromPixel(const Vector2dd &point) const
    {
        return (extrinsics.orientation.conjugated() * intrinsics.reverse(point)).normalised();
    }
    Ray3d               rayFromCenter();

    Vector3dd           forwardDirection() const;
    Vector3dd           topDirection() const;
    Vector3dd           rightDirection() const;

    Matrix33            getRotationMatrix() const;
    Matrix44            getCameraMatrix() const;
    Vector3dd           getCameraTVector() const;

    ConvexPolyhedron    getViewport(const Vector2dd &p1, const Vector2dd &p2);


    void copyModelFrom(const CameraModel &other) {
        intrinsics = other.intrinsics;
        distortion = other.distortion;
        extrinsics = other.extrinsics;
    }

    void setLocation(const Affine3DQ &location)
    {
        extrinsics = CameraLocationData(location);
    }

    Affine3DQ getAffine() const
    {
        return extrinsics.toAffine3D();
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(intrinsics, PinholeCameraIntrinsics()      , "intrinsics");
        visitor.visit(extrinsics, CameraLocationData()           , "extrinsics");
        visitor.visit(distortion, LensDistortionModelParameters(), "distortion");
        visitor.visit(nameId,     std::string("")                , "nameId"    );
    }


    friend ostream& operator << (ostream &out, CameraModel &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void prettyPrint(std::ostream &out = cout);
};

//typedef std::vector<PointObservation> PatternPoints3d;
typedef std::vector<ObservationList>  MultiCameraPatternPoints;


} // namespace corecvs

#endif // CALIBRATION_CAMERA_H
