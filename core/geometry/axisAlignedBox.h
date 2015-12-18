#ifndef AXIS_ALIGNED_BOX_H
#define AXIS_ALIGNED_BOX_H

#include "axisAlignedBoxParameters.h"

/**
 * \file mesh3d.h
 *
 * \date Dec 13, 2012
 **/

namespace corecvs
{

class AxisAlignedBox3d
{
    Vector3dd mLow;
    Vector3dd mHigh;
public:

    AxisAlignedBox3d()
    {
        _initByLowHigh(Vector3dd(0.0), Vector3dd(0.0));
    }

    AxisAlignedBox3d(const Vector3dd &low, const Vector3dd &high)
    {
        _initByLowHigh(low, high);
    }

    AxisAlignedBox3d(const AxisAlignedBoxParameters &params)
    {
        Vector3dd measure(params.width(), params.height(), params.depth());
        Vector3dd center (params.x()    , params.y()     , params.z    ());

        _initByCenter(center, measure);
    }

    void _initByLowHigh(const Vector3dd &low, const Vector3dd &high)
    {
        /* TODO: Move this to vector operations and use cycle */
        mLow .x() = CORE_MIN(low.x(), high.x());
        mHigh.x() = CORE_MAX(low.x(), high.x());

        mLow .y() = CORE_MIN(low.y(), high.y());
        mHigh.y() = CORE_MAX(low.y(), high.y());

        mLow .z() = CORE_MIN(low.z(), high.z());
        mHigh.z() = CORE_MAX(low.z(), high.z());
    }

    void _initByCenter(const Vector3dd &center, const Vector3dd &measure)
    {
        _initByLowHigh((center - measure / 2.0), (center + measure / 2.0));
    }

    static AxisAlignedBox3d ByCenter(const Vector3dd &center, const Vector3dd &measure)
    {
        AxisAlignedBox3d box;
        box._initByCenter(center, measure);
        return box;
    }

    bool contains (const Vector3dd &vector) const
    {
        return vector.isInCube(mLow, mHigh);
    }

    const Vector3dd &low()  const {
        return mLow;
    }

    const Vector3dd &high() const {
        return mHigh;
    }

    double width() const {
        return mHigh.x() - mLow.x();
    }

    double height() const {
        return mHigh.y() - mLow.y();
    }

    double depth() const {
        return mHigh.z() - mLow.z();
    }

    Vector3dd getCenter()
    {
        return ((mLow + mHigh) / 2.0);
    }

    Vector3dd measure()
    {
        return mHigh - mLow;
    }

    void getPoints(Vector3dd points[8])
    {
        points[0] = Vector3dd(mLow .x() , mLow .y(), mLow.z());
        points[1] = Vector3dd(mHigh.x() , mLow .y(), mLow.z());
        points[2] = Vector3dd(mHigh.x() , mHigh.y(), mLow.z());
        points[3] = Vector3dd(mLow .x() , mHigh.y(), mLow.z());
        points[4] = Vector3dd(mLow .x() , mLow .y(), mHigh.z());
        points[5] = Vector3dd(mHigh.x() , mLow .y(), mHigh.z());
        points[6] = Vector3dd(mHigh.x() , mHigh.y(), mHigh.z());
        points[7] = Vector3dd(mLow .x() , mHigh.y(), mHigh.z());
    }

    AxisAlignedBox3d outset(double value) const
    {
        return AxisAlignedBox3d(mLow - Vector3dd(value), mHigh + Vector3dd(value));
    }

    friend ostream & operator <<(ostream &out, const AxisAlignedBox3d &box)
    {
        out << "[";
            out << box.low() << " -> " << box.high();
        out << "]";
        return out;
    }

};


} // namespace corecvs

#endif // AXIS_ALIGNED_BOX_H
