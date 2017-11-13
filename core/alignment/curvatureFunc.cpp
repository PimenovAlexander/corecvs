#include "core/alignment/curvatureFunc.h"
#include "core/alignment/radialFunc.h"

namespace corecvs
{

CurvatureFunc::CurvatureFunc(const vector<vector<Vector2dd> > &sample, const Vector2d32 &center, int polynomDegree, double scaleFactor) :
    FunctionArgs(polynomDegree /*+ 2*/, (int)sample.size()),
    mSample(sample),
    mCenter(center),
    mPolynomDegree(polynomDegree),
    mScaleFactor(scaleFactor)
{
}

void CurvatureFunc::operator ()(const double in[], double out[])
{
    for (unsigned i = 0; i < mSample.size(); i++)
    {
        vector<Vector2dd> straight = mSample.at(i);
        out[i] = 0;
        if (straight.size() < 3)
            continue;
        Vector newStraight((int)straight.size() * 2);
        RadialFunc f(straight, mCenter, mPolynomDegree);
        f.setScaleFactor(mScaleFactor);
        f(in, newStraight);
        vector<Vector2dd> newVector;
        for (unsigned j = 0; j < straight.size(); j ++)
        {
            newVector.push_back(Vector2dd(newStraight[2 * j], newStraight[2 * j + 1]));
        }
        Vector2dd p1 = newVector.at(0);
        Vector2dd p2 = newVector.at(1);
        for (unsigned j = 1; j < straight.size() - 1; j++)
        {
            Vector2dd p3 = newVector.at(j + 1);
            double a = (p1 - p2).l2Metric();
            double b = (p2 - p3).l2Metric();
            double c = (p1 - p3).l2Metric();
            double p = 0.5 * (a + b + c);
            out[i] +=  4 * sqrt(p * (p - a) * (p - b) * (p - c)) / (a * b * c);
            p1 = p2;
            p2 = p3;
        }
    }
}

Matrix CurvatureFunc::getJacobian(const double in[], double delta)
{
    CORE_UNUSED(delta);
    Matrix res1 = FunctionArgs::getJacobian(in, 1e-25);
    return res1;
    Matrix result(outputs, inputs);
    for (unsigned i = 0; i < mSample.size(); i ++)
    {
        vector<Vector2dd> straight = mSample.at(i);
        RadialFunc f(straight, mCenter, mPolynomDegree);
        Vector newStraight((int)straight.size() * 2);
        f.setScaleFactor(mScaleFactor);
        Matrix dPointsDKoeff = f.getJacobian(in);
        f(in, newStraight);
        Matrix dFuncDPoints(1, (int)straight.size() * 2);
        for (unsigned j = 0; j < straight.size() * 2; j ++)
        {
            dFuncDPoints.a(0, j) = 0;
        }
        vector<Vector2dd> newVector;
        for (unsigned j = 0; j < straight.size(); j ++)
        {
            newVector.push_back(Vector2dd(newStraight[2 * j], newStraight[2 * j + 1]));
        }
        Vector2dd p1 = newVector.at(0);
        Vector2dd p2 = newVector.at(1);
        for (unsigned j = 1; j < straight.size() - 1; j++)
        {
            Vector2dd p3 = newVector.at(j + 1);
            double a = (p1 - p2).l2Metric();
            double b = (p2 - p3).l2Metric();
            double c = (p1 - p3).l2Metric();
            double p = a + b + c;
            if (p == 2 * b || p == 2 * a || p == 2 * c)
            {
                p1 = p2;
                p2 = p3;
                continue;
            }
            double dADX1 = (p1.x() - p2.x()) / a;
            double dADY1 = (p1.y() - p2.y()) / a;
            double dADX2 = - dADX1;
            double dADY2 = - dADY1;
            double dBDX2 = (p2.x() - p3.x()) / b;
            double dBDY2 = (p2.y() - p3.y()) / b;
            double dBDX3 = - dBDX2;
            double dBDY3 = - dBDY2;
            double dCDX1 = (p1.x() - p3.x()) / c;
            double dCDY1 = (p1.y() - p3.y()) / c;
            double dCDX3 = - dCDX1;
            double dCDY3 = - dCDY1;
            double dFDA = (pow(b, 4) - 2 * b * b * c * c - pow (a, 4)) /
                        (b * c * a * a * sqrt(p * (p - 2 * a) * (p - 2 * b) * (p - 2 * c)));
            double dFDB = (pow(a, 4) - 2 * a * a * c * c +  a * pow(c, 4) - pow (b, 4)) /
                        (b * c * b * a * sqrt(p * (p - 2 * a) * (p - 2 * b) * (p - 2 * c)));
            double dFDC = (pow(b, 4) - 2 * b * b * a * a  + pow(a, 4) - pow(c, 4)) /
                        (b * c * c * a * sqrt(p * (p - 2 * a) * (p - 2 * b) * (p - 2 * c)));
            dFuncDPoints.a(0, (j - 1) * 2) += dFDA * dADX1 + dFDC * dCDX1; //der p1.x()
            dFuncDPoints.a(0, (j - 1) * 2 + 1) += dFDA * dADY1 + dFDC * dCDY1; // der p1.y()
            dFuncDPoints.a(0, j * 2) += dFDA * dADX2  + dFDB * dBDX2; // der p2.x()
            dFuncDPoints.a(0, j * 2 + 1) += dFDA * dADY2 + dFDB * dBDY2; //der p2.y()
            dFuncDPoints.a(0, (j + 1) * 2) += dFDB * dBDX3 + dFDC * dCDX3; // der p3.x()
            dFuncDPoints.a(0, (j + 1) * 2 + 1) += dFDB * dBDY3 + dFDC * dCDY3; // der p3.y()
            p1 = p2;
            p2 = p3;
        }
        Matrix dFuncDKoef = dFuncDPoints * dPointsDKoeff;
        for (int j = 0; j < inputs; j ++)
        {
            result.a(i, j) = dFuncDKoef.a(0, j);
        }
    }
//    for (int i = 0; i < outputs; i ++)
//    {
//        for (int j = 0; j <  inputs; j ++)
//        {
//            qDebug () << "at" << i << j << "computational method" << res1.a(i, j) << "derivative" << result.a(i, j);
//        }
//    }
    return result;
}

} // namespace corecvs
