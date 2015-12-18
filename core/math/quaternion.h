/**
 * \file quaternion.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "matrix33.h"
#include "fixedVector.h"
#include "mathUtils.h"

namespace corecvs {

const int QUATERNION_DIMENTION = 4;

template<typename ElementType>
class GenericQuaternion : public FixedVectorBase<GenericQuaternion<ElementType>, ElementType, 4>
{
private:
    typedef FixedVectorBase<GenericQuaternion<ElementType>, ElementType, 4> BaseClass;
    typedef Vector3d<ElementType> VectorType;
public:


    inline GenericQuaternion(const GenericQuaternion &Q) : BaseClass(Q)
    {
    }

    inline GenericQuaternion(const ElementType &_x, const ElementType &_y,const ElementType &_z, const ElementType &_t)
    {
        (*this)[0] = _x;
        (*this)[1] = _y;
        (*this)[2] = _z;
        (*this)[3] = _t;

    }

    inline explicit GenericQuaternion(const BaseClass &V) : BaseClass(V) {}
    inline explicit GenericQuaternion(const ElementType &x) : BaseClass(x) {}
    inline GenericQuaternion(): BaseClass() {}

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

    inline ElementType &t()
    {
        return (*this)[3];
    }

    inline VectorType v() const
    {
        return VectorType(x(), y(), z());
    }

    /* Constant versions for read-only form const vectors */
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

    inline const ElementType &t() const
    {
        return (*this)[3];
    }

    /**
     *   Hamilton Product
     *
     *
     *    \f[
     *       (t_1 t_2 - x_1 x_2 - y_1 y_2 - z_1 z_2,
     *        t_1 x_2 + x_1 t_2 + y_1 z_2 - z_1 y_2,
     *        t_1 y_2 + x_1 z_2 + y_1 t_2 - z_1 x_2,
     *        t_1 z_2 + x_1 y_2 + y_1 x_2 - z_1 t_2)
     *    \f]
     *
     *  Totally:
     *      16 multiplications
     *      12 add/subtract
     *
     *      28 overall
     *
     **/

    friend inline GenericQuaternion operator ^(const GenericQuaternion &Q1, const GenericQuaternion &Q2)
    {
        ElementType x, y, z, t;
        t = Q1.t() * Q2.t() - Q1.x() * Q2.x() - Q1.y() * Q2.y() - Q1.z() * Q2.z();
        x = Q1.t() * Q2.x() + Q1.x() * Q2.t() + Q1.y() * Q2.z() - Q1.z() * Q2.y();
        y = Q1.t() * Q2.y() - Q1.x() * Q2.z() + Q1.y() * Q2.t() + Q1.z() * Q2.x();
        z = Q1.t() * Q2.z() + Q1.x() * Q2.y() - Q1.y() * Q2.x() + Q1.z() * Q2.t();

        return GenericQuaternion(x, y, z, t);
    }

    friend inline GenericQuaternion operator ^(const GenericQuaternion &Q, const VectorType &V)
    {
        ElementType x, y, z, t;
        t =               - Q.x() * V.x() - Q.y() * V.y() - Q.z() * V.z();
        x = Q.t() * V.x()                 + Q.y() * V.z() - Q.z() * V.y();
        y = Q.t() * V.y() - Q.x() * V.z()                 + Q.z() * V.x();
        z = Q.t() * V.z() + Q.x() * V.y() - Q.y() * V.x()                ;
        return GenericQuaternion(x, y, z, t);
    }

    friend inline GenericQuaternion operator ^(const VectorType &V, const GenericQuaternion &Q)
    {
        ElementType x, y, z, t;
        y =  - V.x() * Q.x() - V.y() * Q.y() - V.z() * Q.z();
        x =  + V.x() * Q.t() + V.y() * Q.z() - V.z() * Q.y();
        y =  - V.x() * Q.z() + V.y() * Q.t() + V.z() * Q.x();
        z =  + V.x() * Q.y() - V.y() * Q.x() + V.z() * Q.t();
        return GenericQuaternion(x, y, z, t);
    }


    /**
     *  Other version of Hamilton product.
     *  This could be faster in case addition is faster then multiplication
     *  This is not true for x86 Core i7.
     *  Totally:
     *    9 multiplications
     *    1 multiplication by const
     *    27 add/sub
     *
     *    37 overall
     *
     */
    friend inline GenericQuaternion hamilton1(const GenericQuaternion &Q1, const GenericQuaternion &Q2)
    {

        ElementType tt = (Q1.z() + Q1.x()) * (Q2.x() + Q2.y());
        ElementType yy = (Q1.t() - Q1.y()) * (Q2.t() + Q2.z());
        ElementType zz = (Q1.t() + Q1.y()) * (Q2.t() - Q2.z());
        ElementType xx = tt + yy + zz;
        ElementType qq = 0.5 * (xx + (Q1.z() - Q1.x()) * (Q2.x() - Q2.y()));

        ElementType t = qq - tt + (Q1.z() - Q1.y()) * (Q2.y() - Q2.z());
        ElementType x = qq - xx + (Q1.x() + Q1.t()) * (Q2.x() + Q2.t());
        ElementType y = qq - yy + (Q1.t() - Q1.x()) * (Q2.y() + Q2.z());
        ElementType z = qq - zz + (Q1.z() + Q1.y()) * (Q2.t() - Q2.x());

        return GenericQuaternion(x, y, z, t);
    }


    inline GenericQuaternion conjugated() const
    {
        return GenericQuaternion(-this->x(), -this->y(), -this->z(), this->t());
    }

    /**
     *  This makes rotation always positive, by optionally swapping the direction of rotation axis
     **/
    inline GenericQuaternion positivised() const
    {
        if (t() < 0) {
            return - (*this);
        }
        return *this;
    }

    /**
     *  TODO:
     *  Add -
     *
     *   * Reciprocal
     *   * 4x4 Matrix
     *   * Interpolation
     *
     * */
    GenericQuaternion(const VectorType &axis, const ElementType &alpha)
    {
        Vector3dd naxis = axis.normalised();

        double sina2 = sin(alpha * 0.5);
        double cosa2 = cos(alpha * 0.5);
        x() = naxis.x() * sina2;
        y() = naxis.y() * sina2;
        z() = naxis.z() * sina2;
        t() = cosa2;
    }


    /**
     * This function is a reworked function form somewhere in Internet. License is unclear
     * */
    inline Matrix33 toMatrix() const  {
        double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        double s  = 2.0f / this->operator !();
        x2 = x() * s;
        y2 = y() * s;
        z2 = z() * s;
        xx = x() * x2;   xy = x() * y2;   xz = x() * z2;
        yy = y() * y2;   yz = y() * z2;   zz = z() * z2;
        wx = t() * x2;   wy = t() * y2;   wz = t() * z2;

        return Matrix33 (
        1.0f - (yy + zz),     xy - wz     ,     xz + wy,
             xy + wz    , 1.0f - (xx + zz),     yz - wx,
             xz - wy    ,     yz + wx     ,  1.0f - (xx + yy)
        );
    }



    /**
     *   NB - this function expects normalized quaternion as input.
     *   Normalise it explicitly
     **/
    inline double  getAngle() const  {        
        return 2 * acos(t());
    }

    inline VectorType getAxis() const  {
        double sina2 = sqrt(1.0 - (t() * t()));
        return (v() / sina2);
    }

    inline explicit GenericQuaternion(const VectorType &vec)
    {
        x() = vec.x();
        y() = vec.y();
        z() = vec.z();
        t() = 0;
    };


    /**
     * TODO: Speed this up by removing dead code
     * TODO: better do this in common friend style
     **/
    inline const VectorType operator *(const VectorType &input) const
    {
        GenericQuaternion tmp = ((*this) ^ input) ^ this->conjugated();
        return tmp.v();
    }

    static GenericQuaternion RotationX(ElementType alpha)
    {
        ElementType sina2 = sin(alpha * 0.5);
        ElementType cosa2 = cos(alpha * 0.5);
        return GenericQuaternion(sina2, 0.0, 0.0, cosa2);
    }

    static GenericQuaternion RotationY(ElementType alpha)
    {
        ElementType sina2 = sin(alpha * 0.5);
        ElementType cosa2 = cos(alpha * 0.5);
        return GenericQuaternion(0.0, sina2, 0.0, cosa2);
    }

    static GenericQuaternion RotationZ(ElementType alpha)
    {
        ElementType sina2 = sin(alpha * 0.5);
        ElementType cosa2 = cos(alpha * 0.5);
        return GenericQuaternion(0.0, 0.0, sina2, cosa2);
    }

    static GenericQuaternion Rotation(const VectorType &axis, const ElementType &alpha)
    {
        return GenericQuaternion(axis, alpha);
    }

    static GenericQuaternion RotationIdentity()
    {
        return GenericQuaternion(0.0, 0.0, 0.0, 1.0);
    }

    static GenericQuaternion Identity()
    {
        return RotationIdentity();
    }


    static GenericQuaternion FromMatrix(const Matrix33 &R)
    {
        double a = R.a(0,0);
        double b = R.a(1,1);
        double c = R.a(2,2);
        double trace = a + b + c;

        if (trace > 0)
        {
            double r = sqrt(trace + 1.0);
            double i2r = 1 / (2.0 * r);
            double t = r / 2.0;
            double x = ( R.a(2,1) - R.a(1,2)) * i2r;
            double y = ( R.a(0,2) - R.a(2,0)) * i2r;
            double z = ( R.a(1,0) - R.a(0,1)) * i2r;
            return GenericQuaternion(x,y,z,t);
        }

        if (a > b && a > c)
        {
            double r = sqrt(1 + a - b - c);
            double t = (R.a(2,1) - R.a(1,2)) / (2.0 * r);
            double x = r / 2.0;
            double y = (R.a(0,1) + R.a(1,0)) / (2.0 * r);
            double z = (R.a(0,2) + R.a(2,0)) / (2.0 * r);
            return GenericQuaternion(x,y,z,t);
        } else
        if (b > c)
        {
            double r = sqrt(1 + b - a - c);
            double t = (R.a(0,2) - R.a(2,0)) / (2.0 * r);
            double x = (R.a(0,1) + R.a(1,0)) / (2.0 * r);
            double y = r / 2.0;
            double z = (R.a(1,2) + R.a(2,1)) / (2.0 * r);
            return GenericQuaternion(x,y,z,t);
        } else
        {
            double r = sqrt(1 + c - a - b);
            double t = (R.a(1,0) - R.a(0,1)) / (2.0 * r);
            double x = (R.a(0,2) + R.a(2,0)) / (2.0 * r);
            double y = (R.a(1,2) + R.a(2,1)) / (2.0 * r);
            double z = r / 2.0;
            return GenericQuaternion(x,y,z,t);
        }
    }

    static GenericQuaternion slerp(const GenericQuaternion &Q1, const GenericQuaternion &Q2, ElementType t)
    {
        double cos = Q1 & Q2;

        const double THRESHOLD = 0.9995;
        if (cos > THRESHOLD) {
            // Use linear
            GenericQuaternion result = Q1 + t*(Q2 - Q1);
            result.normalise();
            return result;
        }
        double theta = acos(cos);

        double a = sin((1 - t) * theta);
        double b = sin(t * theta);

        GenericQuaternion result = (Q1 * a + Q2 * b) / sin(theta);
        result.normalise();
        return result;
    }


template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(x(), ElementType(0), "x");
        visitor.visit(y(), ElementType(0), "y");
        visitor.visit(z(), ElementType(0), "z");
        visitor.visit(t(), ElementType(0), "t");

    }

    void printAxisAndAngle(std::ostream &out = cout)
    {
        GenericQuaternion o = this->normalised().positivised();
        Vector3dd axis = o.getAxis();
        double   angle = radToDeg(o.getAngle());

        out << "Rotation around: " << axis << " angle " << angle << "(" << angle << " deg)" << std::endl;
    }


};

typedef GenericQuaternion<double> Quaternion;


} //namespace corecvs
#endif /* QUATERNION_H_ */

