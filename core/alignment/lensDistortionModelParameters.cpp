/**
 * \file lensDistortionModelParameters.cpp
 * \attention This file was automatically generated and now we modify it manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */
#include <vector>
#include <stddef.h>

#include "core/alignment/lensDistortionModelParameters.h"
#include "core/polynomial/polynomialSolver.h"

/**
 *  Looks extremely unsafe because it depends on the order of static initialization.
 *  Should check standard if this is ok
 *
 *  Also it's not clear why removing "= Reflection()" breaks the code;
 **/
namespace corecvs {
#if 0
template<>
Reflection BaseReflection<LensDistortionModelParameters>::reflection = Reflection();
template<>
int BaseReflection<LensDistortionModelParameters>::dummy = LensDistortionModelParameters::staticInit();
#endif
} // namespace corecvs

SUPPRESS_OFFSET_WARNING_BEGIN

int LensDistortionModelParameters::staticInit(corecvs::Reflection *toFill)
{
    return LensDistortionModelParametersBase::staticInit(toFill);
}

SUPPRESS_OFFSET_WARNING_END

void LensDistortionModelParameters::getInscribedImageRect(const Vector2dd &tlDistorted, const Vector2dd &drDistorted, Vector2dd &tlUndistorted, Vector2dd &drUndistorted) const
{
    std::vector<corecvs::Vector2dd> boundaries[4];
    getRectMap(tlDistorted, drDistorted, boundaries);

    tlUndistorted = mapBackward(tlDistorted);
    drUndistorted = mapBackward(drDistorted);

    for (auto& v: boundaries[0])
        if (v[1] > tlUndistorted[1])
            tlUndistorted[1] = v[1];
    for (auto& v: boundaries[2])
        if (v[1] < drUndistorted[1])
            drUndistorted[1] = v[1];
    for (auto& v: boundaries[1])
        if (v[0] > tlUndistorted[0])
            tlUndistorted[0] = v[0];
    for (auto& v: boundaries[3])
        if (v[0] < drUndistorted[0])
            drUndistorted[0] = v[0];
}

void LensDistortionModelParameters::getCircumscribedImageRect(const Vector2dd &tlDistorted, const Vector2dd &drDistorted, Vector2dd &tlUndistorted, Vector2dd &drUndistorted) const
{
    std::vector<corecvs::Vector2dd> boundaries[4];
    getRectMap(tlDistorted, drDistorted, boundaries);

    tlUndistorted = mapBackward(tlDistorted);
    drUndistorted = mapBackward(drDistorted);

    for (auto& v: boundaries[0])
        if (v[1] < tlUndistorted[1])
            tlUndistorted[1] = v[1];
    for (auto& v: boundaries[2])
        if (v[1] > drUndistorted[1])
            drUndistorted[1] = v[1];
    for (auto& v: boundaries[1])
        if (v[0] < tlUndistorted[0])
            tlUndistorted[0] = v[0];
    for (auto& v: boundaries[3])
        if (v[0] > drUndistorted[0])
            drUndistorted[0] = v[0];
}

bool LensDistortionModelParameters::check(const double r)
{
    auto rn = r / mNormalizingFocal;
    std::vector<double> coeff;
    int n = (int)mKoeff.size();
    coeff.resize(n + 1);
    coeff[0] = 1.0;
    for (int i = 1; i <= n; ++i)
        coeff[i] = (i + 1) * mKoeff[i - 1];

    Polynomial p(coeff);
    std::vector<double> roots;
    PolynomialSolver::solve(p, roots);

    std::cout << "LDMP roots: ";
    for (auto& r : roots)
    {
        if (r > 0.0)
            std::cout << r * mNormalizingFocal << " ";
        if (r > 0.0 && r < rn)
            return false;
    }

    return (radialScaleNormalized(rn / 2.0) > 0) ^ (radialScaleNormalized(rn) < 0);
}

bool LensDistortionModelParameters::check(const Vector2dd &tl, const Vector2dd &br)
{
    double rlimit = 0;
    Vector2dd vv[] = { tl, br };
    Vector2dd center(mPrincipalX, mPrincipalY);
    Vector2dd shift(mShiftX, mShiftY);
    for (int i = 0; i < 4; ++i)
    {
        Vector2dd v(vv[i / 2][0], vv[i % 2][1]);
        if (mMapForward)
        {
            v -= shift;
            v *= 1.0 / mScale;
        }
        v -= center;
        v[0] *= mAspect;
        double r = v[0] * v[0] + v[1] * v[1];
        if (r > rlimit)
            rlimit = r;
    }
    return check(std::sqrt(rlimit));
}

Matrix22 LensDistortionModelParameters::jacobian(double x, double y) const
{
    auto dT1 = mMapForward ? Matrix22(1.0 / mScale,          0.0,
                                               0.0, 1.0 / mScale) :
                             Matrix22(1.0, 0.0,
                                      0.0, 1.0);
    auto dT2 = Matrix22(1.0 / mNormalizingFocal * mAspect,                     0.0,
                                                      0.0, 1.0 / mNormalizingFocal);
    auto T1 = mMapForward ? Matrix33(1.0 / mScale,          0.0, -mShiftX / mScale,
                                              0.0, 1.0 / mScale, -mShiftY / mScale,
                                              0.0,          0.0,               1.0) :
                            Matrix33(1.0, 0.0, 0.0,
                                     0.0, 1.0, 0.0,
                                     0.0, 0.0, 1.0);
    auto T2 = Matrix33(1.0 / mNormalizingFocal * mAspect,                     0.0, -mPrincipalX / mNormalizingFocal * mAspect,
                                                     0.0, 1.0 / mNormalizingFocal, -mPrincipalY / mNormalizingFocal,
                                                     0.0,                     0.0,                              1.0);
    Vector3dd xy(x, y, 1.0);
    auto dxdyv = T2 * (T1 * xy);
    auto dx = dxdyv[0], dy = dxdyv[1];
    auto r2 = dx * dx + dy * dy;
    auto r  = std::sqrt(r2);
    auto drddx = dx / r, drddy = dy / r;

    auto dphidr = 0.0;
    auto phi = 0.0;
    auto rpow = 1.0;
    for (size_t i = 0; i < mKoeff.size(); i++)
    {
        dphidr += mKoeff[i] * (i + 1) * rpow;
        rpow *= r;
        phi += mKoeff[i] * rpow;
    }

    auto dPhi = Matrix22(phi + dphidr*drddx,     dx*dphidr*drddy,
                            dy*dphidr*drddx,  phi + dphidr*drddy);
    auto dPsi = Matrix22(2.0 * mTangentialX*dy + 6.0 * mTangentialY * dx, 2.0 * mTangentialX * dx + 2.0 * mTangentialY*dy,
                         2.0 * mTangentialX*dx + 2.0 * mTangentialY * dy, 6.0 * mTangentialX * dy + 2.0 * mTangentialY*dx);
    auto ddiff= Matrix22::Identity();
    auto dP1 = Matrix22(mNormalizingFocal / mAspect,               0.0,
                                                0.0, mNormalizingFocal);
    auto dP2 = mMapForward ? Matrix22::Identity() :
                             Matrix22::Scale2(mScale);

    return dP2 * dP1 * (ddiff + dPhi + dPsi) * dT2 * dT1;
}

Matrix22 LensDistortionModelParameters::principalJacobian(double x, double y) const
{
    /* Unused
    auto dT1 = mMapForward ? Matrix22(1.0 / mScale) : Matrix22::Identity();
    auto dT2 = Matrix22(1.0 / mNormalizingFocal);
    */

    auto T1 = mMapForward ? Matrix33(1.0 / mScale,          0.0, -mShiftX / mScale,
                                                0.0, 1.0 / mScale, -mShiftY / mScale,
                                                0.0,          0.0,               1.0) :
                            Matrix33(1.0, 0.0, 0.0,
                                        0.0, 1.0, 0.0,
                                        0.0, 0.0, 1.0);
    auto T2 = Matrix33(1.0 / mNormalizingFocal * mAspect,                     0.0, -mPrincipalX / mNormalizingFocal * mAspect,
                                                        0.0, 1.0 / mNormalizingFocal, -mPrincipalY / mNormalizingFocal,
                                                        0.0,                     0.0,                              1.0);
    Vector3dd xy(x, y, 1.0);
    auto dxdyv = T2 * (T1 * xy);
    auto dx = dxdyv[0], dy = dxdyv[1];
    auto r2 = dx * dx + dy * dy;
    auto r  = std::sqrt(r2);
    auto drddx = dx / r, drddy = dy / r;

    auto dphidr = 0.0;
    auto phi = 0.0;
    auto rpow = 1.0;
    for (size_t i = 0; i < mKoeff.size(); i++)
    {
        dphidr += mKoeff[i] * (i + 1) * rpow;
        rpow *= r;
        phi += mKoeff[i] * rpow;
    }

    auto dPhi = Matrix22(phi + dphidr*drddx,     dx*dphidr*drddy,
                            dy*dphidr*drddx,  phi + dphidr*drddy);
    auto dPsi = Matrix22(2.0*mTangentialX*dy+6.0* mTangentialY*dx, 2.0*mTangentialX*dx+2.0*mTangentialY*dy,
                            2.0*mTangentialX*dx+2.0*mTangentialY*dy, 6.0*mTangentialX*dy+2.0*mTangentialY*dx);
    auto ddiff= Matrix22(1.0, 0.0,
                            0.0, 1.0);
    auto dP1 = Matrix22(mNormalizingFocal / mAspect,               0.0,
                                                0.0, mNormalizingFocal);
    auto dP2 = mMapForward ? Matrix22(1.0, 0.0,
                                        0.0, 1.0) :
                            Matrix22(mScale,    0.0,
                                        0.0, mScale);
    auto P3 = Matrix22(1, 0, 0, 1);
    auto P0 = Matrix22(-1.0/mNormalizingFocal*mAspect, 0.0, 0.0, -1.0/mNormalizingFocal);
    return P3 + dP2 * dP1 * (ddiff + dPhi + dPsi) * P0;
}

Matrix22 LensDistortionModelParameters::tangentialJacobian(double x, double y) const
{
    /* Unused
    auto dT1 = mMapForward ? Matrix22(1.0 / mScale) : Matrix22::Identity();
    auto dT2 = Matrix22(1.0 / mNormalizingFocal * mAspect);
    */

    auto T1 = mMapForward ? Matrix33(1.0 / mScale,          0.0, -mShiftX / mScale,
                                                0.0, 1.0 / mScale, -mShiftY / mScale,
                                                0.0,          0.0,               1.0) :
                            Matrix33(1.0, 0.0, 0.0,
                                        0.0, 1.0, 0.0,
                                        0.0, 0.0, 1.0);
    auto T2 = Matrix33(1.0 / mNormalizingFocal * mAspect,                     0.0, -mPrincipalX / mNormalizingFocal * mAspect,
                                                        0.0, 1.0 / mNormalizingFocal, -mPrincipalY / mNormalizingFocal,
                                                        0.0,                     0.0,                              1.0);
    Vector3dd xy(x, y, 1.0);
    auto dxdyv = T2 * (T1 * xy);
    auto dx = dxdyv[0], dy = dxdyv[1];
    auto r2 = dx * dx + dy * dy;
    auto r  = std::sqrt(r2);
    // auto drddx = dx / r; /* unused*/
    // auto drddy = dy / r; /* unused*/

    auto dphidr = 0.0;
    auto phi = 0.0;
    auto rpow = 1.0;
    for (size_t i = 0; i < mKoeff.size(); i++)
    {
        dphidr += mKoeff[i] * (i + 1) * rpow;
        rpow *= r;
        phi += mKoeff[i] * rpow;
    }

    auto dPsi = Matrix22(2.0*dx*dy,      r2 + 2.0*dx*dx,
                            r2 + 2.0*dy*dy, 2.0*dx*dy);
    auto dP1 = Matrix22(mNormalizingFocal / mAspect,               0.0,
                                                0.0, mNormalizingFocal);
    auto dP2 = mMapForward ? Matrix22(1.0, 0.0,
                                        0.0, 1.0) :
                            Matrix22(mScale,    0.0,
                                        0.0, mScale);

    return dP2 * dP1 * dPsi;
}

Matrix   LensDistortionModelParameters::polynomialJacobian(double x, double y) const
{
    /* Unused
    auto dT1 = mMapForward ? Matrix22::Scale2(1.0 / mScale) : Matrix22::Identity();
    auto dT2 = Matrix22(1.0 / mNormalizingFocal * mAspect);
    */

    auto T1 = mMapForward ? Matrix33(1.0 / mScale,          0.0, -mShiftX / mScale,
                                                0.0, 1.0 / mScale, -mShiftY / mScale,
                                                0.0,          0.0,               1.0) :
                            Matrix33(1.0, 0.0, 0.0,
                                        0.0, 1.0, 0.0,
                                        0.0, 0.0, 1.0);
    auto T2 = Matrix33(1.0 / mNormalizingFocal * mAspect,                     0.0, -mPrincipalX / mNormalizingFocal * mAspect,
                                                        0.0, 1.0 / mNormalizingFocal, -mPrincipalY / mNormalizingFocal,
                                                        0.0,                     0.0,                              1.0);
    Vector3dd xy(x, y, 1.0);
    auto dxdyv = T2 * (T1 * xy);
    auto dx = dxdyv[0], dy = dxdyv[1];
    auto r2 = dx * dx + dy * dy;
    auto r  = std::sqrt(r2);
    // auto drddx = dx / r;  /*unused*/
    // auto drddy = dy / r;  /*unused*/

    auto rpow = 1.0;
    Matrix dPhi(2, (int32_t)mKoeff.size());
    for (int i = 0; i < (int)mKoeff.size(); ++i)
    {
        rpow *= r;
        dPhi.a(0, i) = dx * rpow;
        dPhi.a(1, i) = dy * rpow;
    }

    /* Unused
    auto dPsi = Matrix22(2.0*mTangentialX*dy+6.0* mTangentialY*dx, 2.0*mTangentialX*dx+2.0*mTangentialY*dy,
                            2.0*mTangentialX*dx+2.0*mTangentialY*dy, 6.0*mTangentialX*dy+2.0*mTangentialY*dx);
    auto ddiff= Matrix22::Identity();
    */

    auto dP1 = Matrix22(mNormalizingFocal / mAspect,               0.0,
                                                0.0, mNormalizingFocal);
    auto dP2 = mMapForward ? Matrix22(1.0, 0.0,
                                        0.0, 1.0) :
                            Matrix22(mScale,    0.0,
                                        0.0, mScale);
    // corecvs still sucks when this code is being written
    Matrix mmm(2, 2);
    auto barbar = dP2 * dP1;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            mmm.a(i, j) = barbar.a(i, j);
    return mmm * dPhi;
}


void LensDistortionModelParameters::solveRadial(const std::vector<Vector2dd> &src, const std::vector<Vector2dd> &dst)
{
    CORE_ASSERT_TRUE_S(src.size() == dst.size());
    // So we will compute this stuff in normalized coordinates
    int N = (int)src.size(), M = (int)mKoeff.size();
    Matrix D(2 * N, M + 2);
    Vector rhs(2 * N);
    std::vector<double> powers(M);
    for (int i = 0; i < N; ++i)
    {
        auto ux = src[i][0], uy = src[i][1],
             vx = dst[i][0], vy = dst[i][1];
        vx -= mPrincipalX;
        vy -= mPrincipalY;
        if (mMapForward)
        {
            ux -= mShiftX;
            ux /= mScale;
            uy -= mShiftY;
            uy /= mScale;
        }
        else
        {
            vx -= mShiftX;
            vy -= mShiftY;
            vx /= mScale;
            vy /= mScale;
        }
        vx /= mNormalizingFocal;
        vy /= mNormalizingFocal;

        ux = (ux - mPrincipalX) / mNormalizingFocal * mAspect;
        uy = (uy - mPrincipalY) / mNormalizingFocal;

        auto r = std::sqrt(ux * ux + uy * uy);
        auto dx = vx - ux, dy = vy - uy;

        powers[0] = r;
        for (int j = 1; j < M; ++j)
            powers[j] = powers[j - 1] * r;

        for (int j = 0; j < M; ++j)
        {
            D.a(i * 2,     j) = ux * powers[j] / mAspect;
            D.a(i * 2 + 1, j) = uy * powers[j];
        }
        D.a(i * 2,     M) = 2 * ux * uy / mAspect;
        D.a(i * 2 + 1, M) = ux * ux + 3 * uy * uy;
        D.a(i * 2,     M + 1) = (3 * ux * ux + uy * uy) / mAspect;
        D.a(i * 2 + 1, M + 1) = 2 * ux * uy;
        rhs[i * 2    ] = dx;
        rhs[i * 2 + 1] = dy;

    }

    Vector res(M + 2);
#ifndef WITH_BLAS
    D.ata().linSolve(rhs*D, res, true, true);
#else
    LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', N, M + 2, 1, &D.a(0, 0), D.stride, &rhs[0], 1);
    res = rhs;
#endif

    for (int i = 0; i < M; ++i)
        mKoeff[i] = res[i];
    mTangentialX = res[M];
    mTangentialY = res[M + 1];
}

void LensDistortionModelParameters::getRectMap(const Vector2dd &tl, const Vector2dd &dr, std::vector<Vector2dd> boundaries[4]) const
{
    Vector2dd shifts[] =
    {
        Vector2dd(1, 0),
        Vector2dd(0, 1),
        Vector2dd(1, 0),
        Vector2dd(0, 1)
    };
    Vector2dd origins[] =
    {
        tl,
        tl,
        Vector2dd(tl[0], dr[1]),
        Vector2dd(dr[0], tl[1])
    };
    int steps[] =
    {
        (int)std::ceil(dr[0] - tl[0]) + 1,
        (int)std::ceil(dr[1] - tl[1]) + 1,
        (int)std::ceil(dr[0] - tl[0]) + 1,
        (int)std::ceil(dr[1] - tl[1]) + 1
    };

    for (int i = 0; i < 4; ++i)
    {
        boundaries[i].resize(steps[i]);
        auto& boundary = boundaries[i];
        auto& shift    = shifts[i];
        auto& origin   = origins[i];
        auto& step     = steps[i];
        corecvs::parallelable_for(0, step, [&](const corecvs::BlockedRange<int> &r)
                {
                    for (int j = r.begin(); j != r.end(); ++j)
                        boundary[j] = mapBackward(origin + j * shift);
                });
    }
}
