#include "core/alignment/angleFunction.h"
#include "core/alignment/radialFunc.h"

namespace corecvs
{
AngleFunction::AngleFunction(const vector<vector<Vector2dd> > &sample, const Vector2d32 &center, int polynomDegree, double scaleFactor) :
    FunctionArgs(polynomDegree, (int)sample.size()),
    mSample(sample),
    mCenter(center),
    mPolynomDegree(polynomDegree),
    mScaleFactor(scaleFactor)
{
}

void AngleFunction::operator ()(const double in[], double out[])
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
            Vector2dd a = (p1 - p2);
            Vector2dd b = (p3 - p2);
            out[i] += pow((a.x() * b.y() - a.y() * b.x()) / (a.l2Metric() * b.l2Metric()), 2);
            p1 = p2;
            p2 = p3;
        }
    }
}

Matrix AngleFunction::getJacobian(const double in[], double delta)
{
    CORE_UNUSED(delta);
//    Matrix res1 = FunctionArgs::getJacobian(in, 1e-25);
    //return res1;
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
            Vector2dd a = (p1 - p2);
            Vector2dd b = (p3 - p2);

            double val = 2 * (a.x() * b.y() - a.y() * b.x()) / (a.l2Metric() * b.l2Metric());

            dFuncDPoints.a(0, (j - 1) * 2) += val * (b.y() * a.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * a.x() / a.l2Metric()) / (b.l2Metric() * pow (a.l2Metric(), 2)); //der p1.x()
            dFuncDPoints.a(0, (j - 1) * 2 + 1) += val * (- b.x() * a.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * a.y() / a.l2Metric()) / (b.l2Metric() * pow (a.l2Metric(), 2)); // der p1.y()
            dFuncDPoints.a(0, j * 2) += val * ((- b.y() + a.y()) * a.l2Metric() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * (a.x() * b.l2Metric() / a.l2Metric() + b.x() * a.l2Metric() / b.l2Metric()))
                    / pow (a.l2Metric() * b.l2Metric(), 2); // der p2.x()
            dFuncDPoints.a(0, j * 2 + 1) += val * ((- a.x() + b.x()) * a.l2Metric() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * (a.y() * b.l2Metric() / a.l2Metric() + b.y() * a.l2Metric() / b.l2Metric()))
                    / pow (a.l2Metric() * b.l2Metric(), 2); //der p2.y()
            dFuncDPoints.a(0, (j + 1) * 2) += val * (-a.y() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * b.x() / b.l2Metric()) / (a.l2Metric() * pow(b.l2Metric(), 2)); // der p3.x()
            dFuncDPoints.a(0, (j + 1) * 2 + 1) += val * (a.x() * b.l2Metric() - (a.x() * b.y() - a.y() * b.x()) * b.y() / b.l2Metric()) / (a.l2Metric() * pow(b.l2Metric(), 2)); // der p3.y()

            p1 = p2;
            p2 = p3;
        }
        Matrix dFuncDKoef = dFuncDPoints * dPointsDKoeff;
        for (int j = 0; j < inputs; j ++)
        {
            result.a(i, j) = dFuncDKoef.a(0, j);
        }
    }
    return result;
}
}
