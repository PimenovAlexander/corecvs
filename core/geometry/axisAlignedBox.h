#ifndef AXIS_ALIGNED_BOX_H
#define AXIS_ALIGNED_BOX_H

#include "core/math/vector/vector3d.h"
#include "core/geometry/line.h"
#include "core/xml/generated/axisAlignedBoxParameters.h"

/**
 * \file mesh3d.h
 *
 * \date Dec 13, 2012
 **/

namespace corecvs
{

template<class VectorType>
class UnifedAxisAlignedBox
{
    VectorType mLow;
    VectorType mHigh;
public:

    UnifedAxisAlignedBox()
    {
        _initByLowHigh(VectorType::Zero(), Vector3dd::Zero());
    }

    UnifedAxisAlignedBox(const VectorType &low, const VectorType &high)
    {
        _initByLowHigh(low, high);
    }

    UnifedAxisAlignedBox(const AxisAlignedBoxParameters &params)
    {
        VectorType measure(params.width(), params.height(), params.depth());
        VectorType center (params.x()    , params.y()     , params.z    ());

        _initByCenter(center, measure);
    }

    UnifedAxisAlignedBox(const UnifedAxisAlignedBox &first, const UnifedAxisAlignedBox &second)
    {
        _initByLowHigh(
            first. low().perElementMin(second. low()),
            first.high().perElementMax(second.high())
        );

    }

    void _initByLowHigh(const VectorType &low, const VectorType &high)
    {
        /* TODO: Move this to vector operations and use cycle */
        for (int i = 0; i < VectorType::LENGTH; i++)
        {
            mLow [i] = CORE_MIN(low[i], high[i]);
            mHigh[i] = CORE_MAX(low[i], high[i]);
        }
    }

    void _initByCenter(const VectorType &center, const VectorType &measure)
    {
        _initByLowHigh((center - measure / 2.0), (center + measure / 2.0));
    }

    static UnifedAxisAlignedBox ByCenter(const VectorType &center, const VectorType &measure)
    {
        UnifedAxisAlignedBox box;
        box._initByCenter(center, measure);
        return box;
    }

    static UnifedAxisAlignedBox FromPoints(const vector<VectorType> &points)
    {
       UnifedAxisAlignedBox box = UnifedAxisAlignedBox::Empty();
       for (const VectorType &v : points)
       {
           box.mLow  = box.mLow .perElementMin(v);
           box.mHigh = box.mHigh.perElementMax(v);
       }
       return box;
    }

    /**
     * Use this construction function with caution.
     * It breaks general assumption of mLow beeing smaller in all coordinates then mHigh
     **/
    static UnifedAxisAlignedBox Empty()
    {
        UnifedAxisAlignedBox result;
        for (int i = 0; i < VectorType::LENGTH; i++)
        {
            result.mLow [i] =  std::numeric_limits<double>::max();
            result.mHigh[i] = -std::numeric_limits<double>::max();
        }
        return result;
    }

    static UnifedAxisAlignedBox AllSpace()
    {
        UnifedAxisAlignedBox result;
        for (int i = 0; i < VectorType::LENGTH; i++)
        {
            result.mLow [i] = -std::numeric_limits<double>::max();
            result.mHigh[i] =  std::numeric_limits<double>::max();
        }
        return result;
    }

    bool contains (const VectorType &vector) const
    {
        return vector.isInCube(mLow, mHigh);
    }

    const VectorType &low()  const {
        return mLow;
    }

    const VectorType &high() const {
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

    VectorType getCenter()
    {
        return ((mLow + mHigh) / 2.0);
    }

    VectorType measure()
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

    vector<Vector3dd> getPointsVector() {
        vector<Vector3dd> result;
        result.resize(8);
        getPoints(result.data());
        return result;
    }

    /**
     * Please note this method returns an axis aligned bounding box
     * for a current transformed box
     **/
    template<typename TransformType>
    UnifedAxisAlignedBox transformedBound(const TransformType &transform)
    {
        vector<Vector3dd> result;
        result.resize(8);
        getPoints(result.data());
        for (size_t i = 0; i < result.size(); i++)
        {
            result[i] = transform * result[i];
        }
        return UnifedAxisAlignedBox::FromPoints(result);
    }


    UnifedAxisAlignedBox outset(double value) const
    {
        return UnifedAxisAlignedBox(mLow - VectorType(value), mHigh + VectorType(value));
    }

    friend ostream & operator <<(ostream &out, const UnifedAxisAlignedBox &box)
    {
        out << "[";
            out << box.low() << " -> " << box.high();
        out << "]";
        return out;
    }

    bool intersectWith(const Ray3d &ray, double &t1, double &t2)
    {
        Vector3dd v1 =  mLow - ray.p;
        Vector3dd v2 = mHigh - ray.p;

        /* Last enter */
        t1 = -std::numeric_limits<double>::max();
        /* First leave */
        t2 =  std::numeric_limits<double>::max();

        const Vector3dd &a = ray.a;

        for (int i = 0; i < Vector3dd::LENGTH; i++ )
        {
            double t1n = v1[i] / a[i];
            double t2n = v2[i] / a[i];

            if        (a[i] > 0) {
                if (t1n > t1) t1 = t1n;
                if (t2n < t2) t2 = t2n;
            } else if (a[i] < 0) {
                if (t2n > t1) t1 = t2n;
                if (t1n < t2) t2 = t1n;
            }
        }
        return (t1 <= t2);
    }

};


typedef UnifedAxisAlignedBox<Vector3dd> AxisAlignedBox3d;


} // namespace corecvs

#endif // AXIS_ALIGNED_BOX_H
