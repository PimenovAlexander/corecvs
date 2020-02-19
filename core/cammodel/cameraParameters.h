#ifndef CAMERA_PARAMETERS_H_
#define CAMERA_PARAMETERS_H_
/**
 * \file cameraParameters.h
 * \brief a header for cameraParameters.c
 *
 * \date Feb 10, 2011
 */
#include "utils/global.h"
#include "math/vector/vector2d.h"
#include "math/matrix/matrix44.h"

#include "cameracalibration/cameraModel.h"
#include "cameracalibration/projection/pinholeCameraIntrinsics.h"



namespace corecvs {

/**
 * This class describes the parameters of the ideal pin-hole camera
 * TODO:
 *  Should be a child of a DeformMap or something similar
 *
 *
 *  Camera intrinsics scale and shift the image.
 *
 *  THIS IS DEPRICATED.
 *
 ***/
class CameraIntrinsicsLegacy
{
public:
    Vector2dd resolution;   /**< Imager resolution (in pixel) */
    Vector2dd center;       /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double f;               /**< Focal length (in mm) */
    double k;               /**< Pixel size (in mm)   */

    CameraIntrinsicsLegacy() :
        resolution(0.0)
      , center(0.0)
      , f(1.0)
      , k(1.0)
    {}

    CameraIntrinsicsLegacy(
        Vector2dd _resolution,
        Vector2dd _center,
        double _f,
        double _k
    ) :
        resolution(_resolution)
      , center(_center)
      , f(_f)
      , k(_k)
    {}

   // Matrix44 getFrustumMatrix(double zNear = 1.0, double zFar = 1000.0)  const;

    Matrix44 getKMatrix()  const;
    Matrix44 getInvKMatrix()  const;

    Matrix33 getKMatrix33()  const;
    Matrix33 getInvKMatrix33()  const;

    double getVFov() const;
    double getHFov() const;

    PinholeCameraIntrinsics toPinholeCameraIntrinsics() const;

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(resolution, Vector2dd(0.0), "resolution");
        visitor.visit(center, Vector2dd(0.0), "center");
        visitor.visit(f, 1.0, "focalMM");
        visitor.visit(k, 1.0, "pixelMM");
    }

};

} //namespace corecvs

#endif // CAMERA_PARAMETERS_H_
