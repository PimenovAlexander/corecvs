/**
 * \file vector3d.h
 * \brief A 3 component vector
 *
 * \ingroup cppcorefiles
 * \date Feb 18, 2010
 * \author Alexander Pimenov
 */

#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <math.h>

#include "vector2d.h"
#include "vector.h"

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

    explicit operator Vector() const
    {
        Vector v(3);
        v[0] = (*this)[0];
        v[1] = (*this)[1];
        v[2] = (*this)[2];
        return v;
    }
    Vector3d(const Vector& v)
    {
        (*this)[0] = v[0];
        (*this)[1] = v[1];
        (*this)[2] = v[2];
    }

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

    /**
     *   Add xy(), yz() and other sub-methods
     **/

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

    Vector3d<ElementType> yxz() const
    {
        return Vector3d<ElementType>(y(), x(), z());
    }



    void setXY(const Vector2d<ElementType> &vec2d )
    {
        this->x() = vec2d[0];
        this->y() = vec2d[1];
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

    double sineTo(const Vector3d &other) const
    {
        double thisLength  = !(*this);
        double otherLength = !other;

        if (thisLength == 0.0 || otherLength == 0.0)
          return 0.0;
        double cross = ((*this) ^ other).l2Metric();
        return cross / (thisLength * otherLength);
    }

    /**
     *  Not a fastest but stable way to compute an angle
     *
     *  \f[ alpha = atan2(sin(\widehat{V1 V2}) \|V1\|\|V2\|, cos(\widehat{V1 V2}) \|V1\| \|V2\| ) \f]
     *
     **/
    double angleTo(const Vector3d &other) const
    {
        /*double thisLength  = !(*this);
        double otherLength = !other;

        if (thisLength == 0.0 || otherLength == 0.0)
            return 0.0;*/

        double dot   = (*this) & other;
        double cross = ((*this) ^ other).l2Metric();

        return atan2(cross, dot);
    }


    /**
     *  Do the conversion form projective coordinates to the 2D ones
     *
     *  TODO: Should not do this as an automatic conversion
     *
     **/
    inline Vector2d<ElementType> project()
    {
        if (this->z() == 0.0)
        {
            return Vector2d<ElementType>(0.0,0.0);
        }
        return Vector2d<ElementType>(this->x() / this->z(), this->y() / this->z());
    }

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

    /**
     * Calculates two vector that give an ortogonal basis with the current one.
     **/
    void orthogonal(Vector3d<ElementType> &first, Vector3d<ElementType> &second) const
    {
        Vector3d<ElementType> n = this->normalised();
        int min = n.minimumAbsId();
        Vector3d<ElementType> ort(0.0);
        ort[min] = 1.0;

        ort = ort - n * (n & ort);

        first  = ort.normalised();
        second = (ort ^ *this).normalised();
    }

    /**
     *   Return the vector from angle and length and height
     **/
    static Vector3d FromCylindrical(double angle, double length = 1.0, double height = 0.0)
    {
        return Vector3d(cos(angle) * length, sin(angle) * length, height);
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

    /* Some frequently used vectors*/
    static Vector3d<ElementType> Zero() {
        return Vector3d<ElementType>(0.0, 0.0, 0.0);
    }

    static Vector3d<ElementType> OrtX() {
        return Vector3d<ElementType>(1.0, 0.0, 0.0);
    }

    static Vector3d<ElementType> OrtY() {
        return Vector3d<ElementType>(0.0, 1.0, 0.0);
    }

    static Vector3d<ElementType> OrtZ() {
        return Vector3d<ElementType>(0.0, 0.0, 1.0);
    }

};

typedef Vector3d<double>   Vector3dd;
typedef Vector3d<uint32_t> Vector3du32;
typedef Vector3d<uint16_t> Vector3du16;

typedef Vector3d<int32_t> Vector3d32;
typedef Vector3d<int16_t> Vector3d16;


} //namespace corecvs

#endif // _VECTOR3D_H_
