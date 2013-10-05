#include "straightFunc.h"

namespace corecvs {

StraightFunc::StraightFunc(const vector<vector<Vector2dd> > &sample, const Vector2d32 &center) :
    FunctionArgs((int)sample.size() * 2 + 4, (int)sample.size()),
    mSample(sample),
    mCenter(center)
{
}

//in - {k1, k2, p1, p2, A_0, B_0 ... A_N, B_N} straight equation: sin A * x +  cos A * y + B = 0
// N = sample.size() - 1
void StraightFunc::operator ()(const double in[], double out[])
{
    double k1 = in[0];
    double k2 = in[1];
    double p1 = in[2];
    double p2 = in[3];

    for (unsigned i = 0; i < mSample.size(); i++)
    {
        vector<Vector2dd> straight = mSample.at(i);
        for (unsigned j = 0; j < straight.size(); j ++)
        {
            double x = straight.at(j).x();
            double y = straight.at(j).y();
            double dx = x - mCenter.x();
            double dy = y - mCenter.y();
            double dxsq = dx * dx;
            double dysq = dy * dy;
            double dxdy = dx * dy;

            double rsq = dxsq + dysq;
            double radialCorrection = k1 * rsq +  k2 * rsq * rsq;

            double radialX = (double)dx * radialCorrection;
            double radialY = (double)dy * radialCorrection;

            double tangentX = 2 * p1 * dxdy + p2 * ( rsq + 2 * dxsq );
            double tangentY = p1 * (rsq + 2 * dysq) + 2 * p2 * dxdy;

            x += radialX + tangentX;
            y += radialY + tangentY;

            out[i] += pow(sin(in[i * 2 + 4]) * x + cos(in[i * 2 + 4]) * y - in[i * 2  + 5], 2);
        }
    }
}

Matrix StraightFunc::getJacobian(const double in[], double /*delta*/)
{
    Matrix result(outputs, inputs);
    double k1 = in[0];
    double k2 = in[1];
    double p1 = in[2];
    double p2 = in[3];
    for (unsigned i = 0; i < mSample.size(); i++)
    {
        vector<Vector2dd > straight = mSample.at(i);
        for (unsigned j = 0; j < straight.size(); j ++)
        {
            double x = straight.at(j).x();
            double y = straight.at(j).y();
            double dx = x - mCenter.x();
            double dy = y - mCenter.y();
            double dxsq = dx * dx;
            double dysq = dy * dy;
            double dxdy = dx * dy;

            double rsq = dxsq + dysq;
            double radialCorrection = k1 * rsq +  k2 * rsq * rsq;

            double radialX = (double)dx * radialCorrection;
            double radialY = (double)dy * radialCorrection;

            double tangentX = 2 * p1 * dxdy + p2 * ( rsq + 2 * dxsq );
            double tangentY = p1 * (rsq + 2 * dysq) + 2 * p2 * dxdy;

            x += radialX + tangentX;
            y += radialY + tangentY;

            double fValue = sin(in[i * 2 + 4]) * x + cos(in[i * 2 + 4]) * y - in[i * 2  + 5];

            result.a(i, 0) += 2 * fValue * (dx * sin(in[i * 2 + 4]) * rsq + dy * cos(in[i * 2 + 4]) * rsq);
            result.a(i, 1) += 2 * fValue * (dx * sin(in[i * 2 + 4]) * rsq * rsq + dy * cos(in[i * 2 + 4]) * rsq * rsq);
            result.a(i, 2) += 2 * fValue * (sin(in[i * 2 + 4]) * 2 * dxdy + cos(in[i * 2 + 4]) * (rsq + 2 * dysq));
            result.a(i, 3) += 2 * fValue * (sin(in[i * 2 + 4]) * ( rsq + 2 * dxsq ) + cos(in[i * 2 + 4]) * 2 * dxdy);
            result.a(i, 2 * i + 4) += 2 * fValue * (cos(in[i * 2 + 4]) * x - sin(in[i * 2 + 4] * y));
            result.a(i, 2 * i + 5) += - 2 * fValue;
        }
    }
    return result;
}
}
