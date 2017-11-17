#ifndef MATRIX22_H_
#define MATRIX22_H_

/**
 * \file matrix22.h
 * \brief This file holds the header for the commonly used structure -
 * a 2 by 2 matrix.
 *
 * \f[ P=\pmatrix{
 *        a_{1,1} & a_{1,2} \cr
 *        a_{2,1} & a_{2,2} }
 * \f]
 *
 * \ingroup cppcorefiles
 * \date Aug 31, 2015
 **/

#include <iostream>
#include <limits>
#include <algorithm>
#include <string.h>

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
#include "core/math/vector/fixedVector.h"

namespace corecvs {

using std::numeric_limits;
using std::streamsize;

/**
 *  Class describes the 2 by 2 matrix.
 *
 * \f[ P=\pmatrix{
 *        a_{1,1} & a_{1,2} \cr
 *        a_{2,1} & a_{2,2} }
 * \f]
 *
 *
 **/

class  Matrix22 : public FixedVectorBase<Matrix22, double, 4>
{
public:
    typedef FixedVectorBase<Matrix22, double, 4> BaseClass;

    static const int H = 2;
    static const int W = H;
    static const int ELEM_NUM = H * W;


    Matrix22() {}

    /**
     * Constructs the 2 by 2 Matrix with the given diagonal element
     * In terms of projective transform that will always be an identity matrix
     *
     * \f[ P=\pmatrix{
     *        d & 0 \cr
     *        0 & d  }
     * \f]
     **/
    explicit Matrix22(double d) : BaseClass(0.0)
    {
        for (int i = 0; i < H; i++)
            a(i,i) = d;
    }

    Matrix22(const FixedVector<double, 4> &data) : BaseClass(data)
    { }

    Matrix22(double _a00, double _a01,
             double _a10, double _a11);

    /* Element accessors */
    double &a(int i,int j);
    const double &a(int i,int j) const;

    double &operator ()(int i,int j);
    const double &operator ()(int i,int j) const;

    /* Matrix is so small that for convenience we could provide an accessor for all elements*/
    double &a00();
    double &a01();
    double &a10();
    double &a11();
    const double &a00() const;
    const double &a01() const;
    const double &a10() const;
    const double &a11() const;


    void transpose();
    Matrix22 t() const;
    Matrix22 transposed() const;

    bool isInvertable(double tolerance = 1e-9);
    Matrix22 inverted() const;
    double det() const;
    double trace() const;

    /**
     *  This method solves the system
     *  Ax=b by Kramer method
     **/
    static Vector2dd solve(const Matrix22 &A, const Vector2dd &b);

    /**
     * Comuting eigen values and vectors
     *
     * \param A              - input matrix
     * \param lambda1        - first eigenvalue
     * \param e1             - first eigenvector
     * \param lambda2        - second eigenvalue
     * \param e2             - second eigenvector
     * \param EIGTOLERANCE
     **/
    static void eigen(const Matrix22 &A, double &lambda1, Vector2dd &e1, double &lambda2, Vector2dd &e2, double EIGTOLERANCE = 1e-9);

    friend Matrix22 operator * (const Matrix22 &M1, const Matrix22 &M2);
    friend Matrix22 operator *=(      Matrix22 &M1, const Matrix22 &M2);

    friend inline Vector2dd operator *(const Matrix22 &matrix, const Vector2dd &V);
    friend inline Vector2dd operator *(const Vector2dd &V, const Matrix22 &matrix);

    static Matrix22 Identity();
    static Matrix22 Scale2(double v);
    static Matrix22 VectorByVector(const Vector2dd &a, const Vector2dd &b);

    Vector2dd     aV(int i) const;
    Vector2dd    row(int i) const;
    Vector2dd     aW(int i) const;
    Vector2dd column(int i) const;
};

/**
 *  Geting Matrix element functions block
 **/

inline Matrix22::Matrix22(double _a00, double _a01, double _a10, double _a11)
{
    a(0,0) = _a00;  a(0,1) = _a01;
    a(1,0) = _a10;  a(1,1) = _a11;
}

inline double &Matrix22::a(int i,int j)
{
    return (*this)[i * W + j];
}

inline const double &Matrix22::a(int i,int j) const
{
    return (*this)[i * W + j];
}

inline double &Matrix22::operator ()(int i,int j)
{
    return (*this)[i * W + j];
}

inline const double &Matrix22::operator ()(int i,int j) const
{
    return (*this)[i * W + j];
}

inline double &Matrix22::a00()
{
    return (*this)[0];
}

inline double &Matrix22::a01()
{
    return (*this)[1];
}

inline double &Matrix22::a10()
{
    return (*this)[2];
}

inline double &Matrix22::a11()
{
    return (*this)[3];
}

inline const double &Matrix22::a00() const
{
    return (*this)[0];
}

inline const double &Matrix22::a01() const
{
    return (*this)[1];
}

inline const double &Matrix22::a10() const
{
    return (*this)[2];
}

inline const double &Matrix22::a11() const
{
    return (*this)[3];
}

inline bool Matrix22::isInvertable(double tolerance)
{
    return det() > tolerance;
}

inline double Matrix22::det() const {
    return a00() * a11() - a01() * a10();
}

inline Vector2dd Matrix22::solve(const Matrix22 &A, const Vector2dd &b)
{
    double D = A.det();
    return Vector2dd (( A.a11() * b[0] - A.a01() * b[1]), (-A.a10() * b[0] + A.a00() * b[1])) / D;
}

inline Vector2dd operator *(const Matrix22 &matrix, const Vector2dd &V)
{
    return Vector2dd(
        matrix.a00() * V.x() + matrix.a01() * V.y(),
        matrix.a10() * V.x() + matrix.a11() * V.y()
    );
}

inline Matrix22 operator*= (Matrix22 &M1, const Matrix22 &M2)
{
    M1 = M1 * M2;
    return M1;
}


} //namespace  corecvs

#endif // MATRIX22_H_
