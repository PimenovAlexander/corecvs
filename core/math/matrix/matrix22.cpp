#include <cmath>

#include "core/math/matrix/matrix22.h"

namespace corecvs {

/**
 *  Transposing the matrix
 **/
void Matrix22::transpose()
{
   double tmp;
   tmp = a(1,0); a(1,0) = a(0,1); a(0,1) = tmp;
}

/**
 *  Returning transposed matrix
 **/
Matrix22 Matrix22::t() const
{
   return Matrix22(
       a(0,0), a(1,0),
       a(0,1), a(1,1));
}
Matrix22 Matrix22::transposed() const
{
    return t();
}

double Matrix22::trace() const
{
    return a00() + a11();
}

/**
 *  Get the i-th row vector
 **/
Vector2dd Matrix22::aV(int i) const
{
    return Vector2dd (a(i,0), a(i,1));
}

Vector2dd Matrix22::row(int i) const
{
    return aV(i);
}

/**
 *  Get the i-th column vector
 **/

Vector2dd Matrix22::aW(int i) const
{
    return Vector2dd (a(0,i), a(1,i));
}

Vector2dd Matrix22::column(int i) const
{
    return aW(i);
}

Matrix22 Matrix22::Identity()
{
    return Matrix22(1.0);
}

Matrix22 Matrix22::Scale2(double v)
{
    return Matrix22(v);
}

Matrix22 Matrix22::VectorByVector(const Vector2dd &a, const Vector2dd &b)
{
    return Matrix22(
            b.x() * a.x(), b.y() * a.x(),
            b.x() * a.y(), b.y() * a.y()
    );
}

Matrix22 operator* (const Matrix22 &m1, const Matrix22 &m2)
{
   return Matrix22(
           m1.a(0,0) * m2.a(0,0) + m1.a(0,1) * m2.a(1,0),
           m1.a(0,0) * m2.a(0,1) + m1.a(0,1) * m2.a(1,1),

           m1.a(1,0) * m2.a(0,0) + m1.a(1,1) * m2.a(1,0),
           m1.a(1,0) * m2.a(0,1) + m1.a(1,1) * m2.a(1,1)
   );
}

void Matrix22::eigen(const Matrix22 &A, double &lambda1, Vector2dd &e1, double &lambda2, Vector2dd &e2, double EIGTOLERANCE)
{
    double T = A.trace();
    double D = A.det();

    double T2 = T / 2.0;

    lambda2 = T2 + std::sqrt(T2 * T2 - D);
    lambda1 = T2 - std::sqrt(T2 * T2 - D);

    double c = std::abs(A.a(1, 0));
    double b = std::abs(A.a(0, 1));

    if (std::max(b, c) > EIGTOLERANCE)
    {
        if (b > c)
        {
            e1 = Vector2dd(A.a(0, 1), lambda1 - A.a(0, 0)).normalised();
            e2 = Vector2dd(A.a(0, 1), lambda2 - A.a(0, 0)).normalised();
        }
        else
        {
            e1 = Vector2dd(lambda1 - A.a(1, 1), A.a(1, 0)).normalised();
            e2 = Vector2dd(lambda2 - A.a(1, 1), A.a(1, 0)).normalised();
        }
    }
    else
    {
        e1 = Vector2dd(1.0, 0.0);
        e2 = Vector2dd(0.0, 1.0);
    }
}

} // namespace corecvs

