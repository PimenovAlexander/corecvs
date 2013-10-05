/**
 * \file vector3d.h
 * \brief A 3 component vector
 *
 * \ingroup cppcorefiles
 * \date Feb 18, 2010
 * \author Alexander Pimenov
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "math.h"
#include "vector2d.h"
namespace corecvs {

/*
template<typename ElementType>
struct ThreeElement{
    ElementType x;
    ElementType y;
    ElementType z;
};*/


template<typename ElementType>
class Vector3d : public FixedVectorBase<Vector3d<ElementType>, ElementType, 3>
{
protected:
    typedef FixedVectorBase<Vector3d<ElementType>, ElementType, 3> BaseClass;

public:
    Vector3d(const ElementType &_x,const  ElementType &_y, const ElementType &_z)
    {
        (*this)[0] = _x;
        (*this)[1] = _y;
        (*this)[2] = _z;
    }

    inline Vector3d(const BaseClass &V) : BaseClass(V) {}
    inline explicit Vector3d(const ElementType &x) : BaseClass((ElementType)x) {}
    inline Vector3d(): BaseClass() {}

    inline Vector3d(const Vector2d<ElementType> &V, const ElementType &x) :
        BaseClass(V,x)
    {}

    //@{
    /**
     * Helper functions to access elements
     **/
    inline ElementType &x()
    {
        return (*this)[0];
    }

    inline ElementType &y()
    {
        return (*this)[1];
    }

    inline ElementType &z()
    {
        return (*this)[2];
    }
    //@}

    //@{
    /**
     * Constant versions of helper functions for read-only form constant vectors
     **/
    inline const ElementType &x() const
    {
        return (*this)[0];
    }

    inline const ElementType &y() const
    {
        return (*this)[1];
    }

    inline const ElementType &z() const
    {
        return (*this)[2];
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

    /**
     * \brief This function implements the vector cross product (See http://en.wikipedia.org/wiki/Vector_product).
     * It is computed as follows:
     *
     * \f[
     *    V \times U =
     *    \left| \begin{array}{ccc}
     *        i  &  j  &  k  \\ \hline
     *       V_x & V_y & V_z \\
     *       U_x & U_y & U_z \\
     *    \end{array} \right|
     *  \f]
     *
     * \param[in] V the first vector
     * \param[in] U the second vector
     *
     * \return vector \f[ V \times U\f]
     */
    friend Vector3d operator ^(const Vector3d &V, const Vector3d &U)
    {
        ElementType x, y, z;
        x = V.y() * U.z() - U.y() * V.z();
        y = U.x() * V.z() - U.z() * V.x();
        z = U.y() * V.x() - U.x() * V.y();
        return Vector3d(x, y, z);
    }

    /**
     *  Do the conversion form projective coordinates to the 2D ones
     *
     *  TODO: Should not do this as an automatic conversion
     *
     **/
    inline operator Vector2d<ElementType> ()
    {
        if (this->z() == 0.0)
        {
            return Vector2d<ElementType>(0.0,0.0);
        }
        return Vector2d<ElementType>(this->x() / this->z(), this->y() / this->z());
    }

    /**
     *   Add xy(), yz() and other sub-methods
     *
     *
     **/


    inline Vector3d<ElementType> normalizeProjective()
    {
        if (this->z() == 0.0)
        {
            return Vector3d<ElementType>(0.0);
        }
        return Vector3d<ElementType>(this->x() / this->z(), this->y() / this->z(), ElementType(1.0));
    }


    bool isInCube(const Vector3d<ElementType> &low, const Vector3d<ElementType> &high) const
    {
        return this->isInHypercube(low, high);
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
    }

};

typedef Vector3d<double> Vector3dd;
typedef Vector3d<uint32_t> Vector3du32;
typedef Vector3d<uint16_t> Vector3du16;

typedef Vector3d<int32_t> Vector3d32;
typedef Vector3d<int16_t> Vector3d16;


} //namespace corecvs
#endif

