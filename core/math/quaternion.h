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

#include "core/math/matrix/matrix33.h"
#include "core/math/matrix/matrix44.h"
#include "core/math/vector/fixedVector.h"
#include "core/math/mathUtils.h"

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
    {}

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
        t =  - V.x() * Q.x() - V.y() * Q.y() - V.z() * Q.z();
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
#if DEPRICATED
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
#endif
    GenericQuaternion(const ElementType &_t, const VectorType &_v)
    {
        (*this)[0] = _v.x();
        (*this)[1] = _v.y();
        (*this)[2] = _v.z();
        (*this)[3] = _t;
    }

    /**
     * This function is a reworked function form somewhere in Internet. License is unclear
     * This also takes square roots which is very funny :-D and very corecvs-ish (i.e. dumb
     * 'cause you'll get a squared square root in the denominator)
     * */
    template <class MatrixType>
    inline MatrixType toMatrixGeneric() const
    {
        typename MatrixType::ElementType wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        typename MatrixType::ElementType s  = 2.0 / this->operator !();
        x2 = x() * s;
        y2 = y() * s;
        z2 = z() * s;
        xx = x() * x2;   xy = x() * y2;   xz = x() * z2;
        yy = y() * y2;   yz = y() * z2;   zz = z() * z2;
        wx = t() * x2;   wy = t() * y2;   wz = t() * z2;

        MatrixType toReturn = MatrixType::createMatrix(3, 3);
        // We used fillWithArgs here, but with non-pod types, this may lead to problems.
        // A fix could be done with variadic templates.

        toReturn.atm(0, 0) = 1.0 - (yy + zz);   toReturn.atm(0, 1) =        xy - wz;    toReturn.atm(0, 2) =        xz + wy;
        toReturn.atm(1, 0) =        xy + wz;    toReturn.atm(1, 1) = 1.0 - (xx + zz);   toReturn.atm(1, 2) =        yz - wx;
        toReturn.atm(2, 0) =        xz - wy;    toReturn.atm(2, 1) =        yz + wx;    toReturn.atm(2, 2) = 1.0 - (xx + yy);

        return toReturn;
    }

    /*
     * FULL            = full parametrization of *unit* quaternion
     * FULL_NORMALIZED = full parametrization, requires normalization
     * NON_EXCESSIVE   = smooth non-excessive parametrization (w/o one point)
     */
    enum class Parametrization { FULL, FULL_NORMALIZED, NON_EXCESSIVE };

    // XXX: The reason for returning Matrix44 is usage in rigid body transformation context
    static Matrix44 RotationalTransformation(const double &qx, const double &qy, const double &qz, const double &qw, const Parametrization &p, const bool &inverse)
    {
        auto qx2 = qx * qx, qy2 = qy * qy, qz2 = qz * qz, qw2 = qw * qw,
             qxqy= qx * qy, qxqz= qx * qz, qxqw= qx * qw, qyqz= qy * qz, qyqw = qy * qw, qzqw = qz * qw;
        double N;
        switch (p)
        {
            case Parametrization::FULL:
                return !inverse ?
                     Matrix44(1.0 - 2.0*(qy2 + qz2),     2.0*(qxqy - qzqw),     2.0*(qxqz + qyqw), 0.0,
                                  2.0*(qxqy + qzqw), 1.0 - 2.0*(qx2 + qz2),     2.0*(qyqz - qxqw), 0.0,
                                  2.0*(qxqz - qyqw),     2.0*(qyqz + qxqw), 1.0 - 2.0*(qx2 + qy2), 0.0,
                                                0.0,                   0.0,                   0.0, 1.0)
                   : Matrix44(1.0 - 2.0*(qy2 + qz2),     2.0*(qxqy + qzqw),     2.0*(qxqz - qyqw), 0.0,
                                  2.0*(qxqy - qzqw), 1.0 - 2.0*(qx2 + qz2),     2.0*(qyqz + qxqw), 0.0,
                                  2.0*(qxqz + qyqw),     2.0*(qyqz - qxqw), 1.0 - 2.0*(qx2 + qy2), 0.0,
                                                0.0,                   0.0,                   0.0, 1.0);


                break;
            case Parametrization::FULL_NORMALIZED:
                N = qx2 + qy2 + qz2 + qw2;
                return !inverse ?
                    Matrix44((qw2+qx2-qy2-qz2) / N,  2.0*(-qzqw+qxqy) / N,   2.0*(qyqw+qxqz) / N, 0.0,
                               2.0*(qzqw+qxqy) / N, (qw2-qx2+qy2-qz2) / N,  2.0*(-qxqw+qyqz) / N, 0.0,
                              2.0*(-qyqw+qxqz) / N,   2.0*(qxqw+qyqz) / N, (qw2-qx2-qy2+qz2) / N, 0.0,
                                               0.0,                   0.0,                   0.0, 1.0)
                  : Matrix44((qw2+qx2-qy2-qz2) / N,   2.0*(qzqw+qxqy) / N,  2.0*(-qyqw+qxqz) / N, 0.0,
                              2.0*(-qzqw+qxqy) / N, (qw2-qx2+qy2-qz2) / N,   2.0*(qxqw+qyqz) / N, 0.0,
                               2.0*(qyqw+qxqz) / N,  2.0*(-qxqw+qyqz) / N, (qw2-qx2-qy2+qz2) / N, 0.0,
                                               0.0,                   0.0,                   0.0, 1.0);
                break;
            case Parametrization::NON_EXCESSIVE:
                N = (qx2 + qy2 + qz2 - 1.0) * 2.0;
                return !inverse ?
                    Matrix44(N*qy2+N*qz2+1.0,    N*(-qxqy+qz),    -N*(qxqz+qy), 0.0,
                                -N*(qxqy+qz), N*qx2+N*qz2+1.0,     N*(qx-qyqz), 0.0,
                                N*(-qxqz+qy),    -N*(qx+qyqz), N*qx2+N*qy2+1.0, 0.0,
                                         0.0,             0.0,             0.0, 1.0)
                  : Matrix44(N*qy2+N*qz2+1.0,    -N*(qxqy+qz),    N*(-qxqz+qy), 0.0,
                                N*(-qxqy+qz), N*qx2+N*qz2+1.0,    -N*(qx+qyqz), 0.0,
                                -N*(qxqz+qy),     N*(qx-qyqz), N*qx2+N*qy2+1.0, 0.0,
                                         0.0,             0.0,             0.0, 1.0);
                break;
        }
        CORE_ASSERT_TRUE_S(false);
        return Matrix44();
    }

    // XXX: The reason for returning Matrix44 is usage in rigid body transformation context
    static void DiffTransformation(const double &qx, const double &qy, const double &qz, const double &qw, const Parametrization &p, const bool &inverse, Matrix44 &Rqx, Matrix44 &Rqy, Matrix44 &Rqz, Matrix44 &Rqw)
    {
        auto fillFun = inverse ? &Matrix44::FillWithArgsT : &Matrix44::FillWithArgs;
        switch(p)
        {
            case Parametrization::FULL:
                fillFun(Rqx,
                       0.0, 2.0*qy, 2.0*qz, 0.0,
                    2.0*qy,-4.0*qx,-2.0*qw, 0.0,
                    2.0*qz, 2.0*qw,-4.0*qx, 0.0,
                       0.0,    0.0,    0.0, 0.0);
                fillFun(Rqy,
                    -4.0*qy, 2.0*qx, 2.0*qw, 0.0,
                     2.0*qx,    0.0, 2.0*qz, 0.0,
                    -2.0*qw, 2.0*qz,-4.0*qy, 0.0,
                        0.0,    0.0,    0.0, 0.0);
                fillFun(Rqz,
                    -4.0*qz,-2.0*qw, 2.0*qx, 0.0,
                     2.0*qw,-4.0*qz, 2.0*qy, 0.0,
                     2.0*qx, 2.0*qy,    0.0, 0.0,
                        0.0,    0.0,    0.0, 0.0);
                fillFun(Rqw,
                        0.0,-2.0*qz, 2.0*qy, 0.0,
                     2.0*qz,    0.0,-2.0*qx, 0.0,
                    -2.0*qy, 2.0*qx,    0.0, 0.0,
                        0.0,    0.0,    0.0, 0.0);
                break;
            case Parametrization::FULL_NORMALIZED:
                {
                    double qx2 = qx * qx, qy2 = qy * qy, qz2 = qz * qz, qw2 = qw * qw;
                    double qxqy= qx * qy, qxqz = qx * qz, qyqz = qy * qz, qxqw = qx * qw, qyqw = qy * qw, qzqw = qz * qw;
                    double N = qx2 + qy2 + qz2 + qw2;
                    double N2= N * N;
                    fillFun(Rqx,
                                         4.0*qx*(qy2 + qz2) / N2, (4.0*qx*(qzqw - qxqy) + 2.0*qy*N) / N2, (-4.0*qx*(qyqw + qxqz) + 2.0*qz*N) / N2, 0,
                        (-4.0*qx*(qzqw +  qxqy) + 2.0*qy*N) / N2,               -4.0*qx*(qw2 + qy2) / N2, (-2.0*qw*N + 4.0*qx*(qxqw - qyqz)) / N2, 0,
                          (4.0*qx*(qyqw - qxqz) + 2.0*qz*N) / N2, (2.0*qw*N - 4.0*qx*(qxqw + qyqz)) / N2,                -4.0*qx*(qw2 + qz2) / N2, 0,
                                                               0,                                      0,                                       0, 0);
                    fillFun(Rqy,
                                        -4.0*qy*(qw2 + qx2) / N2, (2.0*qx*N + 4.0*qy*(qzqw - qxqy)) / N2, (2.0*qw*N - 4.0*qy*(qyqw + qxqz)) / N2, 0,
                          (2.0*qx*N - 4.0*qy*(qzqw + qxqy)) / N2,                4.0*qy*(qx2 + qz2) / N2, (4.0*qy*(qxqw - qyqz) + 2.0*qz*N) / N2, 0,
                         (-2.0*qw*N + 4.0*qy*(qyqw - qxqz)) / N2,(-4.0*qy*(qxqw + qyqz) + 2.0*qz*N) / N2,               -4.0*qy*(qw2 + qz2) / N2, 0,
                                                               0,                                      0,                                      0, 0);
                    fillFun(Rqz,
                                        -4.0*qz*(qw2 + qx2) / N2,(-2.0*qw*N + 4.0*qz*(qzqw - qxqy)) / N2, (2.0*qx*N - 4.0*qz*(qyqw + qxqz)) / N2, 0,
                          (2.0*qw*N - 4.0*qz*(qzqw + qxqy)) / N2,               -4.0*qz*(qw2 + qy2) / N2, (2.0*qy*N + 4.0*qz*(qxqw - qyqz)) / N2, 0,
                          (2.0*qx*N + 4.0*qz*(qyqw - qxqz)) / N2, (2.0*qy*N - 4.0*qz*(qxqw + qyqz)) / N2,                4.0*qz*(qx2 + qy2) / N2, 0,
                                                               0,                                      0,                                      0, 0);
                    fillFun(Rqw,
                                         4.0*qw*(qy2 + qz2) / N2, (4.0*qw*(qzqw - qxqy) - 2.0*qz*N) / N2, (-4.0*qw*(qyqw + qxqz) + 2.0*qy*N) / N2, 0,
                         (-4.0*qw*(qzqw + qxqy) + 2.0*qz*N) / N2,                4.0*qw*(qx2 + qz2) / N2,  (4.0*qw*(qxqw - qyqz) - 2.0*qx*N) / N2, 0,
                          (4.0*qw*(qyqw - qxqz) - 2.0*qy*N) / N2,(-4.0*qw*(qxqw + qyqz) + 2.0*qx*N) / N2,                 4.0*qw*(qx2 + qy2) / N2, 0,
                                                               0,                                      0,                                       0, 0);

                }
                break;
            case Parametrization::NON_EXCESSIVE:
                {
                    double qx2 = qx * qx, qy2 = qy * qy, qz2 = qz * qz, qxqyqz = qx * qy * qz;
                    double qxqy= qx * qy, qxqz = qx * qz, qyqz = qy * qz,
                        qx3 = qx2 * qx, qy3 = qy2 * qy, qz3 = qz2 * qz, qx2qy = qx2 * qy, qx2qz = qx2 * qz, qxqy2 = qx * qy2, qy2qz = qy2 * qz, qxqz2 = qx * qz2, qyqz2 = qy * qz2;
                    fillFun(Rqx,
                                                    4.0*qx*(qy2+qz2),-6.0*qx2qy+4.0*qxqz-2.0*qy3-2.0*qyqz2+2.0*qy,-6.0*qx2qz-4.0*qxqy-2.0*qy2qz-2.0*qz3+2.0*qz, 0.0,
                        -6.0*qx2qy-4.0*qxqz-2.0*qy3-2.0*qyqz2+2.0*qy,            4.0*qx*(2.0*qx2+qy2+2.0*qz2-1.0),        6.0*qx2-4.0*qxqyqz+2.0*qy2+2.0*qz2-2, 0.0,
                        -6.0*qx2qz+4.0*qxqy-2.0*qy2qz-2.0*qz3+2.0*qz,     -6.0*qx2-4.0*qxqyqz-2.0*qy2-2.0*qz2+2.0,            4.0*qx*(2.0*qx2+2.0*qy2+qz2-1.0), 0.0,
                                                                 0.0,                                         0.0,                                         0.0, 0.0);

                    fillFun(Rqy,
                                    4.0*qy*(qx2+2.0*qy2+2.0*qz2-1.0),-2.0*qx3-6.0*qxqy2-2.0*qxqz2+2.0*qx+4.0*qyqz,     -2.0*qx2-4.0*qxqyqz-6.0*qy2-2.0*qz2+2.0, 0.0,
                        -2.0*qx3-6.0*qxqy2-2.0*qxqz2+2.0*qx-4.0*qyqz,                            4.0*qy*(qx2+qz2),-2.0*qx2qz+4.0*qxqy-6.0*qy2qz-2.0*qz3+2.0*qz, 0.0,
                              2.0*qx2-4.0*qxqyqz+6.0*qy2+2.0*qz2-2.0,-2.0*qx2qz-4.0*qxqy-6.0*qy2qz-2.0*qz3+2.0*qz,            4.0*qy*(2.0*qx2+2.0*qy2+qz2-1.0), 0.0,
                                                                 0.0,                                         0.0,                                         0.0, 0.0);

                    fillFun(Rqz,
                                4.0*qz*(qx2+2.0*qy2+2.0*qz2-1.0),      2.0*qx2-4.0*qxqyqz+2.0*qy2+6.0*qz2-2.0,-2.0*qx3-2.0*qxqy2-6.0*qxqz2+2.0*qx-4.0*qyqz, 0.0,
                         -2.0*qx2-4.0*qxqyqz-2.0*qy2-6.0*qz2+2.0,            4.0*qz*(2.0*qx2+qy2+2.0*qz2-1.0),-2.0*qx2qy+4.0*qxqz-2.0*qy3-6.0*qyqz2+2.0*qy, 0.0,
                    -2.0*qx3-2.0*qxqy2-6.0*qxqz2+2.0*qx+4.0*qyqz,-2.0*qx2qy-4.0*qxqz-2.0*qy3-6.0*qyqz2+2.0*qy,                            4.0*qz*(qx2+qy2), 0.0,
                                                             0.0,                                         0.0,                                         0.0, 0.0);

                }
                break;
        }
    }

    inline Matrix33 toMatrix() const {
        return toMatrixGeneric<Matrix33>();
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
    }


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
        Vector3dd naxis = axis.normalised();

        double sina2 = sin(alpha * 0.5);
        double cosa2 = cos(alpha * 0.5);

        double x = naxis.x() * sina2;
        double y = naxis.y() * sina2;
        double z = naxis.z() * sina2;
        double t = cosa2;

        return GenericQuaternion(x, y, z, t);
    }

    static GenericQuaternion RotationIdentity()
    {
        return GenericQuaternion(0.0, 0.0, 0.0, 1.0);
    }

    static GenericQuaternion Identity()
    {
        return RotationIdentity();
    }

    static GenericQuaternion NaN()
    {
        double notANumber = std::numeric_limits<double>::quiet_NaN();
        return GenericQuaternion(notANumber, notANumber, notANumber, notANumber);
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

#if 1
    static GenericQuaternion exp(const GenericQuaternion &Q)
    {
        double vlen = Q.v().l2Metric();
        return std::exp(Q.t()) * GenericQuaternion(cos(vlen), Q.v().normalised() * sin(vlen));
    }

    static GenericQuaternion ln(const GenericQuaternion &Q)
    {
        double len = Q.l2Metric();
        double cosine = Q.t() / len;
        return GenericQuaternion(std::log(len), Q.v().normalised() * acos(cosine));
    }

    static GenericQuaternion pow(const GenericQuaternion &Q, const ElementType &power)
    {
        return exp(power * ln(Q));
    }
#endif



template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(x(), ElementType(0), "x");
        visitor.visit(y(), ElementType(0), "y");
        visitor.visit(z(), ElementType(0), "z");
        visitor.visit(t(), ElementType(0), "t");
    }

    void printAxisAndAngle(std::ostream &out = std::cout)
    {
        GenericQuaternion o = this->normalised().positivised();
        Vector3dd axis = o.getAxis();
        double   angle = o.getAngle();

        out << "Rotation around normalized axis:" << axis << " angle " << angle << "(" << radToDeg(angle) << " deg)" << std::endl;
    }

};

typedef GenericQuaternion<double> Quaternion;


} //namespace corecvs

#endif /* QUATERNION_H_ */
