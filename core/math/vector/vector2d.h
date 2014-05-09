/**
 * \file vector2d.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Feb 24, 2010
 * \author alexander
 */

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include <math.h>
#include <stdint.h>

#include "global.h"

#ifdef REFLECTION_IN_CORE
#include "reflection.h"
#endif // REFLECTION_IN_CORE

#include "fixedVector.h"

namespace corecvs {


template<typename ElementType>
class Vector2d : public FixedVectorBase<Vector2d<ElementType>, ElementType, 2>
{
protected:
    typedef FixedVectorBase<Vector2d<ElementType>, ElementType, 2> BaseClass;

public:
    enum FieldId {
        FIELD_X = 0,
        FIELD_Y = 1
    };

    Vector2d(const ElementType &_x, const ElementType &_y)
    {
        (*this)[0] = _x;
        (*this)[1] = _y;
    }

    //inline Vector2d(const Vector2d<ElementType> &v): BaseClass(v)  {};
    inline Vector2d(const BaseClass &V) : BaseClass(V) {}
    inline explicit Vector2d(const ElementType &x) : BaseClass(x) {}
    inline Vector2d(): BaseClass() {}

    inline ElementType &x()
    {
        return (*this)[0];
    }

    inline ElementType &y()
    {
        return (*this)[1];
    }

    inline const ElementType &x() const
    {
        return (*this)[0];
    }

    inline const ElementType &y() const
    {
        return (*this)[1];
    }

    inline double getLengthStable () const
    {
        double ma = (x() > 0) ? x() : -x();
        double mb = (y() > 0) ? y() : -y();

        if (ma == 0 && mb == 0) return 0;
        if (ma > mb)
        {
            double div = (mb / ma);
            return ma * sqrt(1.0 + div * div );
        }
        else
        {
            double div = (ma / mb);
            return mb * sqrt(1.0 + div * div );
        }
    }

    inline double argument() const
    {
        if (*this == Vector2d(0.0))
            return 0.0;
        return atan2(y(),x());
    }

    /*TODO: Check if we need this */
    /*template<typename OtherType>
    inline operator Vector2d<OtherType>() const
    {
        return Vector2d<OtherType>((OtherType)((*this)[0]),(OtherType)((*this)[1]));
    }*/

    inline Vector2d<ElementType> leftNormal() const
    {
      return Vector2d(-y(),x());
    }

    inline Vector2d<ElementType> rightNormal() const
    {
      return Vector2d(y(),-x());
    }

    double sineTo(const Vector2d &other) const
    {
        double thisLength  = !(*this);
        double otherLength = !other;

        if (thisLength == 0.0 || otherLength == 0.0)
          return 0.0;
        double tangentCross = (*this) & other.rightNormal();
        return tangentCross / (thisLength * otherLength);
    }

    double cosineTo(const Vector2d &other) const
    {
        double thisLength  = !(*this);
        double otherLength = !other;

        if (thisLength == 0 || otherLength == 0)
          return 0;
        return ((*this) & other) / (thisLength * otherLength);
    }

    /**
     * The oriented area of the parallelogram built on the
     * two vectors: this, and the given one
     *
     * */
    double parallelogramOrientedAreaTo(const Vector2d &other)
    {
        return *this & other.leftNormal();
    }

    double triangleOrientedAreaTo(const Vector2d &other)
    {
        return (*this & other.leftNormal()) / 2;
    }

    /**
     *   Return the vector from angle and length
     **/
    static Vector2d FromPolar(double angle, double length = 1.0)
    {
        return Vector2d(cos(angle), sin(angle)) * length;
    }

    void mapToRect(const Vector2d<ElementType> &low, const Vector2d<ElementType> &high)
    {
        /*if (this->element[0] <  low.element[0]) this->element[0] =  low.element[0];
        if (this->element[1] <  low.element[1]) this->element[1] =  low.element[1];
        if (this->element[0] > high.element[0]) this->element[0] = high.element[0];
        if (this->element[1] > high.element[1]) this->element[1] = high.element[1];*/
        this->mapToHypercube(low, high);
    }

    bool isInRect(const Vector2d<ElementType> &low, const Vector2d<ElementType> &high) const
    {
        return this->isInHypercube(low, high);
    }

//#ifdef REFLECTION_IN_CORE
    static Reflection reflect;

    typedef typename ReflectionHelper<ElementType>::Type ReflectionType;

    static Reflection staticInit()
    {
        Reflection reflection;
        reflection.fields.push_back( new ReflectionType(FIELD_X, ElementType(0), "x") );
        reflection.fields.push_back( new ReflectionType(FIELD_Y, ElementType(0), "y") );
        return reflection;
    }
//#endif

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(x(), static_cast<const ReflectionType *>(reflect.fields[FIELD_X]));
        visitor.visit(y(), static_cast<const ReflectionType *>(reflect.fields[FIELD_Y]));
    }
};


//#ifdef REFLECTION_IN_CORE
template<typename ElementType>
Reflection Vector2d<ElementType>::reflect = Vector2d<ElementType>::staticInit();
//#endif


typedef Vector2d<double> Vector2dd;
typedef Vector2d<uint32_t> Vector2du32;
typedef Vector2d<uint16_t> Vector2du16;

typedef Vector2d<int32_t> Vector2d32;
typedef Vector2d<int16_t> Vector2d16;


} //namespace corecvs
#endif

