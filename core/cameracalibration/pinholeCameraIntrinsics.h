#ifndef PINHOLECAMERAINTRINSICS_H
#define PINHOLECAMERAINTRINSICS_H

#include "core/math/matrix/matrix44.h"
#include "core/cameracalibration/projectionModels.h"
#include "core/math/mathUtils.h"

namespace corecvs {

/**
 * XXX: We already have intrinsics class somewhere (CameraIntrinsicsLegacy), but
 *      it is not full enough to hold abstract projective pin-hole model (e.g. skewed/non-rectangular)
 *      So this one is now the one to use
 *
 *
 *  TODO: The idea is that if we merge distorsion calibration WITH extrinsics/intrinsics
 *        calibration, then this method will project point using forward distorsion map
 *
 *  Now  PinholeCameraIntrinsics has vtable because of CameraProjection base class.
 *  Probably this needs to be changed
 *
 **/

struct PinholeCameraIntrinsics : public CameraProjection
{
    const static int DEFAULT_SIZE_X = 2592;
    const static int DEFAULT_SIZE_Y = 1944;


    Vector2dd focal;            /**< Focal length (in px) in two directions */
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    skew;             /**< Skew parameter to compensate for optical axis tilt */
    Vector2dd size;             /**< Model image resolution (in pixel) */
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

    PinholeCameraIntrinsics(Vector2dd resolution, Vector2dd principal, double focal, double skew = 0.0) :
        focal(focal, focal),
        principal(principal),
        skew(skew),
        size(resolution),
        distortedSize(resolution)
    {
    }

    /**
     * This actually doesn't differ from matrix multiplication, just is a little bit more lightweight
     *
     **/
    virtual Vector2dd project(const Vector3dd &p) const override
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

    /**
     * \depricated Violates degToRad Policy.
     **/
    double angleError(const Vector3dd &p, const Vector2dd &pp)
    {
        return radToDeg(reverse(pp).normalised().angleTo(p.normalised()));
    }
    Vector3dd rayDiffError(const Vector3dd &p, const Vector2dd &pp)
    {
        return reverse(pp).normalised() - p.normalised();
    }
    static Matrix44 RayDiffNormalizerDiff(const double &ux, const double &uy, const double &uz);


    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd result;
        result[1] = (p[1] - principal[1]) / focal[1];
        result[0] = (p[0] - skew * result[1] - principal[0]) / focal[0];
        return Vector3dd(result.x(), result.y(), 1.0);
    }

    virtual bool isVisible(const Vector3dd &p) const override
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

    // XXX: Matrix44 'cause corecvs cannot give me a 2x3 matrix
    static Matrix44 NormalizerDiff(const double &x, const double &y, const double &z);

    Matrix44 getKMatrix() const;
    Matrix44 getInvKMatrix() const;

    Matrix33 getKMatrix33() const;
    Matrix33 getInvKMatrix33() const;

    /**
     * This matrix variant also has non-trivial Z output in the style used in OpenGL
     * This method also returns the matrix that is projecting to -1..1 interval, instead of this->size()
     **/
    Matrix44 getFrustumMatrix(double zNear = -1, double zFar = -1) const;


    // Here we will call K^{-1} Ki, the postfix is diff. var
    // All functions return 4x4 matrices because corecvs does not support matrices of fixed arbitrary size
    static Matrix44 Kf();
    static Matrix44 Kif(const double &f, const double &cx, const double &cy);
    static Matrix44 Kfx();
    static Matrix44 Kfy();
    static Matrix44 Kifx(const double &fx, const double &cx);
    static Matrix44 Kify(const double &fy, const double &cy);
    static Matrix44 Kcx();
    static Matrix44 Kicx(const double &fx);
    static Matrix44 Kcy();
    static Matrix44 Kicy(const double &fy);
    static Matrix44 Ks() { return Matrix44(0.0, 1.0, 0.0, 0.0,
                                           0.0, 0.0, 0.0, 0.0,
                                           0.0, 0.0, 0.0, 0.0,
                                           0.0, 0.0, 0.0, 0.0); }
    static Matrix44 Kis(const double &fx, const double &fy, const double &cy) {
                           return Matrix44(0.0, -1.0/fx/fy, cy/fx/fy, 0.0,
                                           0.0,        0.0,       0.0, 0.0,
                                           0.0,        0.0,       0.0, 0.0,
                                           0.0,        0.0,       0.0, 0.0); }

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


template<typename DoubleType>
class GenericPinholeCameraIntrinsics {
public:
    Vector2d<DoubleType> focal;            /**< Focal length (in px) in two directions */
    Vector2d<DoubleType> principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    DoubleType   skew;                     /**< Skew parameter to compensate for optical axis tilt */

    GenericPinholeCameraIntrinsics(const PinholeCameraIntrinsics &data)
    {
        focal     = Vector2d<DoubleType>(DoubleType(data.focal.x()), DoubleType(data.focal.y()));
        principal = Vector2d<DoubleType>(DoubleType(data.principal.x()), DoubleType(data.principal.y()));
        skew      = DoubleType(data.skew);
    }

    Vector2d<DoubleType> project(const Vector3d<DoubleType> &p) const
    {
        Vector2d<DoubleType> result = (focal * p.xy() + Vector2d<DoubleType>(skew * p.y(), DoubleType(0.0))) / p.z() + principal;
        return result;
    }

};


} // namespace corecvs

#endif // PINHOLECAMERAINTRINSICS_H
