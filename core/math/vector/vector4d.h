/**
 * \file vector3d.h
 * \brief A 3 component vector
 *
 * \ingroup cppcorefiles
 * \date Feb 18, 2010
 * \author Alexander Pimenov
 */

#ifndef _VECTOR4D_H_
#define _VECTOR4D_H_

#include "math.h"
#include "vector2d.h"
#include "vector3d.h"
#include "vector.h"

namespace corecvs {


template<typename ElementType>
class Vector4d : public FixedVectorBase<Vector4d<ElementType>, ElementType, 4>
{
protected:
    typedef FixedVectorBase<Vector4d<ElementType>, ElementType, 4> BaseClass;

public:
    enum {
        FIELD_X,
        FIELD_Y,
        FIELD_Z,
        FIELD_W
    };

    Vector4d(const ElementType &_x,const  ElementType &_y, const ElementType &_z, const ElementType &_w)
    {
        (*this)[FIELD_X] = _x;
        (*this)[FIELD_Y] = _y;
        (*this)[FIELD_Z] = _z;
        (*this)[FIELD_W] = _w;
    }

    inline Vector4d(const BaseClass &V) : BaseClass(V) {}
    inline explicit Vector4d(const ElementType &x) : BaseClass((ElementType)x) {}
    inline Vector4d(): BaseClass() {}

    inline Vector4d(const Vector3d<ElementType> &V, const ElementType &x) :
        BaseClass(V,x)
    {}

    explicit operator Vector() const
    {
        Vector v(4);
        v[0] = (*this)[0];
        v[1] = (*this)[1];
        v[2] = (*this)[2];
        v[3] = (*this)[3];
        return v;
    }


    //@{
    /**
     * Helper functions to access elements
     **/
    inline ElementType &x()
    {
        return (*this)[FIELD_X];
    }

    inline ElementType &y()
    {
        return (*this)[FIELD_Y];
    }

    inline ElementType &z()
    {
        return (*this)[FIELD_Z];
    }

    inline ElementType &w()
    {
        return (*this)[FIELD_W];
    }
    //@}

    //@{
    /**
     * Constant versions of helper functions for read-only form constant vectors
     **/
    inline const ElementType &x() const
    {
        return (*this)[FIELD_X];
    }

    inline const ElementType &y() const
    {
        return (*this)[FIELD_Y];
    }

    inline const ElementType &z() const
    {
        return (*this)[FIELD_Z];
    }

    inline const ElementType &w() const
    {
        return (*this)[FIELD_W];
    }
    //@}

    Vector2d<ElementType> xy() const
    {
        return Vector2d<ElementType>(x(), y());
    }

    Vector2d<ElementType> xz() const
    {
        return Vector2d<ElementType>(x(), z());
    }

    Vector2d<ElementType> yz() const
    {
        return Vector2d<ElementType>(y(), z());
    }

    Vector3d<ElementType> xyz() const
    {
        return Vector3d<ElementType>(x(), y(), z());
    }


    /**
     *  Do the conversion form projective coordinates to the 2D ones
     *
     *  TODO: Should not do this as an automatic conversion
     *
     **/
    inline Vector3d<ElementType> project()
    {
        if (this->w() == 0.0)
        {
            return Vector3d<ElementType>(0.0,0.0,0.0);
        }
        return Vector3d<ElementType>(this->x() / this->w(), this->y() / this->w(), this->z() / this->w());
    }

    /**
     *   Add xy(), yz() and other sub-methods
     *
     *
     **/


    inline Vector4d<ElementType> normalizeProjective()
    {
        if (this->w() == 0.0)
        {
            return Vector4d<ElementType>(0.0);
        }
        return Vector4d<ElementType>(
                    this->x() / this->w(),
                    this->y() / this->w(),
                    this->z() / this->w(), ElementType(1.0));
    }


    /*template<typename OtherType>
    inline operator Vector3d<OtherType>()
    {        return Vector3d<OtherType>(this->a[0],this->a[1], this->a[2]);
    }*/

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(x(), ElementType(0), "x");
        visitor.visit(y(), ElementType(0), "y");
        visitor.visit(z(), ElementType(0), "z");
        visitor.visit(w(), ElementType(0), "w");
    }

    /* Some frequently used vectors*/
    static Vector4d<ElementType> Zero() {
        return Vector4d<ElementType>(0.0, 0.0, 0.0, 0.0);
    }

};

typedef Vector4d<double>   Vector4dd;
typedef Vector4d<uint32_t> Vector4du32;
typedef Vector4d<uint16_t> Vector4du16;

typedef Vector4d<int32_t>  Vector4d32;
typedef Vector4d<int16_t>  Vector4d16;


} //namespace corecvs

#endif // _VECTOR4D_H_

