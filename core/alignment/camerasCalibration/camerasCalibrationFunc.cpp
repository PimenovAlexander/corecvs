#include "camerasCalibrationFunc.h"

namespace corecvs {

//TODO:fix degrees in polynom
CamerasCalibrationFunc::CamerasCalibrationFunc(const vector<Vector3dd> *sample, const Vector2d32 &center) :
    FunctionArgs(14, (int)sample->size() * 2),
    mSample(sample),
    mCenter(center),
    mIsDistored(true),
    mIsFocalLengthConst(false),
    mIsOriginConst(false)
{
}

void CamerasCalibrationFunc::operator ()(const double in[], double out[])
{
    double k1 = mIsDistored ? in[0] : 0;
    double k2 = mIsDistored ? in[1] : 0;

    double p1 = mIsDistored ? in[2] : 0;
    double p2 = mIsDistored ? in[3] : 0;

    double a = in[4];
    double b = in[5];
    double c = in[6];
    double d = in[7];

    double X0 = in[8];
    double Y0 = in[9];
    double Z0 = in[10];

    double f = in[11];

    double sx = in[12];
    double sy = in[13];

    //
    double len = sqrt (a * a + b * b + c * c + d * d);
    a /= len;
    b /= len;
    c /= len;
    d /= len;

    /**
     *
     * [-d^2-c^2+b^2+a^2, 2*b*c-2*a*d,          2*b*d+2*a*c]
     * [2*b*c+2*a*d,      -d^2+c^2-b^2+a^2,     2*c*d-2*a*b]
     * [2*b*d-2*a*c,      2*c*d+2*a*b,      d^2-c^2-b^2+a^2])
     */

    float da, db, dc, aa, bb, bc, ab, ac, cc, a2, b2, c2;
    float s  = 2.0f/len;  // 4 mul 3 add 1 div
    a2 = a * s;    b2 = b * s;    c2 = c * s;
    aa = a * a2;   ab = a * b2;   ac = a * c2;
    bb = b * b2;   bc = b * c2;   cc = c * c2;
    da = d * a2;   db = d * b2;   dc = d * c2;

    for (unsigned i = 0; i < mSample->size(); i ++)
    {
        double RX  = (1.0 - (bb + cc)) * mSample->at(i).x() + (ab - dc) * mSample->at(i).y() + (ac + db) * mSample->at(i).z();
        double RY  = (ab + dc) * mSample->at(i).x() + (1.0f - (aa + cc)) * mSample->at(i).y() + (bc - da) * mSample->at(i).z();
        double RZ  = (ac - db) * mSample->at(i).x() + (bc + da) * mSample->at(i).y() + (1.0f - (aa + bb)) * mSample->at(i).z();

        double MX = RX + X0;
        double MY = RY + Y0;
        double MZ = RZ + Z0;

        double PX = f * MX / MZ;
        double PY = f * MY / MZ;

        double dx = PX * sx;
        double dy = PY * sy;
        double dxsq = dx * dx;
        double dysq = dy * dy;
        double dxdy = dx * dy;

        double rsq = dxsq + dysq;
        double radialCorrection = k1 * sqrt(rsq) + k2 * rsq;

        double radialX = dx * radialCorrection;
        double radialY = dy * radialCorrection;

        double tangentX = 2 * p1 * dxdy + p2 * ( rsq + 2 * dxsq );
        double tangentY = p1 * (rsq + 2 * dysq) + 2 * p2 * dxdy;

        double correctionX = radialX + tangentX;
        double correctionY = radialY + tangentY;

        out[2 * i] = PX * sx + mCenter.x() + correctionX;
        out[2 * i + 1] = PY * sy + mCenter.y() + correctionY;
    }
}

Matrix CamerasCalibrationFunc::getJacobian(const double in[], double delta)
{
    Matrix result(outputs, inputs);
    result = FunctionArgs::getJacobian(in, delta);

    double a = in[4];
    double b = in[5];
    double c = in[6];
    double d = in[7];

    double X0 = in[8];
    double Y0 = in[9];
    double Z0 = in[10];

    double f = in[11];

    double sx = in[12];
    double sy = in[13];

    //
    double len = sqrt (a * a + b * b + c * c + d * d);
    a /= len;
    b /= len;
    c /= len;
    d /= len;

    /**
     *
     * [-d^2-c^2+b^2+a^2, 2*b*c-2*a*d,          2*b*d+2*a*c]
     * [2*b*c+2*a*d,      -d^2+c^2-b^2+a^2,     2*c*d-2*a*b]
     * [2*b*d-2*a*c,      2*c*d+2*a*b,      d^2-c^2-b^2+a^2])
     */

    float da, db, dc, aa, bb, bc, ab, ac, cc, a2, b2, c2;
    float s  = 2.0 * f / len;  // 4 mul 3 add 1 div
    a2 = a * s;    b2 = b * s;    c2 = c * s;
    aa = a * a2;   ab = a * b2;   ac = a * c2;
    bb = b * b2;   bc = b * c2;   cc = c * c2;
    da = d * a2;   db = d * b2;   dc = d * c2;

    for (unsigned i = 0; i < mSample->size(); i ++)
    {
        double RX  = (1.0 - (bb + cc)) * mSample->at(i).x() + (ab - dc) * mSample->at(i).y() + (ac + db) * mSample->at(i).z();
        double RY  = (ab + dc) * mSample->at(i).x() + (1.0f - (aa + cc)) * mSample->at(i).y() + (bc - da) * mSample->at(i).z();
        double RZ  = (ac - db) * mSample->at(i).x() + (bc + da) * mSample->at(i).y() + (1.0f - (aa + bb)) * mSample->at(i).z();

        double MX = RX + X0;
        double MY = RY + Y0;
        double MZ = RZ + Z0;

        double PX = f * MX / MZ;
        double PY = f * MY / MZ;

        double dx = PX * sx;
        double dy = PY * sy;
        double dxsq = dx * dx;
        double dysq = dy * dy;
        double dxdy = dx * dy;

        double rsq = dxsq + dysq;

        result.a(i * 2, 0) = mIsDistored ? dx * rsq : 0;
        result.a(i * 2 + 1, 0) = mIsDistored ? dy * rsq : 0;
        result.a(i * 2, 1) = mIsDistored ? dx * rsq * rsq : 0;
        result.a(i * 2 + 1, 1) = mIsDistored ? dy * rsq * rsq : 0;
        result.a(i * 2, 2) = mIsDistored ? 2 * dxdy : 0;
        result.a(i * 2 + 1, 2) = mIsDistored ? rsq + 2 * dysq : 0;
        result.a(i * 2, 3) = mIsDistored ? rsq + 2 * dysq : 0;
        result.a(i * 2 + 1, 3) = mIsDistored ? 2 * dxdy : 0;
        if (mIsFocalLengthConst)
        {
            result.a(i * 2, 11) = 0;
            result.a(i * 2 + 1, 11) = 0;
        }
        if (mIsOriginConst)
        {
            result.a(i * 2, 8) = 0;
            result.a(i * 2 + 1, 8) = 0;
            result.a(i * 2, 9) = 0;
            result.a(i * 2 + 1, 9) = 0;
            result.a(i * 2, 10) = 0;
            result.a(i * 2 + 1, 10) = 0;
        }
    }
    return result;
}

void CamerasCalibrationFunc::setIsDistored(bool isDistored)
{
    mIsDistored = isDistored;
}

void CamerasCalibrationFunc::setIsFocalLengthConst(bool isFocalLengthConst)
{
    mIsFocalLengthConst = isFocalLengthConst;
}

void CamerasCalibrationFunc::setIsOriginConst(bool isOriginConst)
{
    mIsOriginConst = isOriginConst;
}
}
