#ifndef TWOVIEWOPTIMALTRIANGULATION
#define TWOVIEWOPTIMALTRIANGULATION

#include <array>
#include <vector>
#include <functional>

#include "math/vector/vector3d.h"
#include "math/matrix/matrix33.h"
#include "math/matrix/matrix44.h"
#include "cameracalibration/cameraModel.h"

namespace corecvs {

struct TwoViewOptimalTriangulor
{
    TwoViewOptimalTriangulor(bool L2, const Matrix44 &P1, const Vector2dd &v1, const Matrix44 &P2, const Vector2dd &v2);
    TwoViewOptimalTriangulor(bool L2, const CameraModel &l, const Vector2dd &v1, const CameraModel &r, const Vector2dd &v2);

    Vector3dd triangulate();

private:
    Vector3dd   ftransform(const Vector2dd &l, const Vector2dd &r);

    double      solveT();
    double      solveTL2();
    double      solveTL1();
    std::array<Vector2dd, 2> solveVectors(double T);
    double      solvePoly(std::vector<double> &c, std::function<double(double)> f);
  //Vector3dd   solveX(const Vector3dd &l1, const Vector3dd &l2);

    void        transform(const Vector2dd &v, const Vector3dd &e, Matrix33 &fwd, Matrix33 &bwd);

    Matrix33    F0, T1, T1inv, T2, T2inv, F1;
    Matrix44    P1, P2;
    Vector2dd   v1, v2;
    bool        L2;
    double      f1, f2;

    Vector3dd   e1, e2;
};

} // namespace corecvs

#endif
