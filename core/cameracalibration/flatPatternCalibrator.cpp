#include "cameracalibration/flatPatternCalibrator.h"

corecvs::FlatPatternCalibrator::FlatPatternCalibrator(const CameraConstraints constraints, const PinholeCameraIntrinsics lockParams, const LineDistortionEstimatorParameters distortionEstimatorParams, const double lockFactor) : factor(lockFactor), K(0), N(0), absoluteConic(6), intrinsics(lockParams), lockParams(lockParams), distortionEstimationParams(distortionEstimatorParams), constraints(constraints), forceZeroSkew(!!(constraints & CameraConstraints::ZERO_SKEW))
{
    distortionParams.mMapForward = true;
}

void corecvs::FlatPatternCalibrator::addPattern(const ObservationList &patternPoints, const CameraLocationData &position)
{
    ++N;
    locationData.push_back(position);
    points.push_back(patternPoints);
    K += patternPoints.size();
    for (auto& pp: patternPoints)
    {
        CORE_ASSERT_TRUE_S(!std::isnan(pp.projection[0]));
        CORE_ASSERT_TRUE_S(!std::isnan(pp.projection[1]));
        CORE_ASSERT_TRUE_S(!std::isnan(pp.point[0]));
        CORE_ASSERT_TRUE_S(!std::isnan(pp.point[1]));
        CORE_ASSERT_TRUE_S(!std::isnan(pp.point[2]));
    }
}

struct PlaneFitFunctor : public FunctionArgs
{
    PlaneFitFunctor(const std::vector<std::pair<corecvs::Vector3dd, corecvs::Vector3dd>> &pts
        , const Affine3DQ &initial)
        : FunctionArgs(6, 3 * (int)pts.size()), initial(initial), pts(pts)
    {}

    void operator()(const double *in, double *out)
    {
        double qx = in[0], qy = in[1], qz = in[2],
               tx = in[3], ty = in[4], tz = in[5];
        auto R = corecvs::Quaternion::RotationalTransformation(qx, qy, qz, 0.0, corecvs::Quaternion::Parametrization::NON_EXCESSIVE, false);
        R.a(0, 3) = tx;
        R.a(1, 3) = ty;
        R.a(2, 3) = tz;
        int argout = 0;
        for (auto& pt: pts)
        {
            auto diff = pt.second - (R * (initial * pt.first));
            for (int i = 0; i < 3; ++i)
                out[argout++] = diff[i];
        }

        diff.rotor[0] = qx;
        diff.rotor[1] = qy;
        diff.rotor[2] = qz;
        diff.rotor[3] = 1.0;
        diff.rotor.normalise();
        diff.shift = corecvs::Vector3dd(tx, ty, tz);
    }

    Affine3DQ initial;
    Affine3DQ diff;
    const std::vector<std::pair<corecvs::Vector3dd, corecvs::Vector3dd>> &pts;
};


corecvs::Affine3DQ corecvs::FlatPatternCalibrator::TrafoToPlane(const std::vector<std::pair<corecvs::Vector3dd, corecvs::Vector3dd>> &pts)
{
    int N = (int)pts.size();
    corecvs::Matrix A(N, 3), B(N, 3);
    for (int i = 0; i < N; ++i)
    {
        A.a(i, 0) = pts[i].second[0];
        A.a(i, 1) = pts[i].second[1];
        A.a(i, 2) = 1.0;
        CORE_ASSERT_TRUE_S(pts[i].second[2] == 0.0);
        for (int j = 0; j < 3; ++j)
            B.a(i, j) = pts[i].first[j];
    }
#ifdef WITH_BLAS
    corecvs::Matrix res_(3, 3);
    LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', N, 3, 3, &A.a(0, 0), A.stride, &B.a(0, 0), B.stride);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            res_.a(j, i) = B.a(i, j);
#else
    auto ata = A.ata();
    auto ab  = A.t() * B;
    // corecvs does not have multiple rhs linsolve,
    // but linsolve without blas is stupid like shit,
    // so no bad things happen there
    auto res_ = (ata.inv() * ab).t();
#endif
    corecvs::Matrix33 res;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            res.a(i, j) = res_.a(i, j);

    for (auto& vv: pts)
    {
        auto tgt = vv.first;
        auto frm = vv.second;
        frm[2] = 1.0;
        std::cout << tgt - res * frm << std::endl;
    }
    corecvs::Vector3dd r1(res.a(0, 0), res.a(1, 0), res.a(2, 0)),
                       r2(res.a(0, 1), res.a(1, 1), res.a(2, 1)),
                        t(res.a(0, 2), res.a(1, 2), res.a(2, 2));
    std::cout << res << std::endl;

    r1 = r1.normalised();
    r2 = (r2 - r1 * (r1 & r2)).normalised();
    auto r3 = r1 ^ r2;

    corecvs::Matrix33 rotor(r1[0], r2[0], r3[0],
                            r1[1], r2[1], r3[1],
                            r1[2], r2[2], r3[2]);
    std::cout << rotor << rotor.det() << std::endl;

    for (auto& vv: pts)
    {
        auto tgt = vv.first;
        auto frm = vv.second;
        std::cout << tgt - (rotor * frm + t) << std::endl;
    }
    auto q1 = corecvs::Affine3DQ(corecvs::Quaternion::FromMatrix(rotor), t).inverted();
    corecvs::LevenbergMarquardt lm;
    PlaneFitFunctor pff(pts, q1);
    lm.f = &pff;
    std::vector<double> inputs(6), outputs(pts.size() * 3);
    lm.traceProgress = false;
    auto r1es = lm.fit(inputs, outputs);
    pff(&r1es[0], &outputs[0]);
    auto q2 = pff.diff * q1;
    return q2;
}

void corecvs::FlatPatternCalibrator::solve(bool runPresolver, bool runLM, int LMiterations)
{
    std::cout << "SOLVING FPC" << std::endl;
    if (runPresolver)
    {
        double bestError = std::numeric_limits<double>::max();
        std::vector<CameraLocationData> bestLocations;
        PinholeCameraIntrinsics bestIntrinsics;
        LensDistortionModelParameters bestDistortion;
        for (int i = 0; i < presolverIterations; ++i)
        {
            if (!solveInitialIntrinsics())
            {
                CORE_ASSERT_TRUE_S(i > 0);
                break;
            }
            solveInitialExtrinsics();

            //double projective;
            //projective = getRmseReprojectionError();

            double enforced, distorted;
            enforceParams();
            enforced = getRmseReprojectionError();

            if (std::isnan(enforced))
                break;

            if (enforced < bestError)
            {
                bestError = enforced;
                bestLocations = locationData;
                bestIntrinsics = intrinsics;
                bestDistortion = distortionParams;
                std::cout << "*" << std::flush;
            }

            if (!(constraints & CameraConstraints::UNLOCK_DISTORTION))
                break;

            if (!distortionEstimated)
                distortionParams.mNormalizingFocal = std::max(!intrinsics.principal(), 1.0);
            if (distortionEstimated)
            {
                solveInitialDistortion(false);

                distorted = getRmseReprojectionError();
                if (std::isnan(distorted))
                    break;

                if (distorted < bestError)
                {
                    bestError = distorted;
                    bestLocations = locationData;
                    bestIntrinsics = intrinsics;
                    bestDistortion = distortionParams;
                    std::cout << "@" << std::flush;
                }
            }


            solveInitialDistortion(true);

            distorted = getRmseReprojectionError();
            if (std::isnan(distorted))
                break;

            if (distorted < bestError)
            {
                bestError = distorted;
                bestLocations = locationData;
                bestIntrinsics = intrinsics;
                bestDistortion = distortionParams;
                std::cout << "$" << std::flush;
            }
        }
        CORE_ASSERT_TRUE_S(bestError != std::numeric_limits<double>::max());

        locationData = bestLocations;
        intrinsics = bestIntrinsics;
        distortionParams = bestDistortion;
        std::cout << std::endl;
    }

    if(runLM) refineGuess(LMiterations);
    std::cout << std::endl <<  this << "RES LM: " << getRmseReprojectionError() << std::endl;
    std::cout << "OPTFAC: " << factor << std::endl;
}

PinholeCameraIntrinsics corecvs::FlatPatternCalibrator::getIntrinsics()
{
    return intrinsics;
}

std::vector<CameraLocationData> corecvs::FlatPatternCalibrator::getExtrinsics()
{
    return locationData;
}

LensDistortionModelParameters corecvs::FlatPatternCalibrator::getDistortion()
{
    CORE_ASSERT_TRUE_S(!!(constraints & CameraConstraints::UNLOCK_DISTORTION));
    return distortionParams;
}

double corecvs::FlatPatternCalibrator::getRmseReprojectionError()
{
    std::vector<double> err(K * 2);
    getFullReprojectionError(&err[0]);

    double sqs = 0.0;
    for (auto& e : err)
        sqs += e * e;
    return sqrt(sqs / K);
}

void corecvs::FlatPatternCalibrator::solveInitialDistortion(bool enforcePrincipal)
{
    auto premap = getRmseReprojectionError();
    distortionParams.mMapForward = true;
    distortionParams.mKoeff.resize(distortionEstimationParams.mPolynomDegree);
    if (enforcePrincipal)
    {
        distortionParams.mPrincipalX = intrinsics.cx();
        distortionParams.mPrincipalY = intrinsics.cy();
    }

    std::vector<Vector2dd> proj, pt;
    for (size_t i = 0; i < N; i++)
    {
        auto R = Quaternion::RotationalTransformation(locationData[i].orientation[0], locationData[i].orientation[1], locationData[i].orientation[2], locationData[i].orientation[3], Quaternion::Parametrization::FULL_NORMALIZED, false);
        auto& C = locationData[i].position;

        for (auto& ptp: points[i])
        {
            Vector3dd pp = ptp.point;

            pp[1] *= factor;

            auto res = intrinsics.project(R * (pp - C));
            proj.push_back(res);
            pt.push_back(ptp.projection);
        }
    }
    distortionParams.solveRadial(proj, pt);
    auto postmap = getRmseReprojectionError();
    if (premap > postmap)
        std::cout << "^" << std::flush;
    distortionEstimated = true;
}

Matrix corecvs::FlatPatternCalibrator::getJacobian()
{
    Matrix J(getOutputNum(), getInputNum());
    int idx = 0;

//    auto f = intrinsics.focal[0], fx = intrinsics.focal[0], fy = intrinsics.focal[1];
//    auto cx= intrinsics.principal[0], cy = intrinsics.principal[1];
//    auto skew = intrinsics.skew;

    bool distorting = !!(constraints & CameraConstraints::UNLOCK_DISTORTION);
    for (size_t i = 0; i < N; ++i)
    {
        auto& QQ = locationData[i].orientation;
        auto qx = QQ[0], qy = QQ[1], qz = QQ[2], qw = QQ[3];
        auto& CC = locationData[i].position;
        auto tx = CC[0], ty = CC[1], tz = CC[2];
        Matrix44 R = Quaternion::RotationalTransformation(qx, qy, qz, qw, Quaternion::Parametrization::FULL_NORMALIZED, false), Rqx, Rqy, Rqz, Rqw,
                 Tc(1.0, 0.0, 0.0, -tx,
                    0.0, 1.0, 0.0, -ty,
                    0.0, 0.0, 1.0, -tz,
                    0.0, 0.0, 0.0, 1.0),
                 Kf = PinholeCameraIntrinsics::Kf(), Kfx = PinholeCameraIntrinsics::Kfx(), Kfy = PinholeCameraIntrinsics::Kfy(), Kcx = PinholeCameraIntrinsics::Kcx(), Kcy = PinholeCameraIntrinsics::Kcy(), Ks = PinholeCameraIntrinsics::Ks(),
                 K = intrinsics.getKMatrix();
        Quaternion::DiffTransformation(qx, qy, qz, qw, Quaternion::Parametrization::FULL_NORMALIZED, false, Rqx, Rqy, Rqz, Rqw);

        for (auto& ptp: points[i])
        {
            auto pp = ptp.point;
            auto pr = ptp.projection;

            auto    TX = Tc * Vector4dd(pp[0], pp[1], pp[2], 1.0);
            auto   RTX = R * TX;
            auto K2RTX = K * RTX;
            auto  KRTX = Vector2dd(K2RTX[0], K2RTX[1]) / K2RTX[2];
            Vector2dd DKRTX;
            Matrix22 DD = Matrix22::Identity(); /* Dead code should be elliminated after all */

            if (distorting)
            {
                DKRTX = distortionParams.mapForward(KRTX);
                DD = distortionParams.jacobian(KRTX[0], KRTX[1]);
            }
            auto PCIND = PinholeCameraIntrinsics::NormalizerDiff(K2RTX[0], K2RTX[1], K2RTX[2]);


            int argin = 0;
            if (!(constraints & CameraConstraints::LOCK_FOCAL))
            {
                if (!(constraints & CameraConstraints::EQUAL_FOCAL))
                {
                    auto dfx = Kfx * RTX;
                    auto dfy = Kfy * RTX;
                    auto ndfx = PCIND * dfx;
                    auto ndfy = PCIND * dfy;

                    Vector2dd _dfx(ndfx[0], ndfx[1]),
                              _dfy(ndfy[0], ndfy[1]);
                    if (distorting)
                    {
                        _dfx = DD * _dfx,
                        _dfy = DD * _dfy;
                    }

                    J.a(idx,         argin) = _dfx[0];
                    J.a(idx + 1,     argin) = _dfx[1];
                    J.a(idx,     argin + 1) = _dfy[0];
                    J.a(idx + 1, argin + 1) = _dfy[1];
                    argin += 2;
                }
                else
                {
                    auto df = Kf * RTX;
                    auto ndf = PCIND * df;

                    Vector2dd _df(ndf[0], ndf[1]);
                    if (distorting)
                        _df = DD * _df;
                    J.a(idx,         argin) = _df[0];
                    J.a(idx + 1,     argin) = _df[1];
                    argin++;
                }
            }

            if (!(constraints & CameraConstraints::LOCK_PRINCIPAL))
            {
                auto dcx = Kcx * RTX;
                auto dcy = Kcy * RTX;
                auto ndcx = PCIND * dcx;
                auto ndcy = PCIND * dcy;

                Vector2dd _dcx(ndcx[0], ndcx[1]),
                          _dcy(ndcy[0], ndcy[1]);
                if (distorting)
                {
                    _dcx = DD * _dcx,
                    _dcy = DD * _dcy;
                }

                J.a(idx,         argin) = _dcx[0];
                J.a(idx + 1,     argin) = _dcx[1];
                J.a(idx,     argin + 1) = _dcy[0];
                J.a(idx + 1, argin + 1) = _dcy[1];
                argin += 2;
            }

            if (!(constraints & CameraConstraints::LOCK_SKEW))
            {
                auto ds = Ks * RTX;
                auto nds = PCIND * ds;

                Vector2dd _ds(nds[0], nds[1]);
                if (distorting)
                    _ds = DD * _ds;

                J.a(idx,         argin) = _ds[0];
                J.a(idx + 1,     argin) = _ds[1];
                argin++;
            }

            if (!!(constraints & CameraConstraints::UNLOCK_YSCALE))
            {
                auto ds = K * R * Vector4dd(0, pr[1], 0, 0);
                auto nds = PCIND * ds;

                Vector2dd _ds(nds[0], nds[1]);
                if (distorting)
                    _ds = DD * _ds;

                J.a(idx,         argin) = _ds[0];
                J.a(idx + 1,     argin) = _ds[1];
                argin++;
            }

            // now we optimize 6-dof position
            int pos_argin = argin + (int)i * 7;
            auto dqx = K * Rqx * TX,
                 dqy = K * Rqy * TX,
                 dqz = K * Rqz * TX,
                 dqw = K * Rqw * TX,
                 dtx = K * R * Vector4dd( -1,  0,  0, 0),
                 dty = K * R * Vector4dd(  0, -1,  0, 0),
                 dtz = K * R * Vector4dd(  0,  0, -1, 0);
#define DDD(p) \
            auto nd ## p = PCIND * d ## p; \
            Vector2dd _d ## p(nd ## p[0], nd ## p[1]); \
            if (distorting) \
            _d ## p = DD * _d ## p; \
            J.a(idx,     pos_argin) = _d ## p[0]; \
            J.a(idx + 1, pos_argin) = _d ## p[1]; \
            ++pos_argin;

            DDD(tx)
            DDD(ty)
            DDD(tz)
            DDD(qx)
            DDD(qy)
            DDD(qz)
            DDD(qw)

            if (distorting)
            {
                auto principalJacobian = distortionParams.principalJacobian(KRTX[0], KRTX[1]),
                     tangentialJacobian = distortionParams.tangentialJacobian(KRTX[0], KRTX[1]);
                auto polynomialJacobian = distortionParams.polynomialJacobian(KRTX[0], KRTX[1]);



                int polyDeg = distortionEstimationParams.mPolynomDegree;
                int degStart = 0, degIncrement = 1;
                if (distortionEstimationParams.mEvenPowersOnly)
                    degIncrement = 2;


                int distortion_argin = argin + (int)N * 7;
                for (int k = degStart; k < polyDeg; k += degIncrement)
                {
                    for (int j = 0; j < 2; ++j)
                        J.a(idx + j, distortion_argin) = polynomialJacobian.a(j, k);
                    distortion_argin++;
                }

                if (distortionEstimationParams.mEstimateTangent)
                {
                    for (int j = 0; j < 2; ++j)
                        for (int k = 0; k < 2; ++k)
                            J.a(idx + k, distortion_argin + j) = tangentialJacobian.a(k, j);
                    distortion_argin += 2;
                }

                if (distortionEstimationParams.mEstimateCenter)
                {
                    for (int j = 0; j < 2; ++j)
                        for (int k = 0; k < 2; ++k)
                            J.a(idx + k, distortion_argin + j) = principalJacobian.a(k, j);
                }
            }

            idx += 2;
        }
    }

    CORE_ASSERT_TRUE_S(idx == getOutputNum());
    return J;
}

void corecvs::FlatPatternCalibrator::getFullReprojectionError(double out[])
{
    int idx = 0;

    for (size_t i = 0; i < N; ++i)
    {
        auto R = Quaternion::RotationalTransformation(locationData[i].orientation[0], locationData[i].orientation[1], locationData[i].orientation[2], locationData[i].orientation[3], Quaternion::Parametrization::FULL_NORMALIZED, false);
        auto& C = locationData[i].position;

        for (auto& ptp: points[i])
        {
            Vector3dd pp = ptp.point;

            pp[1] *= factor;

            auto res = intrinsics.project(R * (pp - C));
            if (!!(constraints & CameraConstraints::UNLOCK_DISTORTION))
            {
                CORE_ASSERT_TRUE_S(distortionParams.mMapForward);
                res = distortionParams.mapForward(res);
            }
            auto diff = res - ptp.projection;

            out[idx++] = diff.x();
            out[idx++] = diff.y();
        }
    }

    CORE_ASSERT_TRUE_S(idx == getOutputNum());
}

#define IFNOT(cond, expr) \
    if (!(constraints & CameraConstraints::cond)) \
{ \
    expr; \
}

int corecvs::FlatPatternCalibrator::getInputNum() const
{
    int input = 0;
    IFNOT(LOCK_FOCAL,
            input ++;
            IFNOT(EQUAL_FOCAL,
                input++));
    IFNOT(LOCK_PRINCIPAL, input += 2);
    IFNOT(LOCK_SKEW, IFNOT(ZERO_SKEW, input++));

    input += 7 * (int)N;
    input++;

    if (!!(constraints & CameraConstraints::UNLOCK_DISTORTION))
    {
        int polyDeg = distortionEstimationParams.mPolynomDegree;
        if (distortionEstimationParams.mEvenPowersOnly)
            polyDeg /= 2;
        input += polyDeg;
        if (distortionEstimationParams.mEstimateTangent)
            input += 2;
        if (distortionEstimationParams.mEstimateCenter)
            input += 2;
    }

    IFNOT(UNLOCK_YSCALE, input--);
    return input;
}

int corecvs::FlatPatternCalibrator::getOutputNum() const
{
    return (int)K * 2;
}

void corecvs::FlatPatternCalibrator::enforceParams()
{
#define FORCE(s, a, b) \
    if (!!(constraints & CameraConstraints::s)) intrinsics.a = b;
#define LOCK(s, a) \
    if (!!(constraints & CameraConstraints::s)) intrinsics.a = lockParams.a;

    FORCE(ZERO_SKEW, mSkew, 0.0);
    LOCK(LOCK_SKEW, mSkew);

    double f = (intrinsics.fx() + intrinsics.fy()) / 2.0;
    FORCE(EQUAL_FOCAL, mFx, f);
    FORCE(EQUAL_FOCAL, mFy, f);
    LOCK(LOCK_FOCAL, mFx);
    LOCK(LOCK_FOCAL, mFy);

    LOCK(LOCK_PRINCIPAL, mCx);
    LOCK(LOCK_PRINCIPAL, mCy);
#undef FORCE
#undef LOCK
}

bool corecvs::FlatPatternCalibrator::solveInitialIntrinsics()
{
    computeHomographies();
    return computeAbsoluteConic();
}

void corecvs::FlatPatternCalibrator::solveInitialExtrinsics()
{
    int n = (int)homographies.size();

    auto A = (corecvs::Matrix33)intrinsics;
    auto Ai = A.inv();

    for (int i = 0; i < n; ++i)
    {
        auto H = homographies[i];
        corecvs::Vector3dd h1(H.a(0, 0), H.a(1, 0), H.a(2, 0));
        corecvs::Vector3dd h2(H.a(0, 1), H.a(1, 1), H.a(2, 1));
        corecvs::Vector3dd h3(H.a(0, 2), H.a(1, 2), H.a(2, 2));

        double lambda = (1.0 / !(Ai * h1) + 1.0 / !(Ai * h2)) / 2.0;

        auto T = lambda * Ai * h3;
        auto r1 = lambda * Ai * h1;
        auto r2 = lambda * Ai * h2;
        auto r3 = r1 ^ r2;

        corecvs::Matrix33 R(r1[0], r2[0], r3[0],
                            r1[1], r2[1], r3[1],
                            r1[2], r2[2], r3[2]), V;

        corecvs::Vector3dd W;
        corecvs::Matrix::svd(&R, &W, &V);

        corecvs::Matrix33 RO = R * V.transposed();
        auto C = -RO.transposed() * T;

        corecvs::Quaternion orientation = corecvs::Quaternion::FromMatrix(RO);
        locationData[i] = CameraLocationData(C, orientation);
    }
}

void corecvs::FlatPatternCalibrator::readParams(const double in[])
{
#define GET_PARAM(ref) \
    ref = in[argin++];
#define IF_GET_PARAM(cond, ref) \
    if (!!(constraints & CameraConstraints::cond)) ref = in[argin++];
#define IFNOT_GET_PARAM(cond, ref) \
    if (!(constraints & CameraConstraints::cond)) ref = in[argin++];

    int argin = 0;
    IFNOT(LOCK_FOCAL,
            double f;
            GET_PARAM(f);
            intrinsics.mFx = f;
            intrinsics.mFy = f;

            IFNOT_GET_PARAM(EQUAL_FOCAL, intrinsics.mFy));
    IFNOT(LOCK_PRINCIPAL,
            GET_PARAM(intrinsics.mCx);
            GET_PARAM(intrinsics.mCy));
    IFNOT(LOCK_SKEW,
            IFNOT_GET_PARAM(ZERO_SKEW, intrinsics.mSkew));

    for (size_t i = 0; i < N; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            GET_PARAM(locationData[i].position[j]);
        }
        for (int j = 0; j < 4; ++j)
        {
            GET_PARAM(locationData[i].orientation[j]);
        }
    }
    IF_GET_PARAM(UNLOCK_YSCALE, factor);
    if (!!(constraints & CameraConstraints::UNLOCK_DISTORTION))
    {
        distortionParams.mMapForward = true;
        int polyDeg = distortionEstimationParams.mPolynomDegree;
        distortionParams.mKoeff.resize(polyDeg);
        for (auto& k: distortionParams.mKoeff)
            k = 0.0;
        int degStart = 0, degIncrement = 1;
        if (distortionEstimationParams.mEvenPowersOnly)
        {
            polyDeg /= 2;
            degStart = 1;
            degIncrement = 2;
        }
        for (int i = 0; i < polyDeg; ++i, degStart += degIncrement)
        {
            GET_PARAM(distortionParams.mKoeff[degStart]);
        }
        if (distortionEstimationParams.mEstimateTangent)
        {
            GET_PARAM(distortionParams.mTangentialX);
            GET_PARAM(distortionParams.mTangentialY);
        }
        if (distortionEstimationParams.mEstimateCenter)
        {
            GET_PARAM(distortionParams.mPrincipalX);
            GET_PARAM(distortionParams.mPrincipalY);
        }

    }
    CORE_ASSERT_TRUE_S(argin == getInputNum());
#undef GET_PARAM
#undef IF_GET_PARAM
#undef IF_NOT_GET_PARAM
}

#define SET_PARAM(ref) \
    out[argout++] = ref;
#define IF_SET_PARAM(cond, ref) \
    if (!!(constraints & CameraConstraints::cond)) out[argout++] = ref;
#define IFNOT_SET_PARAM(cond, ref) \
    if (!(constraints & CameraConstraints::cond)) out[argout++] = ref;

void corecvs::FlatPatternCalibrator::writeParams(double out[])
{
    int argout = 0;
    IFNOT(LOCK_FOCAL,
            SET_PARAM(intrinsics.mFx);
            IFNOT_SET_PARAM(EQUAL_FOCAL, intrinsics.mFy));
    IFNOT(LOCK_PRINCIPAL,
            SET_PARAM(intrinsics.mCx);
            SET_PARAM(intrinsics.mCy));
    IFNOT(LOCK_SKEW,
            IFNOT_SET_PARAM(ZERO_SKEW, intrinsics.mSkew));

    for (size_t i = 0; i < N; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            SET_PARAM(locationData[i].position[j]);
        }
        for (int j = 0; j < 4; ++j)
        {
            SET_PARAM(locationData[i].orientation[j]);
        }
    }
    IF_SET_PARAM(UNLOCK_YSCALE, factor);
    if (!!(constraints & CameraConstraints::UNLOCK_DISTORTION))
    {
        distortionParams.mMapForward = true;
        int polyDeg = distortionEstimationParams.mPolynomDegree;
        distortionParams.mKoeff.resize(polyDeg);
        int degStart = 0, degIncrement = 1;
        if (distortionEstimationParams.mEvenPowersOnly)
        {
            polyDeg /= 2;
            degStart = 1;
            degIncrement = 2;
        }
        for (int i = 0; i < polyDeg; ++i, degStart += degIncrement)
        {
            SET_PARAM(distortionParams.mKoeff[degStart]);
        }
        if (distortionEstimationParams.mEstimateTangent)
        {
            SET_PARAM(distortionParams.mTangentialX);
            SET_PARAM(distortionParams.mTangentialY);
        }
        if (distortionEstimationParams.mEstimateCenter)
        {
            SET_PARAM(distortionParams.mPrincipalX);
            SET_PARAM(distortionParams.mPrincipalY);
        }

    }
    CORE_ASSERT_TRUE_S(argout == getInputNum());
}

#undef SET_PARAM
#undef IF_SET_PARAM
#undef IFNOT_SET_PARAM
#undef IFNOT

void corecvs::FlatPatternCalibrator::LMCostFunction::operator() (const double in[], double out[])
{
    calibrator->readParams(in);
    calibrator->getFullReprojectionError(out);
}

Matrix corecvs::FlatPatternCalibrator::LMCostFunction::getJacobian(const double in[], double /*dlta*/)
{
    calibrator->readParams(in);
    return calibrator->getJacobian();
}

void corecvs::FlatPatternCalibrator::refineGuess(int LMiterations)
{
    std::vector<double> in(getInputNum()), out(getOutputNum());
    if (!distortionEstimated)
    {
        distortionParams.mPrincipalX = intrinsics.cx();
        distortionParams.mPrincipalY = intrinsics.cy();
        distortionParams.mNormalizingFocal = !intrinsics.principal();
    }
    writeParams(&in[0]);

    LevenbergMarquardt levmar(LMiterations);
    levmar.f = new LMCostFunction(this);

    auto res = levmar.fit(in, out);
    levmar.dampening = LevenbergMarquardt::Dampening::DIAGONAL;
    res = levmar.fit(res, out);

    readParams(&res[0]);
    for (auto& q: locationData)
        q.orientation.normalise();
}

void corecvs::FlatPatternCalibrator::computeHomographies()
{
    homographies.clear();
    for (auto& pts: points)
    {
        if (!pts.size()) continue;

        std::vector<Vector2dd> ptsI, ptsP;

        for (auto& ptp: pts)
        {
            if (!distortionEstimated)
                ptsI.push_back(ptp.projection);
            else
                ptsI.push_back(distortionParams.mapBackward(ptp.projection));
            ptsP.push_back(ptp.point.xy());
        }

        HomographyReconstructor p2i;
        for (size_t i = 0; i < ptsI.size(); ++i)
        {
            p2i.addPoint2PointConstraint(ptsP[i], ptsI[i]);
        }

        corecvs::Matrix33 A, B;
        p2i.normalisePoints(A, B);
        auto res = p2i.getBestHomographyLSE();
        res = p2i.getBestHomographyLM(res);
        res = B.inv() * res * A;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                CORE_ASSERT_TRUE_S(!std::isnan(res.a(i, j)));

        homographies.push_back(res);
    }
}

bool corecvs::FlatPatternCalibrator::computeAbsoluteConic()
{
    absoluteConic = corecvs::Vector(6);

    int n = (int)homographies.size();
    int n_equ = n * 2;
    if (n < 3) forceZeroSkew = true;

    if (forceZeroSkew) ++n_equ;
    //int n_equ_actual = n_equ;
    if (n_equ < 6) n_equ = 6;

    corecvs::Matrix A(n_equ, 6);
    for (int i = 0; i < n_equ; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            A.a(i, j) = 0.0;
        }
    }

    for (int j = 0; j < n; ++j)
    {
        corecvs::Vector v00(6), v01(6), v11(6);
        auto& H = homographies[j];

#define V(I,J) \
        v ## I ## J[0] = H.a(0, I) * H.a(0, J); \
        v ## I ## J[1] = H.a(0, I) * H.a(1, J) + H.a(1, I) * H.a(0, J); \
        v ## I ## J[2] = H.a(1, I) * H.a(1, J); \
        v ## I ## J[3] = H.a(2, I) * H.a(0, J) + H.a(0, I) * H.a(2, J); \
        v ## I ## J[4] = H.a(2, I) * H.a(1, J) + H.a(1, I) * H.a(2, J); \
        v ## I ## J[5] = H.a(2, I) * H.a(2, J);

        V(0, 0);
        V(0, 1);
        V(1, 1);
#undef V

        auto v1 = v01;
        auto v2 = v00 - v11;

        for (int k = 0; k < 6; ++k)
        {
            A.a(2 * j, k) = v1[k];
            A.a(2 * j + 1, k) = v2[k];
        }
    }

    if (forceZeroSkew)
    {
        for (int i = 0; i < 6; ++i)
        {
            A.a(2 * n, i) = i == 1 ? 1.0 : 0.0;
        }
    }

    corecvs::Matrix V(6, 6), W(1, 6);
    corecvs::Matrix::svd(&A, &W, &V);

    std::array<std::pair<double, int>, 6> sv;

    for (int j = 0; j < 6; ++j)
        sv[j] = std::make_pair(W.a(0, j), j);
    std::sort(sv.begin(), sv.end());

    double minErr = std::numeric_limits<double>::max();
    PinholeCameraIntrinsics bestIntrinsics;
    for (int i = 0; i < 6; ++i)
    {
        auto id = sv[i].second;
        for (int j = 0; j < 6; ++j)
        {
            absoluteConic[j] = V.a(j, id);
            CORE_ASSERT_TRUE_S(!std::isnan(absoluteConic[j]));
        }
        if (extractIntrinsics())
        {
            auto err = getRmseReprojectionError();
            if (err < minErr)
            {
                minErr = err;
                bestIntrinsics = intrinsics;
            }
        }
    }
    if (minErr != std::numeric_limits<double>::max())
    {
        intrinsics = bestIntrinsics;
        return true;
    }
    return false;
}

bool corecvs::FlatPatternCalibrator::extractIntrinsics()
{
    double b11, b12, b22, b13, b23, b33;
    b11 = absoluteConic[0];
    b12 = absoluteConic[1];
    b22 = absoluteConic[2];
    b13 = absoluteConic[3];
    b23 = absoluteConic[4];
    b33 = absoluteConic[5];

    if (b11 == 0.0)
        return false;

    double cx, cy, fx, fy, lambda, skew;
    if (b11*b22-b12*b12 <= 0.0)
        return false;
    cy = (b12 * b13 - b11 * b23) / (b11 * b22 - b12 * b12);
    lambda = b33 - (b13 * b13 + cy * (b12 * b13 - b11 * b23)) / b11;
    if (lambda / b11 <= 0.0)
        return false;
    fx = sqrt(lambda / b11);
    fy = sqrt(lambda * b11 / (b11 * b22 - b12 * b12));
    skew = -b12 * fx * fx * fy / lambda;
    cx = skew * cy / fx - b13 * fx * fx / lambda;

#define ASSERT_GOOD(s) \
    CORE_ASSERT_TRUE_S((!std::isnan(s)) && std::isfinite(s));
    ASSERT_GOOD(fx);
    ASSERT_GOOD(fy);
    ASSERT_GOOD(cx);
    ASSERT_GOOD(cy);
    ASSERT_GOOD(skew);

    intrinsics = PinholeCameraIntrinsics(fx, fy, cx, cy, skew);
    intrinsics.setSize(lockParams.size());
    intrinsics.setDistortedSize(lockParams.distortedSize());

    return true;
}
