#ifndef PINHOLECAMERAINTRINSICS_H
#define PINHOLECAMERAINTRINSICS_H

#include "core/math/matrix/matrix44.h"
#include "core/cameracalibration/projection/projectionModels.h"
#include "core/xml/generated/pinholeCameraIntrinsicsBaseParameters.h"
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
 *
 **/

struct PinholeCameraIntrinsics : public PinholeCameraIntrinsicsBaseParameters,  public CameraProjection
{

    const static int DEFAULT_SIZE_X = 2592;
    const static int DEFAULT_SIZE_Y = 1944;

    Vector2dd offset; /**< this added for indoors project. I'm not sure it should be in core */

    PinholeCameraIntrinsics(
            double fx = 1.0,
            double fy = 1.0,
            double cx = DEFAULT_SIZE_X / 2.0,
            double cy = DEFAULT_SIZE_Y / 2.0,
            double skew = 0.0,
            Vector2dd size = Vector2dd(DEFAULT_SIZE_X, DEFAULT_SIZE_Y),
            Vector2dd distortedSize = Vector2dd(DEFAULT_SIZE_X, DEFAULT_SIZE_Y))
      : PinholeCameraIntrinsicsBaseParameters(fx, fy, cx, cy, skew, Vector2dParameters(size), Vector2dParameters(distortedSize)),
        CameraProjection(ProjectionType::PINHOLE)
    {}

    PinholeCameraIntrinsics(const Vector2dd &resolution, double hfov);

    PinholeCameraIntrinsics(Vector2dd resolution, Vector2dd principal, double focal, double skew = 0.0) :
        PinholeCameraIntrinsicsBaseParameters(
            focal, focal, principal.x(), principal.y(), skew
          , Vector2dParameters(resolution), Vector2dParameters(resolution)),
        CameraProjection(ProjectionType::PINHOLE)
    {
    }

    /**
     * This actually doesn't differ from matrix multiplication, just is a little bit more lightweight
     *
     **/
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        Vector2dd result = (focal() * p.xy() + Vector2dd(skew() * p.y(), 0.0)) / p.z() + principal();
        return result;
    }

    static Matrix44 RayDiffNormalizerDiff(const double &ux, const double &uy, const double &uz);

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd result;
        result.y() = (p.y() - cy()) / fy();
        result.x() = (p.x() - skew() * result.y() - cx()) / fx();
        return Vector3dd(result.x(), result.y(), 1.0);
    }

    virtual bool isVisible(const Vector3dd &p) const override
    {
        if (p.z() <= 0.0) {
            return false;
        }
        Vector2dd proj = project(p);

        if (!proj.isInRect(Vector2dd(0.0, 0.0), size()))
        {
            return false;
        }
        return true;
    }

    /** Needed for indoors **/
    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            PinholeCameraIntrinsicsBaseParameters::accept<VisitorType>(visitor);
            visitor.visit(offset.x(), 0.0, "offsetX");
            visitor.visit(offset.y(), 0.0, "offsetY");

        }

    /**
     * Returns target image size
     **/
    virtual Vector2dd size() const override
    {
        return Vector2dd(PinholeCameraIntrinsicsBaseParameters::size());
    }

    virtual Vector2dd distortedSize() const override
    {
        return  Vector2dd(PinholeCameraIntrinsicsBaseParameters::distortedSize());
    }

    virtual Vector2dd principal() const override
    {
        return  Vector2dd(cx(), cy());
    }

    Vector2dd focal() const
    {
        return  Vector2dd(fx(), fy());
    }

    void setSize(const Vector2dd &size)
    {
        mSize.mX = size.x();
        mSize.mY = size.y();
    }

    void setDistortedSize(const Vector2dd &size)
    {
        mDistortedSize.mX = size.x();
        mDistortedSize.mY = size.y();
    }

    void setFocal(const Vector2dd &focal)
    {
        mFx = focal.x();
        mFy = focal.y();
    }

    void setPrincipal(const Vector2dd &principal)
    {
        mCx = principal.x();
        mCy = principal.y();
    }

    void setPrincipal(int id, const double &principalDim)
    {
        if (id == 0) { mCx = principalDim; return; }
        if (id == 1) { mCy = principalDim; return; }
    }

    explicit operator Matrix33() const  { return getKMatrix33(); }

    // XXX: Matrix44 'cause corecvs cannot give me a 2x3 matrix
    static Matrix44 NormalizerDiff(const double &x, const double &y, const double &z);

    Matrix44 getKMatrix() const;
    Matrix44 getInvKMatrix() const;

    Matrix33 getKMatrix33() const;
    Matrix33 getInvKMatrix33() const;

    /**
     **/
    static PinholeCameraIntrinsics FromKMatix(double w, double h, const Matrix44 &K);

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

    /* Due to historical reasons we name fields in the file in non-generatable manner */
#if 0
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
#endif

    void scale(double scaleFactor)
    {
        mCx *= scaleFactor;
        mCy *= scaleFactor;

        mFx *= scaleFactor;
        mFy *= scaleFactor;

        mSize.mX *= scaleFactor;
        mSize.mY *= scaleFactor;

        mDistortedSize.mX *= scaleFactor;
        mDistortedSize.mY *= scaleFactor;
    }

    /* Helper pseudonim getters */
    /*double cx() const    { return principalX(); }
    double cy() const    { return principalY(); }
    double fx() const    { return focalX(); }
    double fy() const    { return focalY(); }*/

    /* Misc */
    virtual PinholeCameraIntrinsics *clone() const
    {
        PinholeCameraIntrinsics *p = new PinholeCameraIntrinsics();
        *p = *this;
        return p;
    }

    virtual DynamicObjectWrapper getDynamicWrapper() override
    {
        return DynamicObjectWrapper(getReflection(), static_cast<PinholeCameraIntrinsicsBaseParameters *>(this));
    }


    virtual ~PinholeCameraIntrinsics() {}


};


template<typename DoubleType>
class GenericPinholeCameraIntrinsics {
public:
    Vector2d<DoubleType> focal;            /**< Focal length (in px) in two directions */
    Vector2d<DoubleType> principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    DoubleType   skew;                     /**< Skew parameter to compensate for optical axis tilt */

    GenericPinholeCameraIntrinsics(const PinholeCameraIntrinsics &data)
    {
        focal     = Vector2d<DoubleType>(DoubleType(data.fx()), DoubleType(data.fy()));
        principal = Vector2d<DoubleType>(DoubleType(data.cx()), DoubleType(data.cy()));
        skew      = DoubleType(data.skew());
    }

    Vector2d<DoubleType> project(const Vector3d<DoubleType> &p) const
    {
        Vector2d<DoubleType> result = (focal * p.xy() + Vector2d<DoubleType>(skew * p.y(), DoubleType(0.0))) / p.z() + principal;
        return result;
    }

};


} // namespace corecvs

#endif // PINHOLECAMERAINTRINSICS_H
