#include "radialFunc.h"
#include "radialCorrection.h"

namespace corecvs {

RadialCorrection getRadial(const double in[]);
void getModel(const RadialCorrection &correction, double in[]);


RadialCorrection ModelToRadialCorrection::getRadial(const double in[]) const
{
    int count = 0;
    RadialCorrection result = mLockedDimentions;
    if (mGuessCenter) {
        result.mParams.center.x() = in[count++];
        result.mParams.center.y() = in[count++];
    }
    if (mGuessTangent) {
        result.mParams.p1 = in[count++];
        result.mParams.p2 = in[count++];
    }
    result.mParams.koeff.clear();
    for (int i = 0; i < mPolynomPower; i++) {
        result.mParams.koeff.push_back(in[count++]);
    }
    return result;
}

void ModelToRadialCorrection::getModel(const RadialCorrection &correction, double in[]) const
{
    int count = 0;
    if (mGuessCenter) {
        in[count++] = correction.mParams.center.x();
        in[count++] = correction.mParams.center.y();
    }
    if (mGuessTangent) {
        in[count++] = correction.mParams.p1;
        in[count++] = correction.mParams.p2;
    }
    for (int i = 0; i < mPolynomPower; i++) {
        in[count++] = correction.mParams.koeff[i];
    }
}



RadialFunc::RadialFunc(const vector<Vector2dd> &undistortedPoints, const Vector2dd &center, int polynomDegree) :
    FunctionArgs(polynomDegree, (int)undistortedPoints.size() * 2),
    mUndistortedPoints(undistortedPoints),
    mCenter(center),
    mPolynomDegree(polynomDegree),
    mScaleFactor(1.0)
{
}

/**
 * TODO: keep model in one place only
 **/
void RadialFunc::operator ()(const double /*in*/[], double /*out*/[])
{
#if 0
//    double p1 = in[mPolynomDegree];
//    double p2 = in[mPolynomDegree + 1];
    for (unsigned i = 0; i < mUndistortedPoints.size(); i ++)
    {
        double x = mUndistortedPoints.at(i).x();
        double y = mUndistortedPoints.at(i).y();
        double dx = (x - mCenter.x()) * mScaleFactor;
        double dy = (y - mCenter.y());

        double dxsq = dx * dx;
        double dysq = dy * dy;
      //double dxdy = dx * dy;

        double rsq = dxsq + dysq;
        double radialCorrection = 0;
        double r = sqrt(rsq);
        double rpow = r;
        for (unsigned j = 0; j < mPolynomDegree; j ++)
        {
            radialCorrection += in[j] * rpow;
            rpow *= r;
        }
//        SYNC_PRINT(("RadialFunc::operator (): [%lf %lf ] %lf %lf\n", x, y, rsq, radialCorrection));

        double radialX = dx * radialCorrection;
        double radialY = dy * radialCorrection;

//        double tangentX = 2 * p1 * dxdy + p2 * ( rsq + 2 * dxsq );
//        double tangentY = p1 * (rsq + 2 * dysq) + 2 * p2 * dxdy;

        x += (radialX /*+ tangentX*/) / mScaleFactor;
        y += radialY /*+ tangentY*/;
        out[2 * i] = x;
        out[2 * i + 1] = y;
    }
#endif
}

Matrix RadialFunc::getJacobian(const double /*in*/[], double /*delta*/)
{
//    return FunctionArgs::getJacobian(in, 1e-30);
    Matrix result(outputs, inputs);

    for (unsigned i = 0; i < mUndistortedPoints.size(); i ++)
    {
        double dx = mUndistortedPoints.at(i).x() - mCenter.x();
        double dy = mUndistortedPoints.at(i).y() - mCenter.y();
        double dxsq = dx * dx;
        double dysq = dy * dy;
      //double dxdy = dx * dy;

        double rsq = dxsq + dysq;
        for (int j = 0; j < mPolynomDegree; j ++)
        {
            result.a(i * 2, j) = dx * pow(rsq, (j + 1) * 0.5);
            result.a(i * 2 + 1, j) = dy * pow(rsq, (j + 1) * 0.5);
        }
//        result.a(i * 2, mPolynomDegree) = 2 * dxdy;
//        result.a(i * 2 + 1, mPolynomDegree) = rsq + 2 * dysq;
//        result.a(i * 2, mPolynomDegree + 1) = rsq + 2 * dysq;
//        result.a(i * 2 + 1, mPolynomDegree + 1) = 2 * dxdy;
    }
    return result;
}

void RadialFunc::setScaleFactor(double scaleFactor)
{
    mScaleFactor = scaleFactor;
}
}
