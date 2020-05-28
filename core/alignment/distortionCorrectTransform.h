#pragma once
#include "buffers/abstractBuffer.h"

namespace corecvs {

class DistortionCorrectTransform : public DeformMap<int32_t, double>
{
public:
    DistortionCorrectTransform()
    {
        mRad = 0;
        mCenter = Vector2dd(0, 0);
        mZ = 300;
    }

    DistortionCorrectTransform(Vector2dd const & centre, int r)
    {
        mCenter = centre;
        setRadius(r);
    }
    DistortionCorrectTransform(Vector2dd const & centre, int r, int z)
    {
        mRad = r;
        mZ = z;
        mCenter = centre;
    }
    DistortionCorrectTransform(const Vector2dd & centre)
    {
        mCenter = centre;
    }
    virtual ~DistortionCorrectTransform()
    {
    }

    G12Buffer *doCorrectionTransform(G12Buffer *inputBuffer)
    {
        return inputBuffer->doReverseDeformationBl<G12Buffer, DistortionCorrectTransform>(
                                 this, inputBuffer->h, inputBuffer->w);
    }

    inline Vector2dd map(Vector2dd const & v) const
    {
        if (mCenter.x() == 0)
        {
            return v;
        }
        double x = v.x() - mCenter.x();
        double y = v.y() - mCenter.y();
        double k = 1;
        if (isInteriorPoint(x, y))
            k = 2 * mRad * mZ / (mZ * mZ + pow(x, 2) + pow(y, 2));
        //qDebug() << k;
        return Vector2dd(mCenter.x() + x * k, mCenter.y() + y * k);
    }
    inline Vector2dd map(int32_t y, int32_t x) const
    {
        Vector2dd v(x, y);
        return map(v);
    }
    void setRadius(Vector2dd const & point1, Vector2dd const & point2, Vector2dd const & point3)
    {
        double k = 1; // try k = mCenter.y() / mCenter.x()
        Vector2dd p1((point1.x() - mCenter.x()) * k,
                     point1.y() - mCenter.y());
        Vector2dd p2((point2.x() - mCenter.x()) * k,
                     point2.y() - mCenter.y());
        Vector2dd p3((point3.x() - mCenter.x()) * k,
                     point3.y() - mCenter.y());
        Matrix X(3, 1);
        double const firstR = 500;
        mRad = firstR;
        X.a(0, 0) = 0;
        X.a(1, 0) = 0;
        if (p1.x() * p2.y() - p2.x() * p1.y() != 0)
        {
            X.a(0, 0) = (p1.y() * (firstR + sqrt(firstR * firstR - p2.x() * p2.x() - p2.y() * p2.y())) -
                    p2.y() * (firstR + sqrt(firstR * firstR - p1.x() * p1.x() - p1.y() * p1.y()))) /
                    (p1.x() * p2.y() - p2.x() * p1.y());
            X.a(1, 0) =  - (p1.x() * (firstR + sqrt(firstR * firstR - p2.x() * p2.x() - p2.y() * p2.y())) -
                         p2.x() * (firstR + sqrt(firstR * firstR - p1.x() * p1.x() - p1.y() * p1.y()))) /
                         (p1.x() * p2.y() - p2.x() * p1.y());
        }
        X.a(2, 0) = firstR;
        Matrix *W = new Matrix(3, 3);
        for (int i = 0; i < 10; i ++)
        {
            W->a(0, 0) = p1.x();
            W->a(0, 1) = p1.y();
            W->a(0, 2) = 1 + X.a(2, 0) / sqrt(
                        X.a(2, 0) * X.a(2, 0) - p1.x() * p1.x() - p1.y() * p1.y());
            W->a(1, 0) = p2.x();
            W->a(1, 1) = p2.y();
            W->a(1, 2) = 1 + X.a(2, 0) / sqrt(
                        X.a(2, 0) * X.a(2, 0) - p2.x() * p2.x() - p2.y() * p2.y());
            W->a(2, 0) = p3.x();
            W->a(2, 1) = p3.y();
            W->a(2, 2) = 1 + X.a(2, 0) / sqrt(
                        X.a(2, 0) * X.a(2, 0) - p3.x() * p3.x() - p3.y() * p3.y());
            Matrix invW(3, 3);
            invW = W->inv();

#ifdef DEBUG
            Matrix testE(3, 3);
            testE = invW * (*W);
#endif

            Matrix F(3, 1);
            F.a(0, 0) = X.a(0, 0) * p1.x() + X.a(1, 0) * p1.y() + X.a(2, 0)
                    + sqrt(X.a(2, 0) * X.a(2, 0) - p1.x() * p1.x() - p1.y() * p1.y());
            F.a(1, 0) = X.a(0, 0) * p2.x() + X.a(1, 0) * p2.y() + X.a(2, 0)
                    + sqrt(X.a(2, 0) * X.a(2, 0) - p2.x() * p2.x() - p2.y() * p2.y());
            F.a(2, 0) = X.a(0, 0) * p3.x() + X.a(1, 0) * p3.y() + X.a(2, 0)
                    + sqrt(X.a(2, 0) * X.a(2, 0) - p3.x() * p3.x() - p3.y() * p3.y());
            X = X - invW * F;
            double farPointNorm = CORE_MAX(CORE_MAX(p1.l2Metric(), p2.l2Metric()), p3.l2Metric());
            if (X.a(2, 0) <= farPointNorm)
            {
                // something terrible
                X.a(2, 0) = farPointNorm + (i + 11) * 0.1;
            }
            else
            {
                mRad = X.a(2, 0);
            }
        }
        setZ();
        delete W;
    }
    void setRadius(double r)
    {
        mRad = r;
        setZ();
    }

private:
    Vector2dd mCenter;
    double mRad;
    double mZ;

    bool isInteriorPoint(double x, double y) const
    {
        return (pow(mRad, 2) >= pow(x, 2) + pow(y, 2));
    }

    void setZ()
    {
        mZ = mCenter.l2Metric();
        if (isInteriorPoint(mCenter.x(), mCenter.y()))
        {
            mZ = mRad + sqrt(mRad * mRad - mCenter.x() * mCenter.x() - mCenter.y() * mCenter.y());
        }
    }
};
}
