#include "core/alignment/radialFunc.h"
#include "core/alignment/radialCorrection.h"

namespace corecvs {

RadialCorrection getRadial(const double in[]);
void getModel(const RadialCorrection &correction, double in[]);


RadialCorrection ModelToRadialCorrection::getRadial(const double in[]) const
{
    int count = 0;
    RadialCorrection result = mLockedDimentions;
    if (mGuessCenter) {
        result.mParams.setPrincipalX(in[count++]);
        result.mParams.setPrincipalY(in[count++]);
    }
    if (mGuessTangent) {
        result.mParams.setTangentialX(in[count++]);
        result.mParams.setTangentialY(in[count++]);
    }
    result.mParams.mKoeff.clear();

    if (mEvenDegreeOnly)
    {
        for (int i = 0; i < mPolynomialDegree; i++)
        {
            if (i % 2) {   /*even powers are stored at odd indices */
                result.mParams.mKoeff.push_back(in[count++]);
            }
            else {
                result.mParams.mKoeff.push_back(0.0);
            }
        }
    }
    else
    {
        for (int i = 0; i < mPolynomialDegree; i++) {
            result.mParams.mKoeff.push_back(in[count++]);
        }
    }
    return result;
}

void ModelToRadialCorrection::getModel(const RadialCorrection &correction, double in[]) const
{
    int count = 0;
    if (mGuessCenter) {
        in[count++] = correction.mParams.principalX();
        in[count++] = correction.mParams.principalY();
    }
    if (mGuessTangent) {
        in[count++] = correction.mParams.tangentialX();
        in[count++] = correction.mParams.tangentialY();
    }

    if (mEvenDegreeOnly) {
        for (int i = 0; i < mPolynomialDegree / 2; i++)
        {
            if (i * 2 + 1 < (int)correction.mParams.mKoeff.size())
            {
                in[count++] = correction.mParams.mKoeff[i * 2 + 1];
            } else {
                in[count++] = 0.0;
            }
        }
    } else {
        int i = 0;
        int copyDegree = CORE_MIN(mPolynomialDegree, (int)correction.mParams.mKoeff.size());
        for (; i < copyDegree; i++)
        {
            in[count++] = correction.mParams.mKoeff[i];
        }
        for (; i < mPolynomialDegree; i++)
        {
            in[count++] = 0.0;
        }
    }
}



#ifdef OUTDATED

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
void RadialFunc::operator ()(const double in[], double out[])
{
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
        for (int j = 0; j < mPolynomDegree; j++)
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
#endif

} // namespace corecvs;



