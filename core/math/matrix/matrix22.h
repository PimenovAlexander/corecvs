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

template<typename ElementType>
class  GenericMatrix22 : public FixedVectorBase<GenericMatrix22<ElementType>, ElementType, 4>
{
    /*TODO: Add MatrixOperations  */

public:
    typedef FixedVectorBase<GenericMatrix22<ElementType>, ElementType, 4> BaseClass;
    typedef Vector2d<ElementType> VectorType;

    static const int H = 2;
    static const int W = H;
    static const int ELEM_NUM = H * W;


    GenericMatrix22() {}

    /**
     * Constructs the 2 by 2 Matrix with the given diagonal element
     * In terms of projective transform that will always be an identity matrix
     *
     * \f[ P=\pmatrix{
     *        d & 0 \cr
     *        0 & d  }
     * \f]
     **/
    explicit GenericMatrix22(ElementType d) : BaseClass(0.0)
    {
        for (int i = 0; i < H; i++)
            a(i,i) = d;
    }

    GenericMatrix22(const FixedVector<ElementType, 4> &data) : BaseClass(data)
    { }

    GenericMatrix22(
             ElementType _a00, ElementType _a01,
             ElementType _a10, ElementType _a11);

    /* Element accessors */
    ElementType &a(int i,int j);
    const ElementType &a(int i,int j) const;

    ElementType &operator ()(int i,int j);
    const ElementType &operator ()(int i,int j) const;

    /* Matrix is so small that for convenience we could provide an accessor for all elements*/
    ElementType &a00();
    ElementType &a01();
    ElementType &a10();
    ElementType &a11();

    const ElementType &a00() const;
    const ElementType &a01() const;
    const ElementType &a10() const;
    const ElementType &a11() const;


    void transpose();
    GenericMatrix22 t() const;
    GenericMatrix22 transposed() const;

    bool isInvertable(ElementType tolerance = 1e-9);
    GenericMatrix22 inverted() const;
    ElementType det() const;
    ElementType trace() const;

    /**
     *  This method solves the system
     *  Ax=b by Kramer method
     **/
    static Vector2d<ElementType> solve(const GenericMatrix22 &A, const VectorType &b);

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
    static void eigen(const GenericMatrix22 &A,
                      ElementType &lambda_max, VectorType &e1,
                      ElementType &lambda_min, VectorType &e2, ElementType EIGTOLERANCE = 1e-9);

    friend inline GenericMatrix22 operator * (const GenericMatrix22 &M1, const GenericMatrix22 &M2)
    {
       return GenericMatrix22(
               M1.a(0,0) * M2.a(0,0) + M1.a(0,1) * M2.a(1,0),
               M1.a(0,0) * M2.a(0,1) + M1.a(0,1) * M2.a(1,1),

               M1.a(1,0) * M2.a(0,0) + M1.a(1,1) * M2.a(1,0),
               M1.a(1,0) * M2.a(0,1) + M1.a(1,1) * M2.a(1,1)
       );
    }

    friend inline GenericMatrix22 operator*= (GenericMatrix22 &M1, const GenericMatrix22 &M2)
    {
        M1 = M1 * M2;
        return M1;
    }

    friend inline VectorType operator *(const GenericMatrix22 &matrix, const VectorType &V)
    {
        return Vector2d<ElementType>(
            matrix.a00() * V.x() + matrix.a01() * V.y(),
            matrix.a10() * V.x() + matrix.a11() * V.y()
        );
    }

    friend inline VectorType operator *(const VectorType &V, const GenericMatrix22 &matrix)
    {
        return VectorType(
           matrix.a00() * V.x() + matrix.a01() * V.x(),
           matrix.a10() * V.y() + matrix.a11() * V.y()
       );
    }


    static GenericMatrix22 Identity();
    static GenericMatrix22 Scale2(ElementType v);
    static GenericMatrix22 VectorByVector(const VectorType &a, const VectorType &b);

    Vector2d<ElementType>     aV(int i) const;
    Vector2d<ElementType>    row(int i) const;
    Vector2d<ElementType>     aW(int i) const;
    Vector2d<ElementType> column(int i) const;


    friend std::ostream & operator <<(std::ostream &out, const GenericMatrix22 &matrix)
    {
        for (int i = 0; i < matrix.H; i++)
        {
            out << "[";
            for (int j = 0; j < matrix.W; j++)
            {
                out << (j == 0 ? "" : " ");
                out << matrix.a(i,j);
            }
            out << "]\n";
        }
        return out;
    }



};

typedef GenericMatrix22<double> Matrix22d;
typedef GenericMatrix22<float> Matrix22f;

typedef Matrix22d Matrix22;

/**
 *  Geting Matrix element functions block
 **/

template<typename ElementType>
inline GenericMatrix22<ElementType>::GenericMatrix22(ElementType _a00, ElementType _a01, ElementType _a10, ElementType _a11)
{
    a(0,0) = _a00;  a(0,1) = _a01;
    a(1,0) = _a10;  a(1,1) = _a11;
}

template<typename ElementType>
inline ElementType &GenericMatrix22<ElementType>::a(int i,int j)
{
    return (*this)[i * W + j];
}

template<typename ElementType>
inline const ElementType &GenericMatrix22<ElementType>::a(int i,int j) const
{
    return (*this)[i * W + j];
}

template<typename ElementType>
inline ElementType &GenericMatrix22<ElementType>::operator ()(int i,int j)
{
    return (*this)[i * W + j];
}

template<typename ElementType>
inline const ElementType &GenericMatrix22<ElementType>::operator ()(int i,int j) const
{
    return (*this)[i * W + j];
}

template<typename ElementType>
inline ElementType &GenericMatrix22<ElementType>::a00()
{
    return (*this)[0];
}

template<typename ElementType>
inline ElementType &GenericMatrix22<ElementType>::a01()
{
    return (*this)[1];
}

template<typename ElementType>
inline ElementType &GenericMatrix22<ElementType>::a10()
{
    return (*this)[2];
}

template<typename ElementType>
inline ElementType &GenericMatrix22<ElementType>::a11()
{
    return (*this)[3];
}

template<typename ElementType>
inline const ElementType &GenericMatrix22<ElementType>::a00() const
{
    return (*this)[0];
}

template<typename ElementType>
inline const ElementType &GenericMatrix22<ElementType>::a01() const
{
    return (*this)[1];
}

template<typename ElementType>
inline const ElementType &GenericMatrix22<ElementType>::a10() const
{
    return (*this)[2];
}

template<typename ElementType>
inline const ElementType &GenericMatrix22<ElementType>::a11() const
{
    return (*this)[3];
}

template<typename ElementType>
inline bool GenericMatrix22<ElementType>::isInvertable(ElementType tolerance)
{
    return det() > tolerance;
}

template<typename ElementType>
inline ElementType GenericMatrix22<ElementType>::det() const {
    return a00() * a11() - a01() * a10();
}
template<typename ElementType>
inline GenericMatrix22<ElementType> GenericMatrix22<ElementType>::inverted() const {
    ElementType d = this->det();
    return GenericMatrix22<ElementType>(
       a11() / d, -a01() / d,
      -a10() / d,  a00() / d
    );
}

template<typename ElementType>
inline Vector2d<ElementType> GenericMatrix22<ElementType>::solve(const GenericMatrix22 &A, const Vector2d<ElementType> &b)
{
    ElementType D = A.det();
    return Vector2d<ElementType> (( A.a11() * b[0] - A.a01() * b[1]), (-A.a10() * b[0] + A.a00() * b[1])) / D;
}


} //namespace  corecvs

#endif // MATRIX22_H_
