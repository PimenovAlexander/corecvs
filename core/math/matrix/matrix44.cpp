/**
 * \file matrix44.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Aug 10, 2010
 * \author alexander
 */

#include "core/math/matrix/matrix44.h"
namespace corecvs {

Matrix44::Matrix44()
{
}


/**
 * Constructs the 4 by 4 Matrix that projects to the plane Z == 1
 * If you are simulating camera you better use Intrinsic Parameters and
 * reduction from projective coordinates to 2D
 *
 * \f[ P=\pmatrix{
 *        1 & 0 & 0 & 0 \cr
 *        0 & 1 & 0 & 0 \cr
 *        0 & 0 & 1 & 0 \cr
 *        0 & 0 & 1 & 0 }
 * \f]
 *
 *
 **/
Matrix44 Matrix44::ProjectParallelToZ()
{
   Matrix44 M;
   double *y = M.element;
   (*y++) =  1.0;  (*y++) =  0.0;  (*y++) =  0.0; (*y++) =  0.0;
   (*y++) =  0.0;  (*y++) =  1.0;  (*y++) =  0.0; (*y++) =  0.0;
   (*y++) =  0.0;  (*y++) =  0.0;  (*y++) =  1.0; (*y++) =  0.0;
   (*y++) =  0.0;  (*y++) =  0.0;  (*y++) =  1.0; (*y)   =  0.0;
   return M;
}

/**
 * Constructs the matrix form affine shift
 *
 *  \f[
 *    \left( \begin{array}{ccc|c}
 *        \multicolumn{3}{c|}{\multirow{3}{*}{I}} & x \\
 *          &   &   & y \\
 *          &   &   & z \\ \hline
 *        0 & 0 & 0 & 1
 *    \end{array} \right)
 *  \f]
 *
 * */
Matrix44 Matrix44::Shift(double x, double y, double z)
{
    Matrix44 M;
    double *e = M.element;
    (*e++) =  1.0;  (*e++) =  0.0;  (*e++) =  0.0; (*e++) =  x;
    (*e++) =  0.0;  (*e++) =  1.0;  (*e++) =  0.0; (*e++) =  y;
    (*e++) =  0.0;  (*e++) =  0.0;  (*e++) =  1.0; (*e++) =  z;
    (*e++) =  0.0;  (*e++) =  0.0;  (*e++) =  0.0; (*e)   =  1.0;
    return M;
}

Matrix44 Matrix44::Shift(const Vector3dd &v)
{
    return Shift(v.x(), v.y(), v.z());
}

Matrix44 Matrix44::Scale(const double &d)
{
    Matrix44 result(d);
    result.a(3,3) = 1.0;
    return result;
}

Matrix44 Matrix44::Scale(const double &d1, const double &d2, const double &d3)
{
    Matrix44 M;
    double *y = M.element;
    (*y++) =  d1;  (*y++) = 0.0;  (*y++) = 0.0; (*y++) = 0.0;
    (*y++) = 0.0;  (*y++) =  d2;  (*y++) = 0.0; (*y++) = 0.0;
    (*y++) = 0.0;  (*y++) = 0.0;  (*y++) =  d3; (*y++) = 0.0;
    (*y++) = 0.0;  (*y++) = 0.0;  (*y++) = 0.0; (*y)   = 1.0;
    return M;
}


Matrix44 Matrix44::RotationX (double alpha)
{
     return Matrix44(Matrix33::RotationX(alpha));
}

Matrix44 Matrix44::RotationY (double alpha)
{
     return Matrix44(Matrix33::RotationY(alpha));
}

Matrix44 Matrix44::RotationZ (double alpha)
{
     return Matrix44(Matrix33::RotationZ(alpha));
}

/*****************************************************************************/
/*   Mirror Matrix Creation                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

Matrix44 Matrix44::MirrorXY()
{
    return Matrix44 (
        1,  0,  0,  0,
        0,  1,  0,  0,
        0,  0, -1,  0,
        0,  0,  0,  1
    );
}

Matrix44 Matrix44::MirrorYZ()
{
    return Matrix44 (
       -1,  0,  0,  0,
        0,  1,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1

    );
}

Matrix44 Matrix44::MirrorXZ()
{
    return Matrix44 (
        1,  0,  0,  0,
        0, -1,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1
   );
}

Matrix44 Matrix44::Frustum(double fovY, double aspect, double zNear, double zFar)
{
    double f = 1.0 / tan(fovY / 2);
    double zDepth = zNear - zFar;
    return Matrix44 (
         f / aspect, 0 ,             0           ,            0            ,
         0         ,-f ,             0           ,            0            ,
         0         , 0 , -(zFar + zNear) / zDepth, 2 *zFar * zNear / zDepth,
         0         , 0 ,             1           ,            0
    );
}


/**
 *
 * Could use Strassen - http://en.wikipedia.org/wiki/Strassen_algorithm
 * but stability is more valuable now.
 * 49 multiplications instead of 64
 **/
Matrix44 operator* (const Matrix44 &M1, const Matrix44 &M2)
{
    Matrix44 result;
    int row, column, runner;
    for (row = 0; row < Matrix44::H ; row++)
    {
        for (column = 0; column < Matrix44::W ; column++)
        {
            double sum = 0;
            for (runner = 0; runner < Matrix44::H ; runner++)
            {
                sum += M1.a(row, runner) * M2.a(runner, column);
            }
            result.a(row, column) = sum;
        }
    }
    return result;
}

/**
 * This is in fact used only for 3D matrix convenience
 *
 * */
Matrix44 operator* (const Matrix44 &M1, const Matrix33 &M2)
{
    return M1 * Matrix44(M2);
}

Matrix44 operator* (const Matrix33 &M1, const Matrix44 &M2)
{
    return Matrix44(M1) * M2;
}

Matrix44 operator*= (Matrix44 &M1, const Matrix44 &M2)
{
    M1 = M1 * M2;
    return M1;
}

Matrix44 operator*= (Matrix44 &M1, const Matrix33 &M2)
{
    M1 = M1 * M2;
    return M1;
}

/**
 *  Transposing the matrix
 **/
void Matrix44::transpose()
{
   double tmp;
   tmp = a(1,0); a(1,0) = a(0,1); a(0,1) = tmp;

   tmp = a(2,0); a(2,0) = a(0,2); a(0,2) = tmp;
   tmp = a(2,1); a(2,1) = a(1,2); a(1,2) = tmp;

   tmp = a(3,0); a(3,0) = a(0,3); a(0,3) = tmp;
   tmp = a(3,1); a(3,1) = a(1,3); a(1,3) = tmp;
   tmp = a(3,2); a(3,2) = a(2,3); a(2,3) = tmp;
}

/**
 *  Returning transposed matrix
 **/
Matrix44 Matrix44::t() const
{
   return Matrix44(
       a(0,0), a(1,0), a(2,0), a(3,0),
       a(0,1), a(1,1), a(2,1), a(3,1),
       a(0,2), a(1,2), a(2,2), a(3,2),
       a(0,3), a(1,3), a(2,3), a(3,3)
   );
}
Matrix44 Matrix44::transposed() const
{
    return t();
}

/**
 *  Frobenius Norm
 * \f[ \|A\|_F =
 *       \sqrt{\sum_{i=1}^m\sum_{j=1}^n |a_{ij}|^2}=
 *       \sqrt{tr(A^* A)} =
 *       \sqrt{\sum_{i=1}^{\min\{m,\,n\}} \sigma_{i}^2}
 * \f]
 */
double Matrix44::frobeniusNorm() const
{
    return sqrt(sumAllElementsSq());
}

Matrix44 Matrix44::Identity()
{
    return Matrix44 (1.0);
}

FixedVector<double, 4> operator *(const Matrix44 &m, const FixedVector<double, 4> &v)
{
    FixedVector<double, 4> toReturn;
    toReturn[0] = v[0] * m.a(0,0) + v[1] * m.a(0,1) + v[2] * m.a(0,2) + m.a(0,3) * v[3];
    toReturn[1] = v[0] * m.a(1,0) + v[1] * m.a(1,1) + v[2] * m.a(1,2) + m.a(1,3) * v[3];
    toReturn[2] = v[0] * m.a(2,0) + v[1] * m.a(2,1) + v[2] * m.a(2,2) + m.a(2,3) * v[3];
    toReturn[3] = v[0] * m.a(3,0) + v[1] * m.a(3,1) + v[2] * m.a(3,2) + m.a(3,3) * v[3];
    return toReturn;
}

Vector4dd operator *(const Matrix44 &m, const Vector4dd &v)
{
    return Vector4dd( m * FixedVector<double, 4>(v.element));
}

Vector3dd operator *(const Matrix44 &m, const Vector3dd &v)
{
    FixedVector<double, 4> v4;
    v4[0] = v.x();
    v4[1] = v.y();
    v4[2] = v.z();
    v4[3] = 1.0;
    FixedVector<double, 4> r = m * v4;
    return Vector3dd(r[0],r[1],r[2]) / r[3];
}




/**
 *  Terribly ineffective.
 *
 *
 * */
Matrix44 Matrix44::inverted() const
{
    Matrix44 toReturn = Matrix44 (
        a(1,2)*a(2,3)*a(3,1) - a(1,3)*a(2,2)*a(3,1) + a(1,3)*a(2,1)*a(3,2) - a(1,1)*a(2,3)*a(3,2) - a(1,2)*a(2,1)*a(3,3) + a(1,1)*a(2,2)*a(3,3),
        a(0,3)*a(2,2)*a(3,1) - a(0,2)*a(2,3)*a(3,1) - a(0,3)*a(2,1)*a(3,2) + a(0,1)*a(2,3)*a(3,2) + a(0,2)*a(2,1)*a(3,3) - a(0,1)*a(2,2)*a(3,3),
        a(0,2)*a(1,3)*a(3,1) - a(0,3)*a(1,2)*a(3,1) + a(0,3)*a(1,1)*a(3,2) - a(0,1)*a(1,3)*a(3,2) - a(0,2)*a(1,1)*a(3,3) + a(0,1)*a(1,2)*a(3,3),
        a(0,3)*a(1,2)*a(2,1) - a(0,2)*a(1,3)*a(2,1) - a(0,3)*a(1,1)*a(2,2) + a(0,1)*a(1,3)*a(2,2) + a(0,2)*a(1,1)*a(2,3) - a(0,1)*a(1,2)*a(2,3),
        a(1,3)*a(2,2)*a(3,0) - a(1,2)*a(2,3)*a(3,0) - a(1,3)*a(2,0)*a(3,2) + a(1,0)*a(2,3)*a(3,2) + a(1,2)*a(2,0)*a(3,3) - a(1,0)*a(2,2)*a(3,3),
        a(0,2)*a(2,3)*a(3,0) - a(0,3)*a(2,2)*a(3,0) + a(0,3)*a(2,0)*a(3,2) - a(0,0)*a(2,3)*a(3,2) - a(0,2)*a(2,0)*a(3,3) + a(0,0)*a(2,2)*a(3,3),
        a(0,3)*a(1,2)*a(3,0) - a(0,2)*a(1,3)*a(3,0) - a(0,3)*a(1,0)*a(3,2) + a(0,0)*a(1,3)*a(3,2) + a(0,2)*a(1,0)*a(3,3) - a(0,0)*a(1,2)*a(3,3),
        a(0,2)*a(1,3)*a(2,0) - a(0,3)*a(1,2)*a(2,0) + a(0,3)*a(1,0)*a(2,2) - a(0,0)*a(1,3)*a(2,2) - a(0,2)*a(1,0)*a(2,3) + a(0,0)*a(1,2)*a(2,3),
        a(1,1)*a(2,3)*a(3,0) - a(1,3)*a(2,1)*a(3,0) + a(1,3)*a(2,0)*a(3,1) - a(1,0)*a(2,3)*a(3,1) - a(1,1)*a(2,0)*a(3,3) + a(1,0)*a(2,1)*a(3,3),
        a(0,3)*a(2,1)*a(3,0) - a(0,1)*a(2,3)*a(3,0) - a(0,3)*a(2,0)*a(3,1) + a(0,0)*a(2,3)*a(3,1) + a(0,1)*a(2,0)*a(3,3) - a(0,0)*a(2,1)*a(3,3),
        a(0,1)*a(1,3)*a(3,0) - a(0,3)*a(1,1)*a(3,0) + a(0,3)*a(1,0)*a(3,1) - a(0,0)*a(1,3)*a(3,1) - a(0,1)*a(1,0)*a(3,3) + a(0,0)*a(1,1)*a(3,3),
        a(0,3)*a(1,1)*a(2,0) - a(0,1)*a(1,3)*a(2,0) - a(0,3)*a(1,0)*a(2,1) + a(0,0)*a(1,3)*a(2,1) + a(0,1)*a(1,0)*a(2,3) - a(0,0)*a(1,1)*a(2,3),
        a(1,2)*a(2,1)*a(3,0) - a(1,1)*a(2,2)*a(3,0) - a(1,2)*a(2,0)*a(3,1) + a(1,0)*a(2,2)*a(3,1) + a(1,1)*a(2,0)*a(3,2) - a(1,0)*a(2,1)*a(3,2),
        a(0,1)*a(2,2)*a(3,0) - a(0,2)*a(2,1)*a(3,0) + a(0,2)*a(2,0)*a(3,1) - a(0,0)*a(2,2)*a(3,1) - a(0,1)*a(2,0)*a(3,2) + a(0,0)*a(2,1)*a(3,2),
        a(0,2)*a(1,1)*a(3,0) - a(0,1)*a(1,2)*a(3,0) - a(0,2)*a(1,0)*a(3,1) + a(0,0)*a(1,2)*a(3,1) + a(0,1)*a(1,0)*a(3,2) - a(0,0)*a(1,1)*a(3,2),
        a(0,1)*a(1,2)*a(2,0) - a(0,2)*a(1,1)*a(2,0) + a(0,2)*a(1,0)*a(2,1) - a(0,0)*a(1,2)*a(2,1) - a(0,1)*a(1,0)*a(2,2) + a(0,0)*a(1,1)*a(2,2)
   );
   toReturn /= det();
   return toReturn;
}

double Matrix44::trace() const
{
    double sum = 0.0;
    for (int row = 0; row < Matrix44::H ; row++)
    {
        sum += a(row, row);
    }
    return sum;
}

double Matrix44::det() const {
   double det =
       a(0,3)*a(1,2)*a(2,1)*a(3,0) - a(0,2)*a(1,3)*a(2,1)*a(3,0) - a(0,3)*a(1,1)*a(2,2)*a(3,0) + a(0,1)*a(1,3)*a(2,2)*a(3,0)+
       a(0,2)*a(1,1)*a(2,3)*a(3,0) - a(0,1)*a(1,2)*a(2,3)*a(3,0) - a(0,3)*a(1,2)*a(2,0)*a(3,1) + a(0,2)*a(1,3)*a(2,0)*a(3,1)+
       a(0,3)*a(1,0)*a(2,2)*a(3,1) - a(0,0)*a(1,3)*a(2,2)*a(3,1) - a(0,2)*a(1,0)*a(2,3)*a(3,1) + a(0,0)*a(1,2)*a(2,3)*a(3,1)+
       a(0,3)*a(1,1)*a(2,0)*a(3,2) - a(0,1)*a(1,3)*a(2,0)*a(3,2) - a(0,3)*a(1,0)*a(2,1)*a(3,2) + a(0,0)*a(1,3)*a(2,1)*a(3,2)+
       a(0,1)*a(1,0)*a(2,3)*a(3,2) - a(0,0)*a(1,1)*a(2,3)*a(3,2) - a(0,2)*a(1,1)*a(2,0)*a(3,3) + a(0,1)*a(1,2)*a(2,0)*a(3,3)+
       a(0,2)*a(1,0)*a(2,1)*a(3,3) - a(0,0)*a(1,2)*a(2,1)*a(3,3) - a(0,1)*a(1,0)*a(2,2)*a(3,3) + a(0,0)*a(1,1)*a(2,2)*a(3,3);
   return det;
}

#define DECOMPOSE_EPSILON	0.000001
#define RANKDECOMPOSE(a, b, c, x, y, z)      \
    if ((x) < (y))                   \
    {                               \
        if ((y) < (z))               \
        {                           \
            (a) = 2;                \
            (b) = 1;                \
            (c) = 0;                \
        }                           \
        else                        \
        {                           \
            (a) = 1;                \
                                    \
            if ((x) < (z))           \
            {                       \
                (b) = 2;            \
                (c) = 0;            \
            }                       \
            else                    \
            {                       \
                (b) = 0;            \
                (c) = 2;            \
            }                       \
        }                           \
    }                               \
    else                            \
    {                               \
        if ((x) < (z))               \
        {                           \
            (a) = 2;                \
            (b) = 0;                \
            (c) = 1;                \
        }                           \
        else                        \
        {                           \
            (a) = 0;                \
                                    \
            if ((y) < (z))           \
            {                       \
                (b) = 2;            \
                (c) = 1;            \
            }                       \
            else                    \
            {                       \
                (b) = 1;            \
                (c) = 2;            \
            }                       \
        }                           \
    }            


bool Matrix44::decomposeTRS(Vector3dd& scale, Vector3dd& translate, Matrix33& rotate)
{
	double det;
	double *pScales;
	Vector4dd *ppvBasis[3];
	Vector4dd matTemp[4];

	static const Vector4dd x4 = { 1, 0, 0, 0 };
	static const Vector4dd y4 = { 0, 1, 0, 0 };
	static const Vector4dd z4 = { 0, 0, 1, 0 };
	static const Vector4dd w4 = { 0, 0, 0, 1 };

	uint a, b, c;
	static const Vector4dd *pvCanonicalBasis[3] = {
		&x4,
		&y4,
		&z4
	};

	const Matrix44 m = transposed();

	// Get the translation
	translate = translationPart();

	matTemp[0] = m.row(0);
	matTemp[1] = m.row(1);
	matTemp[2] = m.row(2);
	matTemp[3] = w4;

	ppvBasis[0] = &matTemp[0];
	ppvBasis[1] = &matTemp[1];
	ppvBasis[2] = &matTemp[2];

	pScales = scale.element;

	pScales[0] = ppvBasis[0]->l2Metric();
	pScales[1] = ppvBasis[1]->l2Metric();
	pScales[2] = ppvBasis[2]->l2Metric();

	RANKDECOMPOSE(a, b, c, pScales[0], pScales[1], pScales[2])
		if (pScales[a] < DECOMPOSE_EPSILON)
		{
			*ppvBasis[a] = *pvCanonicalBasis[a];
		}

	*ppvBasis[a] = Vector4dd(ppvBasis[a]->xyz().normalised(), 0.0f);

	if (pScales[b] < DECOMPOSE_EPSILON)
	{
		uint aa, bb, cc;

        double fAbsX = abs(ppvBasis[a]->x());
        double fAbsY = abs(ppvBasis[a]->y());
        double fAbsZ = abs(ppvBasis[a]->z());

		RANKDECOMPOSE(aa, bb, cc, fAbsX, fAbsY, fAbsZ);

        CORE_UNUSED(aa);
        CORE_UNUSED(bb);

		*ppvBasis[b] = Vector4dd(ppvBasis[a]->xyz() ^ pvCanonicalBasis[cc]->xyz(), 0);
	}

	*ppvBasis[b] = Vector4dd(ppvBasis[b]->xyz().normalised(), 0);

	if (pScales[c] < DECOMPOSE_EPSILON)
	{
		*ppvBasis[c] = Vector4dd(ppvBasis[a]->xyz() ^ ppvBasis[b]->xyz(), 0);
	}

	*ppvBasis[c] = Vector4dd(ppvBasis[c]->xyz().normalised(), 0.0f);

	det = Matrix44(matTemp[0], matTemp[1], matTemp[2], matTemp[3]).det();

	// use Kramer's rule to check for  handedness of coordinate system
	if (det < 0.0f)
	{
		// switch coordinate system by negating the scale and inverting the basis vector on the x-axis
		pScales[a] = -pScales[a];
		*ppvBasis[a] = *ppvBasis[a] * -1.0f;

		det = -det;
	}

	det -= 1.0f;
	det *= det;

	if (DECOMPOSE_EPSILON < det)
	{
		//		Non-SRT matrix encountered
		return false;
	}

	// generate the quaternion from the matrix
	rotate = Matrix44(matTemp[0], matTemp[1], matTemp[2], matTemp[3]).transposed().topLeft33();
	return true;
}

#undef DECOMPOSE_EPSILON
#undef RANKDECOMPOSE

Matrix44::~Matrix44()
{
}

} //namespace corecvs

