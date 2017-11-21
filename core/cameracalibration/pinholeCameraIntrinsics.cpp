#include "core/cameracalibration/pinholeCameraIntrinsics.h"

using namespace std;

namespace corecvs {


Matrix44 PinholeCameraIntrinsics::Kf()
{
    return Matrix44(1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kfx()
{
    return Matrix44(1.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kfy()
{
    return Matrix44(0.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kcx()
{
    return Matrix44(0.0, 0.0, 1.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kcy()
{
    return Matrix44(0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kif(const double &f, const double &cx, const double &cy)
{
    auto f2 = f * f;
    auto f21 = 1.0 / f2;
    return Matrix44(-f21, 0.0, cx / f2, 0.0,
                     0.0,-f21, cy / f2, 0.0,
                     0.0, 0.0,     0.0, 0.0,
                     0.0, 0.0,     0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kifx(const double &fx, const double &cx)
{
    auto f2 = fx * fx;
    auto f21 = 1.0 / f2;
    return Matrix44(-f21, 0.0, cx / f2, 0.0,
                     0.0, 0.0,     0.0, 0.0,
                     0.0, 0.0,     0.0, 0.0,
                     0.0, 0.0,     0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kify(const double &fy, const double &cy)
{
    auto f2 = fy * fy;
    auto f21 = 1.0 / f2;
    return Matrix44(0.0, 0.0,     0.0, 0.0,
                    0.0,-f21, cy / f2, 0.0,
                    0.0, 0.0,     0.0, 0.0,
                    0.0, 0.0,     0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kicx(const double &fx)
{
    return Matrix44(0.0, 0.0, -1.0 / fx, 0.0,
                    0.0, 0.0,       0.0, 0.0,
                    0.0, 0.0,       0.0, 0.0,
                    0.0, 0.0,       0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::Kicy(const double &fy)
{
    return Matrix44(0.0, 0.0,       0.0, 0.0,
                    0.0, 0.0, -1.0 / fy, 0.0,
                    0.0, 0.0,       0.0, 0.0,
                    0.0, 0.0,       0.0, 0.0);
}



Matrix44 PinholeCameraIntrinsics::NormalizerDiff(const double &x, const double &y, const double &z)
{
    double zz = z * z;
    return Matrix44(1.0 / z,     0.0, -x / zz, 0.0,
                        0.0, 1.0 / z, -y / zz, 0.0,
                        0.0,     0.0,     0.0, 0.0,
                        0.0,     0.0,     0.0, 0.0);
}

Matrix44 PinholeCameraIntrinsics::RayDiffNormalizerDiff(const double &ux, const double &uy, const double &uz)
{
    auto ux2 = ux*ux, uy2 = uy * uy, uz2 = uz * uz,
         uxuy= ux*uy, uxuz= ux * uz, uyuz= uy * uz;
    auto N = ux2 + uy2 + uz2;
    auto N2 = std::sqrt(N);
    auto N32 = N * N2, N21 = 1.0 / N2;
    return Matrix44   (
            -ux2/N32+N21,    -uxuy/N32,    -uxuz/N32, 0.0,
               -uxuy/N32, -uy2/N32+N21,    -uyuz/N32, 0.0,
               -uxuz/N32,    -uyuz/N32, -uz2/N32+N21, 0.0,
                     0.0,          0.0,          0.0, 0.0);
}


/*
    return Matrix44 (
         f / aspect, 0 ,             0           ,            0            ,
         0         ,-f ,             0           ,            0            ,
         0         , 0 , -(zFar + zNear) / zDepth, 2 *zFar * zNear / zDepth,
         0         , 0 ,             1           ,            0
    );

*/

Matrix44 PinholeCameraIntrinsics::getFrustumMatrix(double zNear, double zFar) const
{
    if (zNear == -1) {
        zNear = 1.0;
    }

    if (zFar == -1) {
        zFar = 1000.0;
    }

    double zDepth  = zNear - zFar;

    Matrix44 KF =  Matrix44 (
        focal.x(),   skew   ,          0.0            ,    /*principal.x()*/ 0,
           0.0   ,-focal.y(),          0.0            ,    /*principal.y()*/ 0,
           0.0   ,    0.0   , -(zFar + zNear) / zDepth, 2 * zFar * zNear / zDepth,
           0.0   ,    0.0   ,          1.0            ,        0.0
    );

    Matrix44 D = /*Matrix44::Shift(0.5, 0.5, 0.0) **/ Matrix44::Diagonal(2.0 / size.x(), 2.0 / size.y(), 1.0, 1.0);

    cout << "K Matrix:" << endl;
    cout <<  KF;

    cout << "D Matrix:" << endl;
    cout <<  D;

    Matrix44 toReturn = D * KF;

    cout << "Result Matrix:" << endl;
    cout <<  toReturn;

    return  toReturn;
}


} // namespace corecvs
