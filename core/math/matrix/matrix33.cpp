/**
 * \file matrix33.cpp
 * \brief This file holds the description of the commonly used template -
 * a 3 by 3 matrix.
 *
 * \ingroup cppcorefiles
 * \date Jan 27, 2007
 * \author Alexander Pimenov
 **/


#include "matrix33.h"
#include "matrix.h"
namespace corecvs {

//#define ACCURATE

/**
 * Invertion of the matrix inplace.
 */
void Matrix33::invert()
{
    *this = this->inv();
}

/**
 * \brief This function returns an inverse of the matrix.
 *
 * The calculation is done straight forward using the following formular property
 *
 *  \f[ I^{-1} = \frac 1 {|I|} \pmatrix{ M_{1,1} && \cdots } \f]
 *
 *
 * \attention No check is done, if the matrix is invertable or not
 *
 *
 *
 * \return The \f$I^{-1}\f$
 */
Matrix33 Matrix33::inv() const
{
    Matrix33 toReturn;
    /* Will hold the minors determinant with the changing sign*/
    double ma =   a(1,1) * a(2,2) - a(1,2) * a(2,1);
    double mb = - a(1,0) * a(2,2) + a(1,2) * a(2,0);
    double mc =   a(1,0) * a(2,1) - a(1,1) * a(2,0);

    double det =   a(0,0) * ma
                 + a(0,1) * mb
                 + a(0,2) * mc;

    return Matrix33(
        ma / det,
        (a(0,2) * a(2,1) - a(0,1) * a(2,2) ) / det,
        (a(0,1) * a(1,2) - a(0,2) * a(1,1) ) / det,

        mb / det,
        (a(0,0) * a(2,2) - a(0,2) * a(2,0) ) / det,
        (a(0,2) * a(1,0) - a(0,0) * a(1,2) ) / det,

        mc / det,
        (a(0,1) * a(2,0) - a(0,0) * a(2,1) ) / det,
        (a(0,0) * a(1,1) - a(0,1) * a(1,0) ) / det
    );
}

/**
 *   Computing determinant of the matrix.
 **/

double Matrix33::det() const
{
 return    a(0,0) * (a(1,1) * a(2,2) - a(1,2) * a(2,1))
         - a(0,1) * (a(1,0) * a(2,2) - a(1,2) * a(2,0))
         + a(0,2) * (a(1,0) * a(2,1) - a(1,1) * a(2,0));
}

/**
 *   Computing the trace  of the matrix. The sum of diagonal elements
 **/

double Matrix33::trace() const
{
    return a(0,0) + a(1,1) + a(2,2);
}

/**
 *  Transposing the matrix
 **/
void Matrix33::transpose()
{
   double tmp;
   tmp = a(1,0); a(1,0) = a(0,1); a(0,1) = tmp;
   tmp = a(2,0); a(2,0) = a(0,2); a(0,2) = tmp;
   tmp = a(2,1); a(2,1) = a(1,2); a(1,2) = tmp;
}

void Matrix33::swapRows(int r1, int r2)
{
    double tmp;
    for (int j = 0; j < W; j++)
    {
        tmp = a(r1, j);
        a(r1, j) = a(r2, j);
        a(r2, j) = tmp;
    }
}

void Matrix33::swapColumns(int r1, int r2)
{
    double tmp;
    for (int i = 0; i < H; i++)
    {
        tmp = a(i, r1);
        a(i, r1) = a(i, r2);
        a(i, r2) = tmp;
    }
}


/**
 *  Returning transposed matrix
 **/
Matrix33 Matrix33::transposed() const
{
   return Matrix33(
       a(0,0), a(1,0), a(2,0),
       a(0,1), a(1,1), a(2,1),
       a(0,2), a(1,2), a(2,2)
   );
}




double Matrix33::frobeniusNorm() const
{
    return sqrt(sumAllElementsSq());
}

/**
 *  Negating the matrix - all elements are replaced with their negative values.
 **/
/*void Matrix33::neg()
{
   double *y = x;
   (*y) = -(*y); y++;  (*y) = -(*y); y++;   (*y) = -(*y); y++;
   (*y) = -(*y); y++;  (*y) = -(*y); y++;   (*y) = -(*y); y++;
   (*y) = -(*y); y++;  (*y) = -(*y); y++;   (*y) = -(*y);
}*/

/**
 *  Matrix multiplication
 *  ACCURATE version is slower 1.5 - 2 times
 **/

Matrix33 operator* (const Matrix33 &m1, const Matrix33 &m2)
{
   Matrix33 M;

#ifdef ACCURATE

   double sum;
   for (int i = 0;i < 3; i++)
     for (int j = 0;j <3; j++)
     {
       sum = 0;
       for (int k = 0; k < 3; k++)
         sum+= M1.x[i*3 + k] * M2.x[k*3 + j];
       M.x[i * 3 + j] = sum;
     }

#else
   return Matrix33(
           m1.a(0,0) * m2.a(0,0) + m1.a(0,1) * m2.a(1,0) + m1.a(0,2) * m2.a(2,0),
           m1.a(0,0) * m2.a(0,1) + m1.a(0,1) * m2.a(1,1) + m1.a(0,2) * m2.a(2,1),
           m1.a(0,0) * m2.a(0,2) + m1.a(0,1) * m2.a(1,2) + m1.a(0,2) * m2.a(2,2),

           m1.a(1,0) * m2.a(0,0) + m1.a(1,1) * m2.a(1,0) + m1.a(1,2) * m2.a(2,0),
           m1.a(1,0) * m2.a(0,1) + m1.a(1,1) * m2.a(1,1) + m1.a(1,2) * m2.a(2,1),
           m1.a(1,0) * m2.a(0,2) + m1.a(1,1) * m2.a(1,2) + m1.a(1,2) * m2.a(2,2),

           m1.a(2,0) * m2.a(0,0) + m1.a(2,1) * m2.a(1,0) + m1.a(2,2) * m2.a(2,0),
           m1.a(2,0) * m2.a(0,1) + m1.a(2,1) * m2.a(1,1) + m1.a(2,2) * m2.a(2,1),
           m1.a(2,0) * m2.a(0,2) + m1.a(2,1) * m2.a(1,2) + m1.a(2,2) * m2.a(2,2)
   );
#endif
}


Matrix33 operator*= (Matrix33 &M1, const Matrix33 &M2)
{
    M1 = M1 * M2;
    return M1;
}

/**
 *  Get the i-th row vector
 **/
Vector3dd Matrix33::aV(int i) const
{
    return Vector3dd (a(i,0), a(i,1), a(i,2));
}

Vector3dd Matrix33::row(int i) const
{
    return aV(i);
}

/**
 *  Get the i-th column vector
 **/

Vector3dd Matrix33::aW(int i) const
{
    return Vector3dd (a(0,i), a(1,i), a(2,i));
}

Vector3dd Matrix33::column(int i) const
{
    return aW(i);
}

/**
 *  Get the diagonal vector
 **/
Vector3dd Matrix33::diagonal() const
{
    return Vector3dd (a(0,0), a(1,1), a(2,2));
}



/**
 *
 * Creates a 3D rotation matrix over the Y axis by the angle alpha.
 *
 *
 * \f[ P=\pmatrix{
 *       1 &             0 &           0\cr
 *       0 &  cos(\alpha)  & sin(\alpha)\cr
 *       0 & -sin(\alpha)  & cos(\alpha)}
 * \f]
 *
 * \param alpha angle to rotate by
 * \return the rotation matrix
 */

Matrix33 Matrix33::RotationX (double alpha)
{
  double  sinA = sin(alpha);
  double  cosA = cos(alpha);
  return Matrix33 (
      1,    0,     0,
      0, cosA, -sinA,
      0, sinA,  cosA
  );
}

/**
 *
 * Creates a 3D rotation matrix over the Y axis by the angle alpha.
 *
 *
 * \f[ P=\pmatrix{
 *       cos(\alpha)  & 0  & sin(\alpha)\cr
 *                 0  & 1  &           0\cr
 *       -sin(\alpha) & 0  & cos(\alpha)}
 * \f]
 *
 * \param alpha angle to rotate by
 * \return the rotation matrix
 */
Matrix33 Matrix33::RotationY (double alpha)
{
  double  sinA = sin(alpha);
  double  cosA = cos(alpha);
  return Matrix33 (
      cosA,  0,  sinA,
         0,  1,     0,
     -sinA,  0,  cosA
  );

}


/**
 * Creates a 2D projective transform for the rotation by the angle alpha
 * or a 3d rotation transform.
 *
 *
 * \f[ P=\pmatrix{
 *       cos(\alpha)  & sin(\alpha)  & 0\cr
 *       -sin(\alpha) & cos(\alpha)  & 0\cr
 *                 0  &           0  & 1}
 * \f]
 *
 * \param alpha angle to rotate by
 * \return the rotation matrix
 **/
Matrix33 Matrix33::RotationZ (double alpha)
{
  double  sinA = sin(alpha);
  double  cosA = cos(alpha);
  return Matrix33 (
      cosA, -sinA,  0,
      sinA,  cosA,  0,
         0,     0,  1
  );
}

Matrix33 Matrix33::RotationZ90()
{
    return Matrix33 (
        0, -1,  0,
        1,  0,  0,
        0,  0,  1
    );
}

Matrix33 Matrix33::RotationZ180()
{
    return Matrix33 (
       -1,  0,  0,
        0, -1,  0,
        0,  0,  1
    );
}

Matrix33 Matrix33::RotationZ270()
{
    return Matrix33 (
        0,  1,  0,
       -1,  0,  0,
        0,  0,  1
    );
}


/**
 *  Creates a 3x3 matrix that rotates around the given axis in 3D.
 *
 *
 *  \f[ R = \pmatrix{
 *       u_x^2+(1-u_x^2)c  & u_x u_y(1-c)-u_zs & u_x u_z(1-c)+u_ys \cr
 *       u_x u_y(1-c)+u_zs & u_y^2+(1-u_y^2)c  & u_y u_z(1-c)-u_xs \cr
 *       u_x u_z(1-c)-u_ys & u_y u_z(1-c)+u_xs & u_z^2+(1-u_z^2)c }
 *
 *  \f]
 *
 *   TODO: This is the same code as in Quaternion merge it
 *
 *  \param alpha an angle to rotate by
 *  \param v an axis to rotate around
 *
 *  \return the rotation matrix
 *
 */
Matrix33 Rotate (const Vector3dd& v, double alpha)
{
    double c = cos(alpha);
    double s = sin(alpha);

    Vector3dd u = v / !v;

    return Matrix33(
     u.x() * u.x() + (1 - u.x() * u.x() ) * c,
     u.x() * u.y() * (1 - c) - u.z() * s,
     u.x() * u.z() * (1 - c) + u.y() * s,

     u.x() * u.y() * (1 - c) + u.z() * s,
     u.y() * u.y() + (1 - u.y() * u.y() ) * c,
     u.y() * u.z() * (1 - c) - u.x() * s,

     u.x() * u.z() * (1 - c) - u.y() * s,
     u.y() * u.z() * (1 - c) + u.x() * s,
     u.z() * u.z() + (1 - u.z() * u.z()) * c
    );
}

/*****************************************************************************/
/*   Mirror Matrix Creation                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

Matrix33 Matrix33::MirrorXY()
{
    return Matrix33 (
        1,  0,  0,
        0,  1,  0,
        0,  0, -1
    );
}

Matrix33 Matrix33::MirrorYZ()
{
    return Matrix33 (
       -1,  0,  0,
        0,  1,  0,
        0,  0,  1
    );
}

Matrix33 Matrix33::MirrorXZ()
{
    return Matrix33 (
        1,  0,  0,
        0, -1,  0,
        0,  0,  1
    );
}

/*****************************************************************************/
/*  Scaling Matrix Creation                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

Matrix33 Matrix33::Scale3 (const Vector3dd& v)
{
    return Matrix33 (
        v.x(),     0,     0,
            0, v.y(),     0,
            0,     0, v.z()
    );
}

Matrix33 Matrix33::Scale3(double s1, double s2, double s3)
{
    return Matrix33::Scale3(Vector3dd(s1, s2, s3));
}


/**
 * \brief This function returns a matrix with only 2 scale components
 *
 *
 * \f[ P=\pmatrix{
 *       x   & 0   & 0\cr
 *       0   & y   & 0\cr
 *       0   & 0   & 1}
 * \f]
 *
 * \return The scale matrix
 */
Matrix33 Matrix33::Scale2 (double x, double y)
{
    return Matrix33 (
        x,   0,   0,
        0,   y,   0,
        0,   0,   1
    );
}

Matrix33 Matrix33::Scale3 (double v)
{
    return Matrix33 (v);
}

/**
 * \brief This function returns a matrix with only the projective components
 *
 *
 * \f[ P=\pmatrix{
 *        1  & 0  & 0\cr
 *        0  & 1  & 0\cr
 *        px & py & 1}
 * \f]
 **/
Matrix33 Matrix33::Projective (double px, double py)
{
    return Matrix33 (
        1,   0,  0,
        0,   1,  0,
        px, py,  1
    );
}

/**
 * \brief This function returns a matrix with only shift components
 *
 *
 * \f[ P=\pmatrix{
 *        1  & 0  & x\cr
 *        0  & 1  & y\cr
 *        0  & 0  & 1}
 * \f]
 **/
Matrix33 Matrix33::ShiftProj (double x, double y)
{
   return Matrix33 (
        1,   0,   x,
        0,   1,   y,
        0,   0,   1
   );
}


/**
 *  Produce matrix that is equal to vector product with the given
 *  vector on the right
 *
 * \f[ \mathbf{a} \times \mathbf{b} = [\mathbf{b}]_{\times} \mathbf{a} \f]
 *
 * */
Matrix33 Matrix33::CrossProductRight(const Vector3dd &vector)
{
    return Matrix33::CrossProductLeft(-vector);
}

/**
 * Produce matrix that is equal to vector product with the given vector to the
 * left
 *
 * \f[ \mathbf{a} \times \mathbf{b} = [\mathbf{a}]_{\times} \mathbf{b} \f]
 *
 *
 * */
Matrix33 Matrix33::CrossProductLeft(const Vector3dd &vector)
{
    return Matrix33(
               0.0, -vector.z(),  vector.y(),
        vector.z(),         0.0, -vector.x(),
       -vector.y(),  vector.x(),         0.0
       );
}


/**
 * \brief This function returns a matrix constucted as a product of two vectors
 *  \f$\overrightarrow{A}^T \overrightarrow{B}\f$
 *
 *
 * \f[ P=\pmatrix{
 *        A_x B_x & A_x B_y & A_x B_z \cr
 *        A_y B_x & A_y B_y & A_y B_z \cr
 *        A_z B_x & A_z B_y & A_z B_z }
 * \f]
 **/
Matrix33 Matrix33::VectorByVector(const Vector3dd &a, const Vector3dd &b)
{
   return Matrix33(
        b.x() * a.x(), b.y() * a.x(), b.z() * a.x(),
        b.x() * a.y(), b.y() * a.y(), b.z() * a.y(),
        b.x() * a.z(), b.y() * a.z(), b.z() * a.z()
   );
}

/**
 *  Calculate the Matrix that transform given points \f$(x_i,y_i)\f$ to given images \f$(u_i,v_i)\f$
 *  This is done almost the same way as in rectification Rectificator::getFundamantalRansac() using the SVD
 *
 *  Matrix is searched in form
 *
 * \f[ P=\pmatrix{
 *        a_{00} & a_{01} & a_{0,2}\cr
 *        a_{10} & a_{11} & a_{1,2}\cr
 *        a_{20} & a_{21} &    1
 *        }
 * \f]
 *
 * \f[ u_i = \frac {a_{00} x_i + a_{01} y_i + a_{02}} {a_{20} x_i + a_{21} y_i + a_{22}} \f]
 *
 * \f[ v_i = \frac {a_{10} x_i + a_{11} y_i + a_{12}} {a_{20} x_i + a_{21} y_i + a_{22}} \f]
 *
 * \f[
 *
 *  \pmatrix{
 *  x_0 & y_0 & 1 &  0  &  0  &  0 & -x_0 u_0 & -y_0 u_0 \cr
 *  x_1 & y_1 & 1 &  0  &  0  &  0 & -x_1 u_1 & -y_1 u_1 \cr
 *  x_2 & y_2 & 1 &  0  &  0  &  0 & -x_2 u_2 & -y_2 u_2 \cr
 *  x_3 & y_3 & 1 &  0  &  0  &  0 & -x_3 u_3 & -y_3 u_3 \cr
 *   0  &  0  & 0 & x_0 & y_0 &  1 & -x_0 v_0 & -y_0 v_0 \cr
 *   0  &  0  & 0 & x_1 & y_1 &  1 & -x_1 v_1 & -y_1 v_1 \cr
 *   0  &  0  & 0 & x_2 & y_2 &  1 & -x_2 v_2 & -y_2 v_2 \cr
 *   0  &  0  & 0 & x_3 & y_3 &  1 & -x_3 v_3 & -y_3 v_3 } *
 *
 *  \pmatrix{ a_{00} \cr a_{01} \cr a_{02} \cr
 *            a_{10} \cr a_{11} \cr a_{12} \cr
 *            a_{20} \cr a_{21}   } =
 *
 *  \pmatrix{ u_0 \cr u_1 \cr u_2 \cr u_3 \cr
 *            v_0 \cr v_1 \cr v_2 \cr v_3  }
 *
 * \f]
 *
 */
Matrix33 Matrix33::ProjectiveByPoints(Vector2dd points[4], Vector2dd images[4])
{
    Matrix M(8,8);
    Matrix B(8,1);

    /*TODO: Remove common code in this and the other reconstruction function */
    for (int i = 0; i < 4; i++)
    {
        Vector2dd point = points[i];
        Vector2dd image = images[i];

        M.fillLineWithArgs(i,
             point.x(), point.y(), (double)1.0, (double)0.0, (double)0.0, (double)0.0, -point.x() * image.x(), -point.y() * image.x());
        M.fillLineWithArgs(i + 4,
             (double)0.0, (double)0.0, (double)0.0, point.x(), point.y(), (double)1.0, -point.x() * image.y(), -point.y() * image.y());

        B.a(i,0) = image.x();
        B.a(i+4,0) = image.y();
    }

    Matrix result = M.invSVD() * B;

    Matrix33 toReturn(
            result.a(0,0), result.a(1,0), result.a(2,0),
            result.a(3,0), result.a(4,0), result.a(5,0),
            result.a(6,0), result.a(7,0), 1.0
    );
    return toReturn;
}

/**
 * Get the best transformation for a set of points
 *
 * This functions uses Liner Least Squares Method - http://en.wikipedia.org/wiki/Linear_least_squares_%28mathematics%29
 *
 *
 **/
Matrix33 Matrix33::ProjectiveByPoints(int pointNumber, Vector2dd points[], Vector2dd images[])
{
    ASSERT_TRUE(pointNumber >= 4, "Too few points to create transform");

    if (pointNumber == 4)
        return ProjectiveByPoints(points, images);

    Matrix X(pointNumber * 2,8);
    Matrix B(pointNumber * 2,1);

    for (int i = 0; i < pointNumber; i++)
    {
        Vector2dd point = points[i];
        Vector2dd image = images[i];

        X.fillLineWithArgs(i,
             point.x(), point.y(), (double)1.0, (double)0.0, (double)0.0, (double)0.0, -point.x() * image.x(), -point.y() * image.x());
        X.fillLineWithArgs(i + 4,
             (double)0.0, (double)0.0, (double)0.0, point.x(), point.y(), (double)1.0, -point.x() * image.y(), -point.y() * image.y());

        B.a(i,0) = image.x();
        B.a(i+4,0) = image.y();
    }

    Matrix XTX = X.t() * X;
    Matrix XTB = X.t() * B;

    Matrix result = XTX.invSVD() * XTB;

    Matrix33 toReturn(
              result.a(0,0), result.a(1,0), result.a(2,0),
              result.a(3,0), result.a(4,0), result.a(5,0),
              result.a(6,0), result.a(7,0), 1.0
    );

    return toReturn;
}


} //namespace corecvs

