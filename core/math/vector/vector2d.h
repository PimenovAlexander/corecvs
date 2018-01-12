#ifndef VECTOR2D_H_
#define VECTOR2D_H_

/**
 * \file vector2d.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Feb 24, 2010
 * \author alexander
 */

#include <math.h>
#include <cmath>
#include <stdint.h>

#include "core/utils/global.h"

//#ifdef REFLECTION_IN_CORE
#include "core/reflection/reflection.h"
//#endif // REFLECTION_IN_CORE

#include "core/math/vector/fixedVector.h"
#include "core/math/vector/vector.h"

namespace corecvs {

using std::sqrt;

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

    explicit operator Vector() const
    {
        Vector v(2);
        v[0] = (*this)[0];
        v[1] = (*this)[1];
        return v;
    }

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
        ElementType ma = (x() > 0) ? x() : -x();
        ElementType mb = (y() > 0) ? y() : -y();

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

    /**
     * This function return the angle in the polar coordinates
     *
     * It returns the value in the range -pi..+pi
     **/
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

    template<typename OtherType>
    inline Vector2d<OtherType> cast() const
    {
        return Vector2d<OtherType>((OtherType)((*this)[0]),(OtherType)((*this)[1]));
    }


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

    double azimuthTo(const Vector2d &other) const
    {
        double dot   = (*this) & other;
        double cross = (*this) & other.rightNormal();

        return atan2(cross, dot);
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
        this->mapToHypercube(low, high);
    }

    Vector2d mappedToRect(const Vector2d<ElementType> &low, const Vector2d<ElementType> &high)
    {
        Vector2d toReturn = *this;
        toReturn.mapToRect(low, high);
        return toReturn;
    }

    bool isInRect(const Vector2d<ElementType> &low, const Vector2d<ElementType> &high) const
    {
        return this->isInHypercube(low, high);
    }

    static Vector2d<ElementType> Zero() {
        return Vector2d<ElementType>(0.0, 0.0);
    }


    static Vector2d<ElementType> OrtX() {
        return Vector2d<ElementType>(1.0, 0.0);
    }

    static Vector2d<ElementType> OrtY() {
        return Vector2d<ElementType>(0.0, 1.0);
    }

//#ifdef REFLECTION_IN_CORE
    static Reflection reflection;
    static int        dummy;

    typedef typename ReflectionHelper<ElementType>::Type ReflectionType;

    static int staticInit(const char *name = "")
    {
        reflection.name = name;
        reflection.fields.push_back(new ReflectionType(FIELD_X, ElementType(0), "x"));
        reflection.fields.push_back(new ReflectionType(FIELD_Y, ElementType(0), "y"));

        ReflectionDirectory &directory = *ReflectionDirectoryHolder::getReflectionDirectory();
        directory[std::string(name)]= &reflection;
        // cout << "Adding:" << name << " to directory" << std::endl;
        return 0;
    }
//#endif

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        CORE_ASSERT_TRUE_S(reflection.fields.size() == 2);
        visitor.visit(x(), static_cast<const ReflectionType *>(reflection.fields[FIELD_X]));
        visitor.visit(y(), static_cast<const ReflectionType *>(reflection.fields[FIELD_Y]));
    }
};


typedef Vector2d<double>   Vector2dd;
typedef Vector2d<float>    Vector2df;
typedef Vector2d<uint32_t> Vector2du32;
typedef Vector2d<uint16_t> Vector2du16;

typedef Vector2d<int32_t> Vector2d32;
typedef Vector2d<int16_t> Vector2d16;


//#ifdef REFLECTION_IN_CORE

#if 0   // it's doesn't work - dummy and hence staticInit() don't appear at the code!

template<typename ElementType>
Reflection Vector2d<ElementType>::reflection = Reflection();

template<typename ElementType>
int Vector2d<ElementType>::dummy = Vector2d<ElementType>::staticInit();

#else

// It's moved to vector2d.cpp

//template<> Reflection Vector2d<double>::reflection = Reflection();
//template<> int        Vector2d<double>::dummy = Vector2d<double>::staticInit();
//
//template<> Reflection Vector2d<uint32_t>::reflection = Reflection();
//template<> int        Vector2d<uint32_t>::dummy = Vector2d<uint32_t>::staticInit();
//
//template<> Reflection Vector2d<uint16_t>::reflection = Reflection();
//template<> int        Vector2d<uint16_t>::dummy = Vector2d<uint16_t>::staticInit();
//
//template<> Reflection Vector2d<int32_t>::reflection = Reflection();
//template<> int        Vector2d<int32_t>::dummy = Vector2d<int32_t>::staticInit();
//
//template<> Reflection Vector2d<int16_t>::reflection = Reflection();
//template<> int        Vector2d<int16_t>::dummy = Vector2d<int16_t>::staticInit();

#endif

//#endif


} //namespace corecvs

#endif // VECTOR2D_H_
