/**
 * \file cameraParameters.cpp
 * \brief Add Comment Here
 *
 * \date Feb 16, 2011
 * \author alexander
 */

#include "cameraParameters.h"

namespace corecvs {

/**
 *  We will denote it by matrix \f$K\f$.
 *
 *  \f[K =  \pmatrix{
 *       f \over k &       0    & I_x & 0 \cr
 *           0     &  f \over k & I_y & 0 \cr
 *           0     &       0    & 1   & 0 \cr
 *           0     &       0    & 0   & 1  }
 * \f]
 *
 *
 * This matrix transform the 3D points form camera coordinate system to
 * image coordinate system
 **/
Matrix44 CameraIntrinsics::getKMatrix() const
{
    double focal = f / k;
    return Matrix44 (
        focal,   0.0, center.x(), 0.0,
        0.0,   focal, center.y(), 0.0,
        0.0,     0.0,    1.0,     0.0,
        0.0,     0.0,    0.0,     1.0
    );
}
 
/**
 *  Returns inverse of K matrix.
 *  For K matrix read CameraIntrinsics::getKMatrix()
 **/
Matrix44 CameraIntrinsics::getInvKMatrix() const
{
    double invFocal = k / f;
    return Matrix44 (
       invFocal,  0.0,    -center.x() * invFocal, 0.0,
          0.0,  invFocal, -center.y() * invFocal, 0.0,
          0.0,    0.0,               1.0,         0.0,
          0.0,    0.0,               0.0,         1.0
    );

}

Matrix33 CameraIntrinsics::getKMatrix33() const
{
    double focal = f / k;
    return Matrix33 (
       focal,   0.0, center.x(),
       0.0,   focal, center.y(),
       0.0,     0.0,    1.0
    );
}


Matrix33 CameraIntrinsics::getInvKMatrix33() const
{
    double invFocal = k / f;
    return Matrix33 (
       invFocal,  0.0,    -center.x() * invFocal,
          0.0,  invFocal, -center.y() * invFocal,
          0.0,    0.0,               1.0
    );
}

double CameraIntrinsics::getVFov() const
{
    double focal = f / k;
    return atan((resolution.y() / 2.0) / focal) * 2.0;
}

double CameraIntrinsics::getHFov() const
{
    double focal = f / k;
    return atan((resolution.x() / 2.0) / focal) * 2.0;
}

/*
Matrix44 CameraIntrinsics::getFrustumMatrix(double zNear, double zFar)  const
{
    const double focal = f / k;
    const double zDepth = zNear - zFar;
    const double aspect = resolution.y() / resolution.x();
    return Matrix44 (
        focal ,       0       ,             0           ,            0            ,
        0     ,-focal * aspect,             0           ,            0            ,
        0     ,       0       , -(zFar + zNear) / zDepth, 2 *zFar * zNear / zDepth,
        0     ,       0       ,             1           ,            0
    );
}*/

} //namespace corecvs

