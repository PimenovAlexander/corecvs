#include <cmath>

#include "core/math/matrix/matrix22.h"

namespace corecvs {

/**
 *  Transposing the matrix
 **/
template<typename ElementType>
void GenericMatrix22<ElementType>::transpose()
{
   ElementType tmp;
   tmp = a(1,0); a(1,0) = a(0,1); a(0,1) = tmp;
}

template void GenericMatrix22<double>::transpose();
template void GenericMatrix22<float>::transpose();

/**
 *  Returning transposed matrix
 **/
template<typename ElementType>
GenericMatrix22<ElementType> GenericMatrix22<ElementType>::t() const
{
   return GenericMatrix22<ElementType>(
       a(0,0), a(1,0),
       a(0,1), a(1,1));
}

template GenericMatrix22<double> GenericMatrix22<double>::t() const;
template GenericMatrix22<float> GenericMatrix22<float>::t() const;

template<typename ElementType>
GenericMatrix22<ElementType> GenericMatrix22<ElementType>::transposed() const
{
    return t();
}

template GenericMatrix22<double> GenericMatrix22<double>::transposed() const;
template GenericMatrix22<float> GenericMatrix22<float>::transposed() const;

template<typename ElementType>
ElementType GenericMatrix22<ElementType>::trace() const
{
    return a00() + a11();
}

template double GenericMatrix22<double>::trace() const;
template float GenericMatrix22<float>::trace() const;

/**
 *  Get the i-th row vector
 **/
template<typename ElementType>
Vector2d<ElementType> GenericMatrix22<ElementType>::aV(int i) const
{
    return Vector2d<ElementType> (a(i,0), a(i,1));
}

template Vector2d<double> GenericMatrix22<double>::aV(int i) const;
template Vector2d<float> GenericMatrix22<float>::aV(int i) const;

template<typename ElementType>
Vector2d<ElementType> GenericMatrix22<ElementType>::row(int i) const
{
    return aV(i);
}

template Vector2d<double> GenericMatrix22<double>::row(int i) const;
template Vector2d<float> GenericMatrix22<float>::row(int i) const;

/**
 *  Get the i-th column vector
 **/

template<typename ElementType>
Vector2d<ElementType> GenericMatrix22<ElementType>::aW(int i) const
{
    return Vector2d<ElementType> (a(0,i), a(1,i));
}

template Vector2d<double> GenericMatrix22<double>::aW(int i) const;
template Vector2d<float> GenericMatrix22<float>::aW(int i) const;

template<typename ElementType>
Vector2d<ElementType> GenericMatrix22<ElementType>::column(int i) const
{
    return aW(i);
}

template Vector2d<double> GenericMatrix22<double>::column(int i) const;
template Vector2d<float> GenericMatrix22<float>::column(int i) const;

template<typename ElementType>
GenericMatrix22<ElementType> GenericMatrix22<ElementType>::Identity()
{
    return GenericMatrix22<ElementType>(1.0);
}

template GenericMatrix22<double> GenericMatrix22<double>::Identity();
template GenericMatrix22<float> GenericMatrix22<float>::Identity();

template<typename ElementType>
GenericMatrix22<ElementType> GenericMatrix22<ElementType>::Scale2(ElementType v)
{
    return GenericMatrix22<ElementType>(v);
}

template GenericMatrix22<double> GenericMatrix22<double>::Scale2(double v);
template GenericMatrix22<float> GenericMatrix22<float>::Scale2(float v);

template<typename ElementType>
GenericMatrix22<ElementType> GenericMatrix22<ElementType>::VectorByVector(const Vector2d<ElementType> &a, const Vector2d<ElementType> &b)
{
    return GenericMatrix22<ElementType>(
            b.x() * a.x(), b.y() * a.x(),
            b.x() * a.y(), b.y() * a.y()
    );
}

template GenericMatrix22<double> GenericMatrix22<double>::VectorByVector(const Vector2d<double> &a, const Vector2d<double> &b);
template GenericMatrix22<float> GenericMatrix22<float>::VectorByVector(const Vector2d<float> &a, const Vector2d<float> &b);

template<typename ElementType>
void GenericMatrix22<ElementType>::eigen(const GenericMatrix22<ElementType> &A, ElementType &lambda1, Vector2d<ElementType> &e1, ElementType &lambda2, Vector2d<ElementType> &e2, ElementType EIGTOLERANCE)
{
    ElementType T = A.trace();
    ElementType D = A.det();

    ElementType T2 = T / 2.0;

    lambda2 = T2 + std::sqrt(T2 * T2 - D);
    lambda1 = T2 - std::sqrt(T2 * T2 - D);

    ElementType c = std::abs(A.a(1, 0));
    ElementType b = std::abs(A.a(0, 1));

    if (std::max(b, c) > EIGTOLERANCE)
    {
        if (b > c)
        {
            e1 = Vector2d<ElementType>(A.a(0, 1), lambda1 - A.a(0, 0)).normalised();
            e2 = Vector2d<ElementType>(A.a(0, 1), lambda2 - A.a(0, 0)).normalised();
        }
        else
        {
            e1 = Vector2d<ElementType>(lambda1 - A.a(1, 1), A.a(1, 0)).normalised();
            e2 = Vector2d<ElementType>(lambda2 - A.a(1, 1), A.a(1, 0)).normalised();
        }
    }
    else
    {
        e1 = Vector2d<ElementType>(1.0, 0.0);
        e2 = Vector2d<ElementType>(0.0, 1.0);
    }
}

template void GenericMatrix22<double>::eigen(const GenericMatrix22<double> &A, double &lambda1, Vector2d<double> &e1, double &lambda2, Vector2d<double> &e2, double EIGTOLERANCE);
template void GenericMatrix22<float>::eigen(const GenericMatrix22<float> &A, float &lambda1, Vector2d<float> &e1, float &lambda2, Vector2d<float> &e2, float EIGTOLERANCE);

} // namespace corecvs

